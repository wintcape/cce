/**
 * @file vertex.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines vertex datatypes and operations.
 */
#ifndef MATH_VERTEX_H
#define MATH_VERTEX_H

#include "common.h"

// Type definition for a 3-vertex.
typedef struct
{
    vec3_t pos;
    vec2_t texture_coordinate;
}
vertex3_t;

#endif  // MATH_VERTEX_H
