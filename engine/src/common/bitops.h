/**
 * @file bitops.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Functions and preprocessor bindings which implement bit manipulation
 * operations.
 */
#ifndef BITOPS_H
#define BITOPS_H

#include "common/inline.h"
#include "common/types.h"

// Internal preprocessor bindings.
// (Use inline functions instead for type safety)
#define U64_1       ( ( u64 ) 1 )
#define BIT(x,n)    (( (x) >> (n) ) & U64_1 )
#define BITSET(x,n) ( (x) | ( U64_1 << (n) ) )
#define BITCLR(x,n) ( (x) & ~( U64_1 << (n) )) 
#define BITSWP(x,n) ( (x) ^ ( U64_1 << (n) ) )

/**
 * @brief Reads a bit from a bit vector.
 * @param x A bit vector.
 * @param n The bit to read.
 * @return true if bit n of x set, false otherwise.
 */
INLINE
bool
bit
(   const u64   x
,   const u8    n
)
{
    return BIT ( x , n );
}

/**
 * @brief Sets a bit within a bit vector.
 * @param x A bit vector.
 * @param n The bit to set.
 * @return x with bit n set.
 */
INLINE
u64
bitset
(   const u64   x
,   const u8    n
)
{
    return BITSET ( x , n );
}

/**
 * @brief Clears a bit within a bit vector.
 * @param x A bit vector.
 * @param n The bit to clear.
 * @return x with bit n cleared.
 */
INLINE
u64
bitclr
(   const u64   x
,   const u8    n
)
{
    return BITCLR ( x , n );
}

/**
 * @brief Toggles a bit within a bit vector.
 * @param x A bit vector.
 * @param n The bit to toggle.
 * @return x with bit n toggled.
 */
INLINE
u64
bitswp
(   const u64   x
,   const u8    n
)
{
    return BITSWP ( x , n );
}

/**
 * @brief Pops a bit within a bit vector.
 * @param x A bit vector.
 * @param n The bit to pop.
 * @return x with bit n toggled, or 0.
 */
INLINE
u64
bitpop
(   const u64   x
,   const u8    n
)
{
    return BIT ( x , n ) ? BITSWP ( x , n )
                         : 0
                         ;
}

#endif  // BITOPS_H
