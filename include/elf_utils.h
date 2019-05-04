#ifndef ELF_UTILS_H
#define ELF_UTILS_H

#include <elf.h>

#define DYNSTR_NAME ".dynstr"

struct elf_context {
    int fd;
    Elf64_Ehdr header;
    Elf64_Phdr *program_header;
    Elf64_Shdr *section_header;
    Elf64_Shdr *shstrtab;
    Elf64_Shdr *strtab;
    Elf64_Shdr *dynstr;
    Elf64_Ehdr *elf_base;
};

int init_elf(const char *filename, struct elf_context *ctx);
int parse_elf(struct elf_context *ctx);
int mmap_elf_segments(struct elf_context *ctx);
void fix_auxv(struct elf_context *ctx, const char *envp[]);
void fix_argv(const char *argv[]);
void run_elf_entry(struct elf_context *ctx, const char *argv[]);

#endif
