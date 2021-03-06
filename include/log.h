#ifndef DEBUG_H
#define DEBUG_H

#include <errno.h>
#include <string.h>

typedef enum { 
    LOG_TRACE, 
    LOG_DEBUG, 
    LOG_INFO, 
    LOG_WARN, 
    LOG_ERROR, 
    LOG_FATAL 
} log_level_t;

void log_log(log_level_t level, const char *file, int line, const char *fmt, ...);
int str_to_log_level(const char *str, log_level_t *level) ;

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#define log_errno(fmt, ...) log_error("%s: " fmt, strerror(errno), ##__VA_ARGS__)

#endif