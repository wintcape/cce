/**
 * @file align.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief .
 */
#ifndef ALIGN_H
#define ALIGN_H

#include "common/inline.h"
#include "common/types.h"

// Type definition for a memory range.
typedef struct
{
    u64 offs;
    u64 size;
}
range_t;

/**
 * @brief .
 * @param operand .
 * @param granularity . 
 * @return .
 */
INLINE
u64
aligned
(   u64 operand
,   u64 granularity
)
{
    return ( ( operand + ( granularity - 1 ) ) & ~( granularity - 1 ) );
}

/**
 * @brief .
 * @param offs .
 * @param size .
 * @param granularity . 
 * @return .
 */
INLINE
range_t
aligned_range
(   u64 offs
,   u64 size
,   u64 granularity
)
{
    return ( range_t ){ aligned ( offs , granularity )
                      , aligned ( size , granularity )
                      };
}

#endif  // ALIGN_H
