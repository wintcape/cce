/**
 * @file div.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Integer division predicates.
 */
#ifndef MATH_DIV_H
#define MATH_DIV_H

#include "common.h"

/**
 * @brief Calculates n div 2.
 * @param n A natural number to test.
 * @return true if n div 2, false otherwise.
 */
INLINE
bool
div2
(   u64 n
)
{
    return n && !( n & ( n - 1 ) );
}

#endif  // MATH_DIV_H
