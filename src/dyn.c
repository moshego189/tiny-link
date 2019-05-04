#include <elf.h>

#include "dyn.h"
#include "elf_utils.h"
#include "log.h"

static Elf64_Phdr* locate_dynamic(struct elf_context *ctx);

int is_dynamic(struct elf_context *ctx)
{
    // For now the check will be whether .interp segment exist
    for (int i = 0 ; i < ctx->header.e_phnum ; ++i) {
	if (PT_INTERP == ctx->program_header[i].p_type) {
	    return 1;
 	}
    }

    return 0;
}

const char* elf_dyn_string_parse(struct elf_context *ctx, int offset)
{
    if (NULL == ctx->dynstr) {
	log_error("no dynamic string table");
	goto error;
    }

    return (const char*)ctx->dynstr + offset;

error:
    return NULL;
}

static Elf64_Phdr* locate_dynamic(struct elf_context *ctx)
{
    for (int i = 0 ; i < ctx->header.e_phnum ; ++i) {
	if (PT_DYNAMIC == ctx->program_header[i].p_type) {
	    return &ctx->program_header[i];
 	}
    }

    return NULL;
}

int find_needed_objects(struct elf_context *ctx)
{
    Elf64_Dyn *cur;
    Elf64_Phdr *dyn = locate_dynamic(ctx);
    if (NULL == dyn) {
	log_error("failed finding dynamic segment");
	goto error;
    }

    int ret; 

    for (cur = (Elf64_Dyn*)dyn->p_vaddr ; DT_NULL != cur->d_tag ; ++cur) {
	if (DT_NEEDED == cur->d_tag) {
	    log_info("Needed object offset: %d library: %s", cur->d_un.d_val, elf_dyn_string_parse(ctx, cur->d_un.d_val));
	    ret = handle_needed_object(elf_dyn_string_parse(ctx, cur->d_un.d_val));
	    if (ret) {
		return ret;
	    }	
   	}
    }

    return 0;

error:
    return 1;
}

int handle_needed_object(const char *path)
{
    //TODO: implement this mock
    log_debug("mock: %s", path);
    return 0;
}
