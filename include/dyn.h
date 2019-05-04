#ifndef DYN_H
#define DYN_H

#include <elf.h>
#include "elf_utils.h"

int is_dynamic(struct elf_context *ctx);
const char* elf_dyn_string_parse(struct elf_context *ctx, int offset);
int find_needed_objects(struct elf_context *ctx);
int handle_needed_object(const char *path);

#endif
