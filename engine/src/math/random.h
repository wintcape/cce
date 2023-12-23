/**
 * @file random.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Random number generator functions.
 */
#ifndef MATH_RANDOM_H
#define MATH_RANDOM_H

#include "common.h"

/**
 * @brief Defines an alias for the random function (this avoids function
 * signatures which clash with the platform-specific math header).
 */
#define random()  ( _random () )

/**
 * @brief Generates a random integer.
 * @return A random integer.
 */
i32
_random
( void );

/**
 * @brief Generates a random integer in the specified range.
 * @param l lower bound (inclusive)
 * @param u upper bound (inclusive)
 * @return A random integer in the range [ l , u ].
 */
i32
random2
(   i32 l
,   i32 h
);

/**
 * @brief Generates a random 64-bit integer.
 * @param l lower bound (inclusive)
 * @param u upper bound (inclusive)
 * @return A random 64-bit integer.
 */
i64
random64
( void );

/**
 * @brief Generates a random floating point number.
 * @return A random floating point number.
 */
f32
randomf
( void );

/**
 * @brief Generates a random floating point number in the specified range.
 * @param l lower bound (inclusive)
 * @param u upper bound (inclusive)
 * @return A random floating point number in the range [ l , u ].
 */
f32
randomf2
(   f32 l
,   f32 h
);

#endif  // MATH_RANDOM_H
