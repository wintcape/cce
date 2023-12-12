/**
 * @file math.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Datatypes and operations for performing geometric operations.
 */
#ifndef MATH_H
#define MATH_H

#include "common.h"

#include "math/conversion.h"
#include "math/div.h"
#include "math/float.h"
#include "math/matrix.h"
#include "math/quaternion.h"
#include "math/random.h"
#include "math/root.h"
#include "math/trig.h"
#include "math/vector.h"

/**
 * @brief Calculates the minimum of two integer values.
 */
#define min(A,B)                \
   ({ __typeof__ (A) _A = (A);  \
      __typeof__ (B) _B = (B);  \
      _A < _B ? _A : _B;        \
    })

/**
 * @brief Calculates the maximum of two integer values.
 */
#define max(A,B)                \
   ({ __typeof__ (A) _A = (A);  \
      __typeof__ (B) _B = (B);  \
      _A > _B ? _A : _B;        \
    })

/**
 * @brief Clamps an integer value between a min and max (inclusive).
 * @param value The value to be clamped.
 * @param min The minimum value of the range.
 * @param max The maximum value of the range.
 * @return The clamped value.
 */
#define clamp(VALUE,MIN,MAX)                                    \
    ({ __typeof__ (VALUE) value = (VALUE);                      \
       __typeof__ (MIN) min = (MIN);                            \
       __typeof__ (MAX) max = (MAX);                            \
       ( value <= min ) ? min : ( value >= max ) ? max : value; \
    })

#endif  // MATH_H
