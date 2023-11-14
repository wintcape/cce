/**
 * @file conversion.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Measurement conversion rates.
 */
#ifndef MATH_CONVERSION_H
#define MATH_CONVERSION_H

#include "common.h"

#include "math/trig.h"

// Time conversion.
#define S_TO_US         ( 1000.0f * 1000.0f )
#define S_TO_MS         1000.0f
#define MS_TO_S         0.001f

// Angle conversion.
#define DEG_TO_RAD      ( PI / 180.0f )
#define RAD_TO_DEG      ( 180.0f / PI )

/**
 * @brief Degree -> radian conversion function.
 * @param deg An angle in degrees.
 * @param deg converted to radians.
 */
INLINE
f32
deg_to_rad
(   f32 deg
)
{
    return DEG_TO_RAD * deg;
}

/**
 * @brief Radian -> degree conversion function.
 * @param rad An angle in radians.
 * @param rad converted to degrees.
 */
INLINE
f32
rad_to_deg
(   f32 rad
)
{
    return RAD_TO_DEG * rad;
}

#endif  // MATH_CONVERSION_H
