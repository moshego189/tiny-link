#ifndef ELF_UTILS
#define ELF_UTILS

#include <elf.h>

struct elf_context {
    int fd;
    Elf64_Ehdr header;
    Elf64_Phdr *program_header;
};

int init_elf(const char *filename, struct elf_context *ctx);
int parse_elf(struct elf_context *ctx);
int mmap_elf_segments(struct elf_context *ctx);
void run_elf_entry(struct elf_context *ctx);

#endif
