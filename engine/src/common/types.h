/**
 * @file types.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Common type definitions.
 */
#ifndef TYPES_H
#define TYPES_H

/**
 * @brief Type definitions for numeric types. These are hardware-specific.
 */

// Integer (unsigned).
typedef unsigned char           u8;
typedef unsigned short int      u16;
typedef unsigned int            u32;
typedef unsigned long long int  u64;

// Integer (signed).
typedef signed char             i8;
typedef signed short int        i16;
typedef signed int              i32;
typedef signed long long int    i64;

// Floating point.
typedef float   f32;
typedef double  f64;

// Boolean.
typedef int b32;

/**
 * @brief Type and instance definitions for generic boolean type.
 */

// Type definition.
typedef _Bool bool;

// Instance definitions.
#define true    ( ( bool ) 1 )
#define false   ( ( bool ) 0 )

/**
 * @brief Compile-time assertions.
 */

#include "static_assert.h"

// Verify all hardware-specific types have the expected size.
STATIC_ASSERT ( sizeof( u8 )  == 1 , "Expected u8 to be 1 byte." );
STATIC_ASSERT ( sizeof( u16 ) == 2 , "Expected u16 to be 2 bytes." );
STATIC_ASSERT ( sizeof( u32 ) == 4 , "Expected u32 to be 4 bytes." );
STATIC_ASSERT ( sizeof( u64 ) == 8 , "Expected u64 to be 8 bytes." );
STATIC_ASSERT ( sizeof( i8 )  == 1 , "Expected i8 to be 1 byte." );
STATIC_ASSERT ( sizeof( i16 ) == 2 , "Expected i16 to be 2 bytes." );
STATIC_ASSERT ( sizeof( i32 ) == 4 , "Expected i32 to be 4 bytes." );
STATIC_ASSERT ( sizeof( i64 ) == 8 , "Expected i64 to be 8 bytes." );
STATIC_ASSERT ( sizeof( f32 ) == 4 , "Expected f32 to be 4 bytes." );
STATIC_ASSERT ( sizeof( f64 ) == 8 , "Expected f64 to be 8 bytes." );
STATIC_ASSERT ( sizeof( b32 ) == 4 , "Expected b32 to be 4 bytes." );

#endif  // TYPES_H
