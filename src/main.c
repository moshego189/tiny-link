#include <stdio.h>
#include "elf_utils.h"

int main(int argc, const char *argv[]) 
{
    if (2 != argc) {
            fprintf(stderr, "Usage: %s [elf]\n", argv[0]); 
            return 1;
    }

    int fd = get_elf_fd(argv[1]);
    if (0 > fd) {
        return 1;
    }

    struct elf_data elf = {0};
    if (0 > parse_elf(&elf, fd)) {
        return 1;
    }

    if (0 > mmap_elf_segments(&elf, fd)) {
        return 1;
    }

    /*if (0 > handle_relocations()) {
            return 1;
    }

    if (0 > cleaunup_and_execute()) {
            return 1;
    }*/

    fprintf(stderr, "should never reach here\n");
    return 1;
}
