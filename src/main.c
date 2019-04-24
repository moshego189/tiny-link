#include <stdio.h>
#include "elf_utils.h"

int main(int argc, const char *argv[]) 
{
    if (2 != argc) {
            fprintf(stderr, "Usage: %s [elf]\n", argv[0]); 
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

    run_elf_entry(&ctx);

    // should never get here
    return 1;
}
