/**
 * @file random.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Random number generator functions.
 */
#ifndef MATH_RANDOM_H
#define MATH_RANDOM_H

#include "common.h"

/* Defines an alias for the random function (this avoids function
 * signatures which clash with the platform-specific math header). */
#define random()  ( _random () )

/**
 * Generates a random integer.
 * @return A random integer.
 */
i32
_random
( void );

/**
 * Generates a random integer in the specified range.
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
 * Generates a random floating point number.
 * @return A random floating point number.
 */
f32
randomf
( void );

/**
 * Generates a random floating point number in the specified range.
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
