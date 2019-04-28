#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <malloc.h>
#include <elf.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/user.h>

#include "elf_utils.h"
#include "align.h"
#include "log.h"

static int mmap_pt_load(struct elf_context *ctx, Elf64_Phdr *phdr);
static int parse_elf_header(struct elf_context *ctx);
static int parse_elf_program_header(struct elf_context *ctx);

int init_elf(const char *filename, struct elf_context *ctx)
{
    int fd = open(filename, O_RDONLY|O_CLOEXEC);  
    if (0 > fd) {
        log_errno("open()");
        goto error;
    } 

    log_debug("Opened file %s, fd is %d", filename, fd);
    ctx->fd = fd; 

    return 0;

error:
    return -1; 
}

int parse_elf(struct elf_context *ctx) 
{
    if (0 > parse_elf_header(ctx)) {
        goto error;
    }

    if (0 > parse_elf_program_header(ctx)) {
        goto error;
    }

    return 0;

error:
    return -1;
}

static int parse_elf_header(struct elf_context *ctx)
{
    if (0 > lseek(ctx->fd, 0, SEEK_SET)) {
        log_errno("lseek()");
        goto error;
    }

    if (0 > read(ctx->fd, &(ctx->header), sizeof(ctx->header))) {
        log_errno("read()");
        goto error;
    }

    if (ELFMAG0 != ctx->header.e_ident[EI_MAG0] ||
        ELFMAG1 != ctx->header.e_ident[EI_MAG1] ||
        ELFMAG2 != ctx->header.e_ident[EI_MAG2] ||
        ELFMAG3 != ctx->header.e_ident[EI_MAG3]) {
        log_error("Elf magic is wrong");
        goto error;
    }

    if (ELFCLASS64 != ctx->header.e_ident[EI_CLASS]) {
        log_error("Elf is non 64bit binary");
        goto error;
    }

    if (ET_EXEC != ctx->header.e_type && 
        ET_DYN != ctx->header.e_type) {
        log_error("Elf is neither exe nor so");
        goto error;
    }

    if (EM_X86_64 != ctx->header.e_machine) {
        log_error("Elf is non x86_64 arch");
        goto error;
    }

    if (0 == ctx->header.e_entry) {
        log_error("Elf has no entry point");
        goto error;
    }

    return 0;

error:
    return -1;
}

static int parse_elf_program_header(struct elf_context *ctx)
{
    if (0 == ctx->header.e_phoff) {
        log_error("Elf has no program header");
        goto error;
    }

    if (0 == ctx->header.e_phnum) {
        log_error("Elf has no program header entries");
        goto error; 
    }

    uint32_t phsize = sizeof(Elf64_Phdr) * ctx->header.e_phnum;
    log_debug("Allocting program header, phnum=%d, phsize=%d", ctx->header.e_phnum, phsize);  
    
    ctx->program_header = malloc(phsize);
    if (NULL == ctx->program_header) {
        log_errno("malloc()");
        goto error;
    }

    if (0 > lseek(ctx->fd, ctx->header.e_phoff , SEEK_SET)) {
        log_errno("lseek()");
        goto error;
    }

    if (0 > read(ctx->fd, ctx->program_header, phsize)) {
        log_errno("read()");
        goto error;
    }

    return 0;

error:
    if (NULL != ctx->program_header) {
        free(ctx->program_header); 
        ctx->program_header = NULL;
    }

    return -1; 
}

int mmap_elf_segments(struct elf_context *ctx)
{
    for (int i = 0; i < ctx->header.e_phnum; i++) {
        log_trace("Parsing segment %d, type=0x%08x", i, ctx->program_header[i].p_type);

        if (PT_LOAD == ctx->program_header[i].p_type) {
            if (0 > mmap_pt_load(ctx, &ctx->program_header[i])) {
                goto error;
            }
        }
    }

    return 0;

error:
    return -1;
}

static int mmap_pt_load(struct elf_context *ctx, Elf64_Phdr *phdr) 
{
    log_debug("PT_LOAD: offset=%p, vaddr=%p, filesz=%p, memsz=%p, flags=%p",
              phdr->p_offset, phdr->p_vaddr, phdr->p_filesz, phdr->p_memsz, phdr->p_flags);

    uint64_t vaddr_aligned = ALIGN_DOWN(phdr->p_vaddr, PAGE_SIZE);
    uint64_t start_offset_aligned = ALIGN_DOWN(phdr->p_offset, PAGE_SIZE);
    uint64_t end_offset_aligned = ALIGN_UP(phdr->p_offset + phdr->p_filesz, PAGE_SIZE);
    uint64_t filesz_aligned = end_offset_aligned - start_offset_aligned;

    if (MAP_FAILED == mmap((void *)vaddr_aligned, 
                            filesz_aligned, 
                            phdr->p_flags,
                            MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 
                            ctx->fd, 
                            start_offset_aligned)) {
        log_errno("mmap()");
        goto error;     
    }

    if (0 == start_offset_aligned) {
        log_debug("Elf base is at 0x%x", vaddr_aligned);
        ctx->elf_base = (Elf64_Ehdr *)vaddr_aligned;
    }

    uint64_t mmap_physical_end = vaddr_aligned + filesz_aligned;
    uint64_t mmap_virtual_end = phdr->p_vaddr + phdr->p_memsz;
    if (mmap_virtual_end <= mmap_physical_end) {
        return 0;
    }

    uint64_t mmap_virtual_size = mmap_virtual_end - mmap_physical_end;
    if (MAP_FAILED == mmap((void *)mmap_physical_end, 
                            mmap_virtual_size, 
                            phdr->p_flags,
                            MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, 
                            -1, 
                            0)) {
        log_errno("mmap()");
        goto error;     
    }

    return 0;

error:
    return -1;
}

void fix_auxv(struct elf_context *ctx, const char *envp[]) 
{
    Elf32_auxv_t *auxv;
    while (*envp++ != NULL);

    for (auxv = (Elf32_auxv_t *)envp; auxv->a_type != AT_NULL; auxv++) {
        if (auxv->a_type == AT_PHDR) {
            auxv->a_un.a_val = (uint64_t)ctx->elf_base + ctx->header.e_phoff;
        }
        else if (auxv->a_type == AT_PHNUM) {
            auxv->a_un.a_val = ctx->header.e_phnum;
        }
    }
}

void fix_argv(const char *argv[])
{   
    uint64_t *stack = (uint64_t *)argv;
    stack[0] = stack[-1] - 1;
}

void run_elf_entry(struct elf_context *ctx, const char *argv[]) 
{
    __asm__(
        "mov    %0,%%rsp;"
        "mov    %1,%%rbx;"
        "mov    $0,%%rdx;"
        "jmp    *%%rbx;"
        :: "r" (argv), "r" (ctx->header.e_entry)
    );
}