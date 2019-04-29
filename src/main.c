#include <stdio.h>

#include "elf_utils.h"
#include "log.h"
#include "env.h"

int main(int argc, const char *argv[], const char *envp[]) 
{
    if (2 > argc) {
        fprintf(stderr, "Usage: %s [elf] args...\n", argv[0]); 
        goto error;
    }

    if (0 > init_env()) {
        goto error;
    } 
    
    log_info("Got an Elf file: %s", argv[1]);
    
    struct elf_context ctx = {0};
    if (0 > init_elf(argv[1], &ctx)) {
        goto error;
    }

    log_info("Parsring Elf");
    if (0 > parse_elf(&ctx)) {
        goto error;
    }

    log_info("Loading Elf segments");
    if (0 > mmap_elf_segments(&ctx)) {
        goto error;
    }

    log_info("Fixing argv and auxv");
    fix_argv(argv);
    fix_auxv(&ctx, envp);

    log_info("Running Elf entry");
    run_elf_entry(&ctx, argv);

    // should never get here

error:
    return 1;
}
