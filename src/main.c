#include <stdio.h>
#include "elf_utils.h"

int main(int argc, const char *argv[], const char *envp[]) 
{
    if (2 > argc) {
            fprintf(stderr, "Usage: %s [elf] args...\n", argv[0]); 
            return 1;
    }

    struct elf_context ctx = {0};
    if (0 > init_elf(argv[1], &ctx)) {
        return 1;
    }

    if (0 > parse_elf(&ctx)) {
        return 1;
    }

    if (0 > mmap_elf_segments(&ctx)) {
        return 1;
    }

    fix_auxv(&ctx, envp);
    fix_argv(argv);

    run_elf_entry(&ctx, argv);

    // should never get here
    return 1;
}
