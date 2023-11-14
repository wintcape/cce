/**
 * @file float.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines miscellaneous floating point constants and functions.
 */
#ifndef MATH_FLOAT_H
#define MATH_FLOAT_H

#include "common.h"

// Global constants.
#define INFINITY        ( 1e30f * 1e30f )
#define FLOAT_EPSILON   1.192092896e-07f

/* Defines an alias for the absolute value function (this avoids function
 * signatures which clash with the platform-specific math header). */
#define abs(X) ( _abs ( X ) )

/**
 * @brief Absolute value function.
 * @param x A real number.
 * @return abs(x)
 */
f32
_abs
(   f32 x
);

#endif  // MATH_FLOAT_H
