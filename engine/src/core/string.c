/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file string.c
 * @brief Implementation of the string header.
 * (see string.h for additional details)
 */
#include "core/string.h"

#include "core/memory.h"

#include "math/math.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

u64
string_length
(   const char* s
)
{
    return strlen ( s );
}

u64
string_length_clamp
(   const char* s
,   const u64   lim
)
{
    return strnlen ( s , lim );
}

char*
string_allocate_from
(   const char* s
)
{
    const u64 size = string_length ( s ) + 1;
    char* cpy = memory_allocate ( size , MEMORY_TAG_STRING );
    return memory_copy ( cpy , s , size );
}

void
string_free
(   char* s
)
{
    memory_free ( s , string_length ( s ) + 1 , MEMORY_TAG_STRING );
}

bool
string_equal
(   const char* s1
,   const char* s2
)
{
    return !strcmp ( s1 , s2 );
}

i32
string_format
(   char*       dst
,   const char* fmt
,   ...
)
{
    __builtin_va_list args;
    va_start ( args , fmt );
    const i32 result = string_format_v ( dst , fmt , args );
    va_end ( args );
    return result;
}

i32
string_format_v
(   char*       dst
,   const char* fmt
,   void*       args
)
{
    char buf[ STACK_STRING_MAX_LENGTH ];
    const i32 result = vsnprintf ( buf , STACK_STRING_MAX_LENGTH
                                 , fmt , args
                                 );
    buf[ result ] = 0;
    memory_copy ( dst , buf , result + 1 );
    return result;
}

const char*
string_bytesize
(   u64     size
,   f32*    amt
)
{
    if ( size >= GIBIBYTES ( 1 ) )
    {
        *amt = ( f64 ) size / GIBIBYTES ( 1 );
        return "GiB";
    }
    if ( size >= MEBIBYTES ( 1 ) )
    {
        *amt = ( f64 ) size / MEBIBYTES ( 1 );
        return "MiB";
    }
    if ( size >= KIBIBYTES ( 1 ) )
    {
        *amt = ( f64 ) size / KIBIBYTES ( 1 );
        return "KiB";
    }
    *amt = ( f32 ) size;
    return "B";
}

char*
string_trim
(   char* s
)
{
    const u64 len = string_length ( s );
    u64 i;
    
    // Calculate index of first non-whitespace character.
    for ( i = 0; i < len && whitespace ( s[ i ] ); ++i );
    const char* from = s + i;

    // Calculate index of final non-whitespace character.
    for ( i = len; i > 0 && whitespace ( s[ i - 1 ] ); --i );
    const char* to = s + i;

    // Copy memory range in-place.
    const u64 size = max ( 0 , to - from );
    memory_move ( s , from , size );
    s[ size ] = 0; // Append terminator.
    
    return s;
}
