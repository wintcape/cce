/**
 * @file root.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines n-th root operations.
 */
#ifndef MATH_ROOT_H
#define MATH_ROOT_H

#include "common.h"

// Global constants.
#define SQRT2           1.41421356237309504880f
#define SQRT3           1.73205080756887729352f
#define DIV_SQRT2       0.70710678118654752440f
#define DIV_SQRT3       0.57735026918962576450f

/**
 * @brief Defines an alias for the square root function (this avoids function
 * signatures which clash with the platform-specific math header).
 */
#define sqrt(X) ( _sqrt ( X ) )

/**
 * @brief Square root function.
 * @param x A real number.
 * @return sqrt(x)
 */
f32
_sqrt
(   f32 x
);

#endif  // MATH_ROOT_H
