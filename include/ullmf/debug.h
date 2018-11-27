#ifndef ULLMF_DEBUG_H
#define ULLMF_DEBUG_H

#ifdef NDEBUG

#define DBGLOG(...)
#define DBGLOG2(...)

#else

#include <stdio.h>

#define DBGLOG(TAG, MSG, ...) fprintf(stderr, "[ULLMF-" TAG "] (%s:%d) " MSG, __FILE__, __LINE__, ##__VA_ARGS__)
#define DBGLOG2(MSG, ...) fprintf(stderr, MSG, ##__VA_ARGS__)

#endif //NDEBUG

#define DBG_FMT "%8g"
#define dbglog_error(...)  DBGLOG("ERROR", __VA_ARGS__)
#define dbglog_warn(...)   DBGLOG("WARN", __VA_ARGS__)
#define dbglog_info(...)   DBGLOG("INFO", __VA_ARGS__)
#define dbglog_append(...) DBGLOG2(__VA_ARGS__)

#endif //ULLMF_DEBUG_H