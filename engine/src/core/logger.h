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

,   LOG_LEVEL_COUNT = 6
}
LOG_LEVEL;

// Definition of logger prefix strings.
#define LOG_LEVEL_PREFIXES  \
    { "[FATAL]\t"           \
    , "[ERROR]\t"           \
    , "[WARN]\t"            \
    , "[INFO]\t"            \
    , "[DEBUG]\t"           \
    , "[TRACE]\t"           \
    }

// Enable log elevation? Y/N
#define LOG_WARN_ENABLED    1
#define LOG_INFO_ENABLED    1
#define LOG_DEBUG_ENABLED   1
#define LOG_TRACE_ENABLED   1

// Auto-disable DEBUG and TRACE level logging for release builds.
#if VERSION_RELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

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
 * @param msg Formatted message to log.
 */
void
LOG
(   const LOG_LEVEL lvl
,   const char*     msg
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

#endif  // LOGGER_H
