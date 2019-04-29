#include <stdlib.h>

#include "env.h"
#include "log.h"

static const char* get_env_or_default(const char* name, const char *default_value);

int init_env() 
{    
    const char* log_level_str = get_env_or_default("LOG_LEVEL", "INFO");
    if (0 > str_to_log_level(log_level_str, &env_g.log_level)) {
        log_fatal("Unrecognized log level: %s", log_level_str);
        goto error;
    }

    return 0;

error:
    return -1;
}

static const char* get_env_or_default(const char* name, const char *default_value)
{
    const char* env = getenv(name);
    if (NULL == getenv(name)) {
        return default_value;
    }

    return env;
}