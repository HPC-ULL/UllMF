#ifndef CAL_DEBUG_H
#define CAL_DEBUG_H

#ifndef NDEBUG

#include <stdio.h>

#define DBGLOG(TAG, MSG, ...) fprintf(stderr, "[CAL-" TAG "] (%s:%d) " MSG "%c", __FILE__, __LINE__, __VA_ARGS__)

#else

#define DBGLOG(...)

#endif //NDEBUG

#define dbglog_error(...)  DBGLOG("ERROR", __VA_ARGS__, '\0')
#define dbglog_warn(...)  DBGLOG("WARN", __VA_ARGS__, '\0')
#define dbglog_info(...)  DBGLOG("INFO", __VA_ARGS__, '\0')
#define dbglog_append(...) fprintf(stderr, __VA_ARGS__)

#endif //EML_DEBUG_H
