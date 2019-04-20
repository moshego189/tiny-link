#ifndef ELF_UTILS
#define ELF_UTILS

#include <elf.h>

struct elf_data {
    Elf64_Ehdr header;
    Elf64_Phdr * ph;          
};

int get_elf_fd(const char *filename); 
int parse_elf(struct elf_data *elf, int fd); 
int mmap_elf_segments(struct elf_data *elf, int fd); 

#endif
