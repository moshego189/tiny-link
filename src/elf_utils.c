#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <malloc.h>
#include <elf.h>
 #include <sys/mman.h>
#include "elf_utils.h"

static int parse_elf_header(struct elf_context *ctx);
static int parse_elf_program_header(struct elf_context *ctx);

int init_elf(const char *filename, struct elf_context *ctx)
{
    int fd = open(filename, O_RDONLY|O_CLOEXEC); 
    
    if (0 > fd) {
        perror("error opening file");
        return -1;
    } 

    printf("opened file %s, fd is %d\n", filename, fd);
    ctx->fd = fd; 

    return 0; 
}

int parse_elf(struct elf_context *ctx) 
{
    if (0 > parse_elf_header(ctx)) {
        return -1;
    }

    if (0 > parse_elf_program_header(ctx)) {
        return -1;
    }

    return 0; 
}

static int parse_elf_header(struct elf_context *ctx)
{
    if (0 > lseek(ctx->fd, 0, SEEK_SET)) {
        perror("error seeking file");
        return 0; 
    }

    if (0 > read(ctx->fd, &(ctx->header), sizeof(ctx->header))) {
        perror("error reading file"); 
        return -1;
    }

    if (ELFMAG0 != ctx->header.e_ident[EI_MAG0] ||
        ELFMAG1 != ctx->header.e_ident[EI_MAG1] ||
        ELFMAG2 != ctx->header.e_ident[EI_MAG2] ||
        ELFMAG3 != ctx->header.e_ident[EI_MAG3]) {
        fprintf(stderr, "elf magic is wrong\n");
        return -1;
    }

    if (ELFCLASS64 != ctx->header.e_ident[EI_CLASS]) {
        fprintf(stderr, "elf is non 64bit arch\n");
        return -1;
    }

    if (ET_EXEC != ctx->header.e_type && 
        ET_DYN != ctx->header.e_type) {
        fprintf(stderr, "elf is neither exeutable nor so\n");
        return -1;
    }

    if (EM_X86_64 != ctx->header.e_machine) {
        fprintf(stderr, "elf is non x86_64 arch\n");
        return -1;
    }

    if (0 == ctx->header.e_entry) {
        fprintf(stderr, "elf has no entry point\n");
    }

    return 0;
}

static int parse_elf_program_header(struct elf_context *ctx)
{
    if (0 == ctx->header.e_phoff) {
        fprintf(stderr, "elf has no program header\n");
        return -1; 
    }

    if (0 == ctx->header.e_phnum) {
        fprintf(stderr, "elf has 0 program header entries\n");
        return -1; 
    }

    printf("allocting ph, phnum=%d\n", ctx->header.e_phnum);  
    uint32_t ph_size = (sizeof(Elf64_Phdr) * ctx->header.e_phnum); 
    ctx->program_header = malloc(ph_size);
    
    if (0 == ctx->program_header) {
        perror("error allocte program header");
        return -1;
    }

    if (0 > lseek(ctx->fd, ctx->header.e_phoff , SEEK_SET)) {
        perror("error seeking file");
        goto error;
    }

    if (0 > read(ctx->fd, ctx->program_header, ph_size)) {
        perror("error reading file"); 
        goto error;
    }

    return 0;

error:
    free(ctx->program_header); 
    return -1; 
}

int mmap_elf_segments(struct elf_context *ctx)
{
    for(int i = 0; i < ctx->header.e_phnum; i++) {
        printf("parsing segment %d, type=0x%08x\n", i, ctx->program_header[i].p_type);

        if (PT_LOAD == ctx->program_header[i].p_type) {
            Elf64_Addr vaddr = ctx->program_header[i].p_vaddr & -0x1000;
            Elf64_Off offset = ctx->program_header[i].p_offset & -0x1000;
            Elf64_Xword filesz = (ctx->program_header[i].p_filesz + (0x1000 - 1)) & -0x1000; 

            if (MAP_FAILED == mmap((void *)vaddr, filesz, ctx->program_header[i].p_flags,
                                   MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, ctx->fd, offset)) {
                perror("mmap");
                return -1;     
            }

            if (ctx->program_header[i].p_filesz == ctx->program_header[i].p_memsz) {
                continue;
            }

            if (MAP_FAILED == mmap((void *)vaddr + filesz, 
                                   (ctx->program_header[i].p_vaddr + ctx->program_header[i].p_memsz) - (vaddr + filesz),
                                   ctx->program_header[i].p_flags,
                                   MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS,-1, 0)) {
                perror("mmap");
                return -1;     
            }
        }
    };

    return 0;
}

void run_elf_entry(struct elf_context *ctx) 
{
    ((void (*)(void))ctx->header.e_entry)();
}