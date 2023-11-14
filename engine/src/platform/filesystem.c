/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file filesystem.c
 * @brief Implementation of the filesystem header.
 * (see filesystem.h for additional details)
 */
#include "platform/filesystem.h"

#include "core/logger.h"
#include "core/memory.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define FILEBUFFER_MAX_SIZE 32000

bool
file_exists
(   const char* p
)
{
    struct stat buf;
    return !stat ( p , &buf );
}

bool
file_open
(   const char*     p
,   FILE_MODE       mode
,   bool            binary
,   file_handle_t*  f
)
{
    const char* mode_name;
    
    ( *f ).valid = false;
    ( *f ).handle = 0;

    // Interpret argument.
    if ( ( mode & FILE_MODE_READ ) && ( mode & FILE_MODE_WRITE ) )
    {
        mode_name = binary ? "w+b" : "w+";
    }
    else if ( ( mode & FILE_MODE_READ ) && !( mode & FILE_MODE_WRITE ) )
    {
        mode_name = binary ? "rb" : "r";
    }
    else if ( !( mode & FILE_MODE_READ ) && ( mode & FILE_MODE_WRITE ) )
    {
        mode_name = binary ? "wb" : "w";
    }
    else
    {
        LOGERROR ( "file_open: Invalid file mode provided for file '%s'."
                 , p
                 );
        return false;
    }

    // Attempt file open.
    FILE* file = fopen ( p , mode_name );
    if ( !file )
    {
        LOGERROR ( "file_open: Call to fopen failed for file '%s'."
                 , p
                 );
        return false;
    }
    
    ( *f ).handle = file;
    ( *f ).valid = true;

    return true;
}

void
file_close
(   file_handle_t* f
)
{
    if ( !( *f ).handle )
    {
         return;
    }

    FILE* file = ( *f ).handle;
    ( *f ).valid = false;
    fclose ( file );
    ( *f ).handle = 0;
}

bool
file_read_line
(   file_handle_t*  f
,   char**          buf
)
{
    if ( !( *f ).handle )
    {
        return false;
    }

    FILE* file = ( *f ).handle;
    char buf_[ FILEBUFFER_MAX_SIZE ];

    if ( !fgets ( buf_ , FILEBUFFER_MAX_SIZE , file ) )
    {
        return false;
    }

    const u64 len = strlen ( buf_ );
    *buf = memory_allocate ( sizeof ( char ) * len + 1 , MEMORY_TAG_STRING );
    strcpy ( *buf , buf_ );
    return true;
}

bool
file_write_line
(   file_handle_t*  f
,   const char*     s
)
{
    if ( !( *f ).handle )
    {
        return false;
    }
    
    FILE* file = ( *f ).handle;

    i32 result = fputs ( s , file );
    if ( result != EOF )
    {
        result = fputc ( '\n' , file );
    }
    
    fflush ( file );
    return result != EOF;
}

bool
file_read
(   file_handle_t*  f
,   u64             size
,   void*           buf
,   u64*            read
)
{    
    if ( !( *f ).handle || !buf )
    {
        return false;
    }
    
    FILE* file = ( *f ).handle;

    *read = fread ( buf , 1 , size , file );
    return *read == size;
}

bool
file_read_all
(   file_handle_t*  f
,   u8**            buf
,   u64*            read
)
{
    if ( !( *f ).handle || !buf )
    {
        return false;
    }
    
    FILE* file = ( *f ).handle;

    fseek ( file , 0 , SEEK_END );
    u64 size = ftell ( file );
    rewind ( file );

    *buf = memory_allocate ( sizeof ( u8 ) * size , MEMORY_TAG_STRING );
    *read = fread ( *buf , 1 , size , file );
    return *read == size;
    
}

bool
file_write
(   file_handle_t*  f
,   u64             size
,   const void*     data
,   u64*            written
)
{
    if ( !( *f ).handle )
    {
        return false;
    }
    
    FILE* file = ( *f ).handle;
    
    *written = fwrite ( data , 1 , size , file );
    fflush ( file );
    return *written == size;
}

