/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file array.c
 * @brief Implementation of the array header.
 * (see array.h for additional details)
 */
#include "container/array.h"

#include "core/logger.h"
#include "core/memory.h"

void*
_array_allocate
(   ARRAY_FIELD icap
,   ARRAY_FIELD stride
)
{
    const u64 header_size = ARRAY_FIELD_COUNT * sizeof ( u64 );
    const u64 content_size = icap * stride;
    const u64 size = header_size + content_size;

    u64* arr = memory_allocate ( size , MEMORY_TAG_DYNAMIC_ARRAY );
    memory_set ( arr , 0 , size );

    arr[ ARRAY_FIELD_CAPACITY ] = icap;
    arr[ ARRAY_FIELD_LENGTH ]   = 0;
    arr[ ARRAY_FIELD_STRIDE ]   = stride;

    return ( void* )( arr + ARRAY_FIELD_COUNT );
}

void
_array_free
(   void* arr
)
{
    const u64 header_size = ARRAY_FIELD_COUNT * sizeof ( u64 );
    u64* header = ( u64* ) arr - ARRAY_FIELD_COUNT;
    memory_free ( header
                , header_size + header[ ARRAY_FIELD_CAPACITY ] * header[ ARRAY_FIELD_STRIDE ]
                , MEMORY_TAG_DYNAMIC_ARRAY
                );
}

u64
_array_field_get
(   void*       arr
,   ARRAY_FIELD field
)
{
    const u64* header = ( u64* ) arr - ARRAY_FIELD_COUNT;
    return header[ field ];
}

void
_array_field_set
(   void*       arr
,   ARRAY_FIELD field
,   u64         value
)
{
    u64* header = ( u64* ) arr - ARRAY_FIELD_COUNT;
    header[ field ] = value;
}

void*
_array_resize
(   void* arr
)
{
    const u64 length = array_length ( arr );
    const u64 stride = array_stride ( arr );

    void* new = _array_allocate ( array_capacity ( arr ) * ARRAY_SCALE_FACTOR
                                , stride
                                );
    memory_copy ( new , arr , length * stride );
    _array_field_set ( new , ARRAY_FIELD_LENGTH , length );

    _array_free ( arr );
    
    return new;
}

void*
_array_push
(   void*       arr
,   const void* elem
)
{
    const u64 length = array_length ( arr );
    const u64 stride = array_stride ( arr );

    if ( length >= array_capacity ( arr ) )
    {
        arr = _array_resize ( arr );
    }

    const u64 addr = ( u64 ) arr + length * stride;
    memory_copy ( ( void* ) addr , elem , stride );
    _array_field_set ( arr , ARRAY_FIELD_LENGTH , length + 1 );

    return arr;
}

void
_array_pop
(   void* arr
,   void* dst
)
{
    const u64 length = array_length ( arr ) - 1;
    const u64 stride = array_stride ( arr );

    const u64 addr = ( u64 ) arr + length * stride;
    memory_copy ( dst , ( void* ) addr , stride );
    _array_field_set ( arr , ARRAY_FIELD_LENGTH , length );
}

void*
_array_insert
(   void*       arr
,   u64         indx
,   const void* elem
)
{
    const u64 length = array_length ( arr );
    const u64 stride = array_stride ( arr );
    
    if ( indx > length )
    {
        LOGERROR ( "_array_insert called with out of bounds index: %i (index) >= %i (array length)."
                 , indx , length
                 );
        return arr;
    }
    
    if ( length >= array_capacity ( arr ) )
    {
        arr = _array_resize ( arr );
    }
    
    const u64 addr = ( u64 ) arr;
    if ( indx < length - 1 )
    {
        memory_copy ( ( void* )( addr + ( indx + 1 ) * stride )
                    , ( void* )( addr + indx * stride )
                    , stride * ( length - indx )
                    );
    }
    memory_copy ( ( void* )( addr + indx * stride ) , elem , stride );

    _array_field_set ( arr , ARRAY_FIELD_LENGTH , length + 1 );

    return arr;
}

void*
_array_remove
(   void*   arr
,   u64     indx
,   void*   dst
)
{
    const u64 length = array_length ( arr );
    const u64 stride = array_stride ( arr );
    
    if ( indx >= length )
    {
        LOGERROR ( "_array_remove called with out of bounds index: %i (index) >= %i (array length)."
                 , indx , length
                 );
        return arr;
    }
    
    const u64 addr = ( u64 ) arr;
    memory_copy ( dst , ( void* )( addr + indx * stride ) , stride );
    if ( indx < length - 1 )
    {
        memory_copy ( ( void* )( addr + indx * stride )
                    , ( void* )( addr + ( indx + 1 ) * stride )
                    , stride * ( length - indx )
                    );
    }

    _array_field_set ( arr , ARRAY_FIELD_LENGTH , length - 1 );

    return arr;
}
