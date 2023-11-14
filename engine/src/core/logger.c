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

// Type definition for logger subsystem state.
typedef struct
{
    file_handle_t   file;
}
state_t;

// Global subsystem state.
static state_t* state;

/**
 * @brief Appends a message to the log file.
 * @param msg The message to append.
 */
void
logger_file_append
(   const char* msg
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
        platform_console_write_error ( "logger_startup: Unable to open '"LOG_FILEPATH"' for writing."
                                     , LOG_ERROR
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
,   const char*     msg
,   ...
)
{
    const char* prefix[] = { "[FATAL]\t"
                           , "[ERROR]\t"
                           , "[WARN]\t"
                           , "[INFO]\t"
                           , "[DEBUG]\t"
                           , "[TRACE]\t"
                           };
    const bool err = lvl < LOG_WARN;
  
    char buf[ LOG_MSG_MAX_LENGTH ];
    memory_clear ( buf , sizeof ( char ) * LOG_MSG_MAX_LENGTH );
    
    __builtin_va_list args;
    va_start ( args , msg );
    string_format_v ( buf , msg , args );
    va_end ( args );

    if ( err )
    {
        platform_console_write_error ( prefix[ lvl ] , lvl );
        platform_console_write_error ( buf , lvl );
        platform_console_write_error ( "\n" , LOG_LEVEL_COUNT );        
    }
    else if ( lvl == LOG_DEBUG )
    {
        platform_console_write ( prefix[ lvl ] , lvl );
        platform_console_write ( buf , LOG_LEVEL_COUNT + 1 );        
        platform_console_write ( "\n" , LOG_LEVEL_COUNT );        
    }
    else
    {
        platform_console_write ( prefix[ lvl ] , lvl );
        platform_console_write ( buf , LOG_LEVEL_COUNT  );        
        platform_console_write ( "\n" , LOG_LEVEL_COUNT ); 
    }

    string_format ( buf , "%s%s\n" , prefix[ lvl ] , buf );    
    logger_file_append ( buf );
}

void
logger_file_append
(   const char* msg
)
{
    if ( !state || !( *state ).file.valid )
    {
        return;
    }
    
    const u64 len = string_length ( msg );
    u64 written = 0;
    
    if ( !file_write ( &( *state ).file , len , msg , &written ) )
    {
        platform_console_write ( "Error writing to '"LOG_FILEPATH"'."
                               , LOG_ERROR
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
