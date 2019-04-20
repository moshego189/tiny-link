#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <malloc.h>
#include <elf.h>
#include <elf_utils.h>

int get_elf_fd(const char *filename) 
{
    int fd = open(filename, O_RDONLY|O_CLOEXEC); 
    
    if (0 > fd) {
        perror("error opening file");
    } else {
        printf("opened file %s, fd is %d\n", filename, fd);
    }
    
    return fd; 
}

static int parse_elf_header(struct elf_data *elf, int fd)
{
    if (0 > lseek(fd, 0, SEEK_SET)) {
        perror("error seeking file");
        return 0; 
    }

    if (0 > read(fd, &(elf->header), sizeof(elf->header))) {
        perror("error reading file"); 
        return -1;
    }

    if (ELFMAG0 != elf->header.e_ident[EI_MAG0] ||
        ELFMAG1 != elf->header.e_ident[EI_MAG1] ||
        ELFMAG2 != elf->header.e_ident[EI_MAG2] ||
        ELFMAG3 != elf->header.e_ident[EI_MAG3]) {
        fprintf(stderr, "elf magic is wrong\n");
        return -1;
    }

    if (ELFCLASS64 != elf->header.e_ident[EI_CLASS]) {
        fprintf(stderr, "elf is non 64bit arch\n");
        return -1;
    }

    if (ET_EXEC != elf->header.e_type && 
        ET_DYN != elf->header.e_type) {
        fprintf(stderr, "elf is neither exeutable nor so\n");
        return -1;
    }

    if (EM_X86_64 != elf->header.e_machine) {
        fprintf(stderr, "elf is non x86_64 arch\n");
        return -1;
    }

    if (0 == elf->header.e_entry) {
        fprintf(stderr, "elf has no entry point\n");
    }

    return 0;
}

static int parse_elf_ph(struct elf_data *elf, int fd)
{
    if (0 == elf->header.e_phoff) {
        fprintf(stderr, "elf has no program header\n");
        return -1; 
    }

    if (0 == elf->header.e_phnum) {
        fprintf(stderr, "elf has 0 program header entries\n");
        return -1; 
    }

    printf("allocting ph, phnum=%d\n", elf->header.e_phnum);  
    uint32_t ph_size = (sizeof(Elf64_Phdr) * elf->header.e_phnum); 
    elf->ph = malloc(ph_size);
    
    if (0 == elf->ph) {
        perror("error allocte program header");
        return -1;
    }

    if (0 > lseek(fd, elf->header.e_phoff , SEEK_SET)) {
        perror("error seeking file");
        goto error;
    }

    if (0 > read(fd, elf->ph, ph_size)) {
        perror("error reading file"); 
        goto error;
    }

    return 0;

error:
    free(elf->ph); 
    return -1; 
}

int parse_elf(struct elf_data *elf, int fd) 
{
    if (0 > parse_elf_header(elf, fd)) {
        return -1;
    }

    if (0 > parse_elf_ph(elf, fd)) {
        return -1;
    }

    return 0; 
} 

int mmap_elf_segments(struct elf_data * elf, int fd)
{
    for(int i = 0; i < elf->header.e_phnum; i++) {
        printf("parsing segment %d, type=0x%08x\n", i, 
               elf->ph[i].p_type);
    }

    return 0;
}
