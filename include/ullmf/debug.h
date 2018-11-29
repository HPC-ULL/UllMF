#ifndef ULLMF_DEBUG_H
#define ULLMF_DEBUG_H

/**
 * @file
 * Internal debug macros.
 * @ingroup internalapi
 */

#ifdef NDEBUG

/// Internal Macro
#define DBGLOG(...)
/// Internal Macro
#define DBGLOG2(...)

#else

#include <stdio.h>

/// Internal Macro
#define DBGLOG(TAG, MSG, ...) fprintf(stderr, "[ULLMF-" TAG "] (%s:%d) " MSG, __FILE__, __LINE__, ##__VA_ARGS__)
/// Internal Macro
#define DBGLOG2(MSG, ...) fprintf(stderr, MSG, ##__VA_ARGS__)

#endif //NDEBUG

/**
 * Basic debug format to unify debug output
 */
#define DBG_FMT "%8g"

/**
 * Prints an Error message
 */
#define dbglog_error(...)  DBGLOG("ERROR", __VA_ARGS__)

/**
 * Prints a warning message
 */
#define dbglog_warn(...)   DBGLOG("WARN", __VA_ARGS__)

/**
 * Prints an info message
 */
#define dbglog_info(...)   DBGLOG("INFO", __VA_ARGS__)

/**
 * Prints a message without the file, line and tag prefixes
 */
#define dbglog_append(...) DBGLOG2(__VA_ARGS__)

#endif //ULLMF_DEBUG_H
