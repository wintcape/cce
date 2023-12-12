/**
 * @file quaternion.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines a quaternion datatype and operators.
 */
#ifndef MATH_QUATERNION_H
#define MATH_QUATERNION_H

#include "common.h"

#include "math/vector.h"

// Type definition.
typedef vec4_t quaternion_t;

/**
 * @brief Forms the identity quaternion.
 * @return The identity quaternion.
 */
INLINE
quaternion_t
quaternion_id
( void )
{
    return ( quaternion_t ){{ 0.0f , 0.0f , 0.0f , 1.0f }};
}

/**
 * @brief Computes the scalar product of two quaternions.
 * @param a A quaternion.
 * @param b A quaternion.
 * @return The scalar product of a and b.
 */
INLINE
f32
quaternion_dot
(   quaternion_t a
,   quaternion_t b
)
{
    return a.i * b.i + a.j * b.j + a.k * b.k + a.l * b.l;
}

/**
 * @brief Forms a normalized quaternion from the provided quaternion.
 * @param q A quaternion.
 * @return q / sqrt(dot(q,q))
 */
INLINE
quaternion_t
quaternion_norm
(   quaternion_t q
)
{
    const f32 mag = sqrt ( quaternion_dot ( q , q ) );
    return ( quaternion_t ){{ q.i / mag
                            , q.j / mag
                            , q.k / mag
                            , q.l / mag
                           }};
}

/**
 * @brief Forms a quaternion from the provided axis and angle.
 * @param axis      The axis of rotation.
 * @param r         The angle of rotation.
 * @param normalize Normalize output? Y/N
 */
INLINE
quaternion_t
quaternion_from_angle
(   vec3_t  axis
,   f32     r
,   bool    normalize
)
{
    const f32 r_ = 0.5f * r;
    const f32 sinr = sin ( r_ );
    const f32 cosr = cos ( r_ );
    const quaternion_t q = ( quaternion_t ){{ sinr * axis.x
                                            , sinr * axis.y
                                            , sinr * axis.z
                                            , cosr
                                            }};
    return ( normalize ) ? quaternion_norm ( q )
                         : q
                         ;
}

/**
 * @brief Forms the conjugate of the provided quaternion.
 * @param q A quaternion.
 * @return The conjugate of q.
 */
INLINE
quaternion_t
quaternion_conjugate
(   quaternion_t q
)
{
    return ( quaternion_t ){{ -q.i , -q.j , -q.k , q.l }};
}

/**
 * @brief Forms the inverse of the provided quaternion.
 * @param q A quaternion.
 * @return The inverse of q.
 */
INLINE
quaternion_t
quaternion_inverse
(   quaternion_t q
)
{
    return quaternion_norm ( quaternion_conjugate ( q ) );
}

/**
 * @brief Computes the quaternion product of two quaternions.
 * @param a A quaternion.
 * @param b A quaternion.
 * @return The quaternion product of a and b.
 */
INLINE
quaternion_t
quaternion_product
(   quaternion_t a
,   quaternion_t b
)
{
    return ( quaternion_t ){{  a.i * b.l + a.j * b.k - a.k * b.j + a.l * b.i
                            , -a.i * b.k + a.j * b.l + a.k * b.i + a.l * b.j
                            ,  a.i * b.j - a.j * b.i + a.k * b.l + a.l * b.k
                            , -a.i * b.i - a.j * b.j - a.k * b.k + a.l * b.l
                            }};
}

/**
 * @brief Computes the spherical linear interpolation between two quaternions.
 * @param a A quaternion.
 * @param b A quaternion.
 * @param s A percentage amount.
 * @return The quaternion that is s% of the transformation from a to b.
 */
INLINE
quaternion_t
quaternion_slerp
(   quaternion_t    a
,   quaternion_t    b
,   f32             s
)
{
    const f32 threshold = 0.9995f;

    quaternion_t a_ = quaternion_norm ( a );
    quaternion_t b_ = quaternion_norm ( b );

    f32 dot = quaternion_dot ( a_ , b_ );
    if ( dot < 0.0f )
    {
        b_.i = -b_.i;
        b_.j = -b_.j;
        b_.k = -b_.k;
        b_.l = -b_.l;
        dot = -dot;
    }
    if ( dot > threshold )
    {
        return quaternion_norm ( ( quaternion_t ){{ a_.i + s * ( b_.i - a_.i )
                                                  , a_.j + s * ( b_.j - a_.j )
                                                  , a_.k + s * ( b_.k - a_.k )
                                                  , a_.l + s * ( b_.l - a_.l )
                                                 }});
    }

    const f32 r0 = acos ( dot );
    const f32 r1 = s * r0;
    const f32 sinr0 = sin ( r0 );
    const f32 sinr1 = sin ( r1 );
    const f32 sa = cos ( r1 ) - dot * sinr1 / sinr0;
    const f32 sb = sinr1 / sinr0;
    return ( quaternion_t ){{ sa * a_.i + sb * b_.i
                            , sa * a_.j + sb * b_.j
                            , sa * a_.k + sb * b_.k
                            , sa * a_.l + sb * b_.l
                           }};
}

#endif  // MATH_QUATERNION_H
