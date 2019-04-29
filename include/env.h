#ifndef ENV_H
#define ENV_H

#include "log.h"

struct {
    log_level_t log_level;
} env_g;

int init_env();

#endif