/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file logger.c
 * @brief Implementation of the logger and assert headers.
 * (see logger.h and assert.h for additional details)
 */
#include "core/assert.h"
#include "core/logger.h"

#include "core/memory.h"
#include "core/string.h"

#include "platform/platform.h"
#include "platform/filesystem.h"

#include <stdarg.h>
#include <stdio.h>

// Global constants.
#define LOG_MSG_MAX_LENGTH  32000
#define LOG_FILEPATH        "console.log"

// Defines logger output message prefixes.
static const char* log_level_prefixes[] = { LOG_LEVEL_PREFIX_FATAL
                                          , LOG_LEVEL_PREFIX_ERROR
                                          , LOG_LEVEL_PREFIX_WARN
                                          , LOG_LEVEL_PREFIX_INFO
                                          , LOG_LEVEL_PREFIX_DEBUG
                                          , LOG_LEVEL_PREFIX_TRACE
                                          , LOG_LEVEL_PREFIX_SILENT
                                          };

// Defines logger output message colors.
static const char* log_level_colors[] = { LOG_LEVEL_COLOR_FATAL
                                        , LOG_LEVEL_COLOR_ERROR
                                        , LOG_LEVEL_COLOR_WARN
                                        , LOG_LEVEL_COLOR_INFO
                                        , LOG_LEVEL_COLOR_DEBUG
                                        , LOG_LEVEL_COLOR_TRACE
                                        };

// Type definition for logger subsystem state.
typedef struct
{
    file_handle_t file;
}
state_t;

// Global subsystem state.
static state_t* state;



/**
 * @brief Appends a message to the log file.
 * @param mesg The message to append.
 */
void
logger_file_append
(   const char* mesg
);

bool
logger_startup
(   u64*    memory_requirement
,   void*   state_
)
{
    *memory_requirement = sizeof ( state_t );

    if ( !state_ )
    {
        return true;
    }

    state = state_;
    platform_memory_clear ( state , sizeof ( state_t ) );

    // Initialize log file.
    if ( !file_open ( LOG_FILEPATH
                    , FILE_MODE_WRITE
                    , false
                    , &( *state ).file
                    ))
    {
        platform_console_write_error ( LOG_LEVEL_COLOR_ERROR
                                       "logger_startup: Unable to open '"LOG_FILEPATH"' for writing."
                                       ANSI_CC_RESET "\n"
                                     );
        return false;
    }

    return true;
}

void
logger_shutdown
(   void* state_
)
{
    state = 0;
}

void
LOG
(   const LOG_LEVEL lvl
,   const char*     mesg
,   ...
)
{
    const bool err = lvl < LOG_WARN;
    const bool colored = lvl != LOG_INFO;

    char buf[ LOG_MSG_MAX_LENGTH ];
    memory_clear ( buf , sizeof ( char ) * LOG_MSG_MAX_LENGTH );
    
    __builtin_va_list args;
    va_start ( args , mesg );
    string_format_v ( buf , mesg , args );
    va_end ( args );

    // Write raw string to log file.
    string_format ( buf , "%s%s" , log_level_prefixes[ lvl ] , buf );    
    logger_file_append ( buf );
    if ( lvl == LOG_SILENT )
    {
        return;
    }

    // Write formatted string to console.
    string_format ( buf
                  , "%s%s%s%s" ANSI_CC_RESET "\n"
                  , log_level_colors[ lvl ] , log_level_prefixes[ lvl ]
                  , ( colored ) ? "" : ANSI_CC_RESET
                  , buf + string_length ( log_level_prefixes[ lvl ] )
                  );
    ( err ) ? platform_console_write_error ( buf )
            : platform_console_write ( buf );
}

void
logger_file_append
(   const char* mesg
)
{
    if ( !state || !( *state ).file.valid )
    {
        return;
    }
    
    const char newline = '\n';
    const u64 len = string_length ( mesg ) + 1;
    u64 written = 0;
    
    if (   !file_write ( &( *state ).file , len , mesg , &written )
        || !file_write ( &( *state ).file , sizeof ( newline ) , &newline , &written )
       )
    {
        platform_console_write ( LOG_LEVEL_COLOR_ERROR
                                 "logger_file_append: Error writing to log file '"LOG_FILEPATH"'."
                                 ANSI_CC_RESET "\n"
                               );
    }
}

void
assertf
(   const char* expr
,   const char* mesg
,   const char* file
,   const i32   line
)
{
    if ( *mesg == '\0' )
    {
        LOG ( LOG_FATAL
            , "Assertion failure in file %s (line %d): %s"
            , file , line , expr
            );
    }
    else
    {
        LOG ( LOG_FATAL
            , "Assertion failure in file %s (line %d): %s\n\tMessage: %s"
            , file , line , expr , mesg
            );
    }
}
