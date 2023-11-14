/**
 * @file trig.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines trigonometric functions.
 */
#ifndef MATH_TRIGONOMETRY_H
#define MATH_TRIGONOMETRY_H

#include "common.h"

// Global constants: pi.
#define PI              3.14159265358979323846f
#define TWO_PI          ( 2.0f * PI )
#define FOUR_PI         ( 4.0f * PI )
#define HALF_PI         ( 0.5f * PI )
#define QUARTER_PI      ( 0.25f * PI )
#define DIV_PI          ( 1.0f / PI )
#define DIV_TWO_PI      ( 1.0f / 2PI )

/* Defines an alias for each trigonometric function (this avoids function
 * signatures which clash with the platform-specific math header). */
#define sin(X)  ( _sin ( X ) )
#define cos(X)  ( _cos ( X ) )
#define tan(X)  ( _tan ( X ) )
#define asin(X) ( _asin ( X ) )
#define acos(X) ( _acos ( X ) )
#define atan(X) ( _atan ( X ) )

/**
 * @brief Sine function.
 * @param x A real number.
 * @return sin(x)
 */
f32
_sin
(   f32 x
);

/**
 * @brief Cosine function.
 * @param x A real number.
 * @return cos(x)
 */
f32
_cos
(   f32 x
);

/**
 * @brief Tangent function.
 * @param x A real number.
 * @return tan(x)
 */
f32
_tan
(   f32 x
);

/**
 * @brief Inverse sine function.
 * @param x A real number.
 * @return arcsin(x)
 */
f32
_asin
(   f32 x
);

/**
 * @brief Inverse cosine function.
 * @param x A real number.
 * @return arccos(x)
 */
f32
_acos
(   f32 x
);

/**
 * @brief Inverse tangent function.
 * @param x A real number.
 * @return arctan(x)
 */
f32
_atan
(   f32 x
);

#endif  // MATH_TRIGONOMETRY_H
