/**
 * @file logger.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Engine subsystem for logging runtime information.
 */
#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"

// Type definition for log elevation.
typedef enum
{
    LOG_FATAL       = 0
,   LOG_ERROR       = 1
,   LOG_WARN        = 2
,   LOG_INFO        = 3
,   LOG_DEBUG       = 4
,   LOG_TRACE       = 5
,   LOG_SILENT      = 6

,   LOG_LEVEL_COUNT = 7
}
LOG_LEVEL;

// Enable log elevation? Y/N
#define LOG_WARN_ENABLED    1
#define LOG_INFO_ENABLED    1
#define LOG_DEBUG_ENABLED   1
#define LOG_TRACE_ENABLED   1
#define LOG_SILENT_ENABLED  1

// Auto-disable DEBUG and TRACE level logging for release builds.
#if VERSION_RELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

// Defines logger output message prefixes.
#define LOG_LEVEL_PREFIX_FATAL  "[FATAL]\t"
#define LOG_LEVEL_PREFIX_ERROR  "[ERROR]\t"
#define LOG_LEVEL_PREFIX_WARN   "[WARN]\t"
#define LOG_LEVEL_PREFIX_INFO   "[INFO]\t"
#define LOG_LEVEL_PREFIX_DEBUG  "[DEBUG]\t"
#define LOG_LEVEL_PREFIX_TRACE  "[TRACE]\t"
#define LOG_LEVEL_PREFIX_SILENT "\t\t"

// Defines logger output message colors (for pretty-print in console).
#define LOG_LEVEL_COLOR_FATAL  ANSI_CC ( ANSI_CC_BG_DARK_RED )
#define LOG_LEVEL_COLOR_ERROR  ANSI_CC ( ANSI_CC_FG_RED )
#define LOG_LEVEL_COLOR_WARN   ANSI_CC ( ANSI_CC_FG_YELLOW )
#define LOG_LEVEL_COLOR_INFO   ANSI_CC ( ANSI_CC_FG_DARK_GREEN )
#define LOG_LEVEL_COLOR_DEBUG  ANSI_CC ( ANSI_CC_FG_GRAY )
#define LOG_LEVEL_COLOR_TRACE  ANSI_CC ( ANSI_CC_FG_DARK_YELLOW )

/**
 * @brief Initializes the logger subsystem. Call once to read the memory
 * requirement. Call again passing in a state pointer.
 * @param memory_requirement Output buffer to read memory requirement.
 * @param state Pass 0 to read memory requirement. Otherwise, pass a buffer.
 * @return false on error, true otherwise.
 */
bool
logger_startup
(   u64*    memory_requirement
,   void*   state
);

/**
 * @brief Terminates the logger subsystem.
 * @param state .
 */
void
logger_shutdown
(   void* state
);

/**
 * @brief Logs a message according to the logging elevation protocol for the
 * specified log elevation.
 * @param lvl The log elevation.
 * @param mesg Formatted message to log.
 */
void
LOG
(   const LOG_LEVEL lvl
,   const char*     mesg
,   ...
);

// Fatal.
#define LOGFATAL(msg,...)  ( LOG ( LOG_FATAL , (msg) , ##__VA_ARGS__ ) )

// Error.
#define LOGERROR(msg,...)  ( LOG ( LOG_ERROR , (msg) , ##__VA_ARGS__ ) )

// Warn.
#if LOG_WARN_ENABLED == 1
#define LOGWARN(msg,...)  ( LOG ( LOG_WARN , (msg) , ##__VA_ARGS__ ) )
#else
#define LOGWARN(msg,...)
#endif

// Info.
#if LOG_INFO_ENABLED == 1
#define LOGINFO(msg,...)  ( LOG ( LOG_INFO , (msg) , ##__VA_ARGS__ ) )
#else
#define LOGINFO(msg,...)
#endif

// Debug.
#if LOG_DEBUG_ENABLED == 1
#define LOGDEBUG(msg,...)  ( LOG ( LOG_DEBUG , (msg) , ##__VA_ARGS__ ) )
#else
#define LOGDEBUG(msg,...)
#endif

// Trace.
#if LOG_TRACE_ENABLED == 1
#define LOGTRACE(msg,...)  ( LOG ( LOG_TRACE , (msg) , ##__VA_ARGS__ ) )
#else
#define LOGTRACE(msg,...)
#endif

// Silent.
#if LOG_SILENT_ENABLED == 1
#define LOGSILENT(msg,...)  ( LOG ( LOG_SILENT , (msg) , ##__VA_ARGS__ ) )
#else
#define LOGSILENT(msg,...)
#endif

#endif  // LOGGER_H
