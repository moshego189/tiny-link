#include <stdio.h>
#include <stdarg.h>

#include "log.h"
#include "env.h"

static const char *level_names[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

static const char *level_colors[] = {
    "\x1b[94m",
    "\x1b[36m",
    "\x1b[32m",
    "\x1b[33m",
    "\x1b[31m",
    "\x1b[35m"
};

void log_log(log_level_t level, const char *file, int line, const char *fmt, ...)
{
    if (level < env_g.log_level) {
        return;
    }

    fprintf(stderr, "%s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", level_colors[level], level_names[level], file, line);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
    fflush(stderr);
}

int str_to_log_level(const char *str, log_level_t *level) 
{
    for (int i = 0; i < LOG_FATAL + 1; i++) {
        if (0 == strcmp(str, level_names[i])) {
            *level = i;
            return 0;
        }
    }

    return -1;
}