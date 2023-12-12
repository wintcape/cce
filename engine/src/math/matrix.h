/**
 * @file matrix.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines matrix datatypes and operators.
 */
#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include "common.h"

#include "core/memory.h"
#include "core/logger.h"

#include "math/quaternion.h"
#include "math/trig.h"
#include "math/vector.h"

// Type definition for a 4x4 matrix.
typedef union
{
    f32 data[ 16 ];
}
mat4_t;

/**
 * @brief Forms the 4x4 identity matrix.
 * @return  | 1 0 0 0 |
 *          | 0 1 0 0 |
 *          | 0 0 1 0 |
 *          | 0 0 0 1 |
 */
INLINE
mat4_t
mat4_id
( void )
{
    return ( mat4_t ){{ 1.0f , 0.0f , 0.0f , 0.0f
                      , 0.0f , 1.0f , 0.0f , 0.0f
                      , 0.0f , 0.0f , 1.0f , 0.0f
                      , 0.0f , 0.0f , 0.0f , 1.0f
                      }};
}

/**
 * @brief Forms a 4x4 matrix from the provided quaternion.
 * @param q A quaternion.
 * @return The 4x4 matrix formed by q.
 */
INLINE
mat4_t
mat4_from_quaternion
(   quaternion_t q
)
{
    const quaternion_t q_ = quaternion_norm ( q );
    return ( mat4_t ){{ 1.0f - 2.0f * q_.j * q_.j - 2.0f * q_.k * q_.k
                      , 2.0f * q_.i * q_.j - 2.0f * q_.k * q_.l
                      , 2.0f * q_.i * q_.k + 2.0f * q_.j * q_.l
                      , 2.0f * q_.i * q_.j + 2.0f * q_.k * q_.l
                      , 1.0f - 2.0f * q_.i * q_.i - 2.0f * q_.k * q_.k
                      , 2.0f * q_.j * q_.k - 2.0f * q_.i * q_.l
                      , 2.0f * q_.i * q_.k - 2.0f * q_.j * q_.l
                      , 2.0f * q_.j * q_.k + 2.0f * q_.i * q_.l
                      , 1.0f - 2.0f * q_.i * q_.i - 2.0f * q_.j * q_.j
                      }};
}

/**
 * @brief Forms the inverse of a provided 4x4 matrix.
 * @param m A 4x4 matrix.
 * @return The inverse of m.
 */
INLINE
mat4_t
mat4_inverse
(   mat4_t m
)
{
    mat4_t n;

    const f32 t0 = m.data[ 10 ] * m.data[ 15 ];
    const f32 t1 = m.data[ 14 ] * m.data[ 11 ];
    const f32 t2 = m.data[ 6 ] * m.data[ 15 ];
    const f32 t3 = m.data[ 14 ] * m.data[ 7 ];
    const f32 t4 = m.data[ 6 ] * m.data[ 11 ];
    const f32 t5 = m.data[ 10 ] * m.data[ 7 ];
    const f32 t6 = m.data[ 2 ] * m.data[ 15 ];
    const f32 t7 = m.data[ 14 ] * m.data[ 3 ];
    const f32 t8 = m.data[ 2 ] * m.data[ 11 ];
    const f32 t9 = m.data[ 10 ] * m.data[ 3 ];
    const f32 t10 = m.data[ 2 ] * m.data[ 7 ];
    const f32 t11 = m.data[ 6 ] * m.data[ 3 ];
    const f32 t12 = m.data[ 8 ] * m.data[ 13 ];
    const f32 t13 = m.data[ 12 ] * m.data[ 9 ];
    const f32 t14 = m.data[ 4 ] * m.data[ 13 ];
    const f32 t15 = m.data[ 12 ] * m.data[ 5 ];
    const f32 t16 = m.data[ 4 ] * m.data[ 9 ];
    const f32 t17 = m.data[ 8 ] * m.data[ 5 ];
    const f32 t18 = m.data[ 0 ] * m.data[ 13 ];
    const f32 t19 = m.data[ 12 ] * m.data[ 1 ];
    const f32 t20 = m.data[ 0 ] * m.data[ 9 ];
    const f32 t21 = m.data[ 8 ] * m.data[ 1 ];
    const f32 t22 = m.data[ 0 ] * m.data[ 5 ];
    const f32 t23 = m.data[ 4 ] * m.data[ 1 ];

    n.data[ 0 ] = ( t0 * m.data[ 5 ] + t3 * m.data[ 9 ] + t4 * m.data[ 13 ] )
                - ( t1 * m.data[ 5 ] + t2 * m.data[ 9 ] + t5 * m.data[ 13 ] );
    n.data[ 1 ] = ( t1 * m.data[ 1 ] + t6 * m.data[ 9 ] + t9 * m.data[ 13 ] )
                - ( t0 * m.data[ 1 ] + t7 * m.data[ 9 ] + t8 * m.data[ 13 ] );
    n.data[ 2 ] = ( t2 * m.data[ 1 ] + t7 * m.data[ 5 ] + t10 * m.data[ 13 ] )
                - ( t3 * m.data[ 1 ] + t6 * m.data[ 5 ] + t11 * m.data[ 13 ] );
    n.data[ 3 ] = ( t5 * m.data[ 1 ] + t8 * m.data[ 5 ] + t11 * m.data[ 9 ] ) 
                - ( t4 * m.data[ 1 ] + t9 * m.data[ 5 ] + t10 * m.data[ 9 ] );

    const f32 d = 1.0f / ( m.data[ 0 ] * n.data[ 0 ] + m.data[ 4 ] * n.data[ 1 ]
                         + m.data[ 8 ] * n.data[ 2 ] + m.data[ 12 ] * n.data[ 3 ]
                         );

    n.data[ 0 ] = d * n.data[ 0 ];
    n.data[ 1 ] = d * n.data[ 1 ];
    n.data[ 2 ] = d * n.data[ 2 ];
    n.data[ 3 ] = d * n.data[ 3 ];
    n.data[ 4 ] = d * ( ( t1 * m.data[ 4 ] + t2 * m.data[ 8 ] + t5 * m.data[ 12 ] ) 
                - ( t0 * m.data[ 4 ] + t3 * m.data[ 8 ] + t4 * m.data[ 12 ] ) );
    n.data[ 5 ] = d * ( ( t0 * m.data[ 0 ] + t7 * m.data[ 8 ] + t8 * m.data[ 12 ] ) 
                - ( t1 * m.data[ 0 ] + t6 * m.data[ 8 ] + t9 * m.data[ 12 ] ) );
    n.data[ 6 ] = d * ( ( t3 * m.data[ 0 ] + t6 * m.data[ 4 ] + t11 * m.data[ 12 ] ) 
                - ( t2 * m.data[ 0 ] + t7 * m.data[ 4 ] + t10 * m.data[ 12 ] ) );
    n.data[ 7 ] = d * ( ( t4 * m.data[ 0 ] + t9 * m.data[ 4 ] + t10 * m.data[ 8 ] ) 
                - ( t5 * m.data[ 0 ] + t8 * m.data[ 4 ] + t11 * m.data[ 8 ] ) );
    n.data[ 8 ] = d * ( ( t12 * m.data[ 7 ] + t15 * m.data[ 11 ] + t16 * m.data[ 15 ] )
                - ( t13 * m.data[ 7 ] + t14 * m.data[ 11 ] + t17 * m.data[ 15 ] ) );
    n.data[ 9 ] = d * ( ( t13 * m.data[ 3 ] + t18 * m.data[ 11 ] + t21 * m.data[ 15 ] ) 
                - ( t12 * m.data[ 3 ] + t19 * m.data[ 11 ] + t20 * m.data[ 15 ] ) );
    n.data[ 10 ] = d * ( ( t14 * m.data[ 3 ] + t19 * m.data[ 7 ] + t22 * m.data[ 15 ] ) 
                 - ( t15 * m.data[ 3 ] + t18 * m.data[ 7 ] + t23 * m.data[ 15 ] ) );
    n.data[ 11 ] = d * ( ( t17 * m.data[ 3 ] + t20 * m.data[ 7 ] + t23 * m.data[ 11 ] ) 
                 - ( t16 * m.data[ 3 ] + t21 * m.data[ 7 ] + t22 * m.data[ 11 ] ) );
    n.data[ 12 ] = d * ( ( t14 * m.data[ 10 ] + t17 * m.data[ 14 ] + t13 * m.data[ 6 ] ) 
                 - ( t16 * m.data[ 14 ] + t12 * m.data[ 6 ] + t15 * m.data[ 10 ] ) );
    n.data[ 13 ] = d * ( ( t20 * m.data[ 14 ] + t12 * m.data[ 2 ] + t19 * m.data[ 10 ] ) 
                 - ( t18 * m.data[ 10 ] + t21 * m.data[ 14 ] + t13 * m.data[ 2 ] ) );
    n.data[ 14 ] = d * ( ( t18 * m.data[ 6 ] + t23 * m.data[ 14 ] + t15 * m.data[ 2 ] ) 
                 - ( t22 * m.data[ 14 ] + t14 * m.data[ 2 ] + t19 * m.data[ 6 ] ) );
    n.data[ 15 ] = d * ( ( t22 * m.data[ 10 ] + t16 * m.data[ 2 ] + t21 * m.data[ 6 ] ) 
                 - ( t20 * m.data[ 6 ] + t23 * m.data[ 10 ] + t17 * m.data[ 2 ] ) );

    return n;
}

/**
 * @brief Forms the transpose of a provided 4x4 matrix.
 * @param m A 4x4 matrix.
 * @return The transpose of m.
 */
INLINE
mat4_t
mat4_transpose
(   mat4_t m
)
{
    return ( mat4_t ){{ m.data[ 0 ] , m.data[ 4 ] , m.data[ 8 ]  , m.data[ 12 ]
                      , m.data[ 1 ] , m.data[ 5 ] , m.data[ 9 ]  , m.data[ 13 ]
                      , m.data[ 2 ] , m.data[ 6 ] , m.data[ 10 ] , m.data[ 14 ]
                      , m.data[ 3 ] , m.data[ 7 ] , m.data[ 11 ] , m.data[ 15 ]
                      }};
}

/**
 * @brief Calculates the product of two 4x4 matrices.
 * @param a A 4x4 matrix.
 * @param b A 4x4 matrix.
 * @return The matrix product produced by applying a to b.
 */
INLINE
mat4_t
mat4_product
(   mat4_t a
,   mat4_t b
)
{
    return ( mat4_t ){{   a.data[ 0 ] * b.data[ 0 ] + a.data[ 1 ] * b.data[ 4 ]
                        + a.data[ 2 ] * b.data[ 8 ] + a.data[ 3 ] * b.data[ 12 ]
                      ,   a.data[ 0 ] * b.data[ 1 ] + a.data[ 1 ] * b.data[ 5 ]
                        + a.data[ 2 ] * b.data[ 9 ] + a.data[ 3 ] * b.data[ 13 ]
                      ,   a.data[ 0 ] * b.data[ 2 ] + a.data[ 1 ] * b.data[ 6 ]
                        + a.data[ 2 ] * b.data[ 10 ] + a.data[ 3 ] * b.data[ 14 ]
                      ,   a.data[ 0 ] * b.data[ 3 ] + a.data[ 1 ] * b.data[ 7 ]
                        + a.data[ 2 ] * b.data[ 11 ] + a.data[ 3 ] * b.data[ 15 ]
                      ,   a.data[ 4 ] * b.data[ 0 ] + a.data[ 5 ] * b.data[ 4 ]
                        + a.data[ 6 ] * b.data[ 8 ] + a.data[ 7 ] * b.data[ 12 ]
                      ,   a.data[ 4 ] * b.data[ 1 ] + a.data[ 5 ] * b.data[ 5 ]
                        + a.data[ 6 ] * b.data[ 9 ] + a.data[ 7 ] * b.data[ 13 ]
                      ,   a.data[ 4 ] * b.data[ 2 ] + a.data[ 5 ] * b.data[ 6 ]
                        + a.data[ 6 ] * b.data[ 10 ] + a.data[ 7 ] * b.data[ 14 ]
                      ,   a.data[ 4 ] * b.data[ 3 ] + a.data[ 5 ] * b.data[ 7 ]
                        + a.data[ 6 ] * b.data[ 11 ] + a.data[ 7 ] * b.data[ 15 ]
                      ,   a.data[ 8 ] * b.data[ 0 ] + a.data[ 9 ] * b.data[ 4 ]
                        + a.data[ 10 ] * b.data[ 8 ] + a.data[ 11 ] * b.data[ 12 ]
                      ,   a.data[ 8 ] * b.data[ 1 ] + a.data[ 9 ] * b.data[ 5 ]
                        + a.data[ 10 ] * b.data[ 9 ] + a.data[ 11 ] * b.data[ 13 ]
                      ,   a.data[ 8 ] * b.data[ 2 ] + a.data[ 9 ] * b.data[ 6 ]
                        + a.data[ 10 ] * b.data[ 10 ] + a.data[ 11 ] * b.data[ 14 ]
                      ,   a.data[ 8 ] * b.data[ 3 ] + a.data[ 9 ] * b.data[ 7 ]
                        + a.data[ 10 ] * b.data[ 11 ] + a.data[ 11 ] * b.data[ 15 ]
                      ,   a.data[ 12 ] * b.data[ 0 ] + a.data[ 13 ] * b.data[ 4 ]
                        + a.data[ 14 ] * b.data[ 8 ] + a.data[ 15 ] * b.data[ 12 ]
                      ,   a.data[ 12 ] * b.data[ 1 ] + a.data[ 13 ] * b.data[ 5 ]
                        + a.data[ 14 ] * b.data[ 9 ] + a.data[ 15 ] * b.data[ 13 ]
                      ,   a.data[ 12 ] * b.data[ 2 ] + a.data[ 13 ] * b.data[ 6 ]
                        + a.data[ 14 ] * b.data[ 10 ] + a.data[ 15 ] * b.data[ 14 ]
                      ,   a.data[ 12 ] * b.data[ 3 ] + a.data[ 13 ] * b.data[ 7 ]
                        + a.data[ 14 ] * b.data[ 11 ] + a.data[ 15 ] * b.data[ 15 ]
                      }};
}

/**
 * @brief Forms a matrix translated to the specified position.
 * @param p The position to translate to.
 * @return A translation matrix that fits the parameter.
 */
INLINE
mat4_t
mat4_translation
(   vec3_t p
)
{
    return ( mat4_t ){{ 1.0f , 0.0f , 0.0f , 0.0f
                      , 0.0f , 1.0f , 0.0f , 0.0f
                      , 0.0f , 0.0f , 1.0f , 0.0f
                      , p.x  , p.y  , p.z  , 1.0f
                      }};
}

/**
 * @brief Forms a matrix scaled by the specified amount.
 * @param s The scale amount.
 * @return A scale matrix that fits the parameter.
 */
INLINE
mat4_t
mat4_scale
(   vec3_t s
)
{
    return ( mat4_t ){{ s.x  , 0.0f , 0.0f , 0.0f
                      , 0.0f , s.y  , 0.0f , 0.0f
                      , 0.0f , 0.0f , s.z  , 0.0f
                      , 0.0f , 0.0f , 0.0f , 1.0f
                      }};
}

/**
 * @brief Forms a matrix rotated along the x-axis by the specified amount.
 * @param r The rotation angle (in radians)
 * @return A rotation matrix that fits the parameter.
 */
INLINE
mat4_t
mat4_euler_x
(   f32 r
)
{
    const f32 cosr = cos ( r );
    const f32 sinr = sin ( r );
    return ( mat4_t ){{ 1.0f ,  0.0f , 0.0f , 0.0f
                      , 0.0f ,  cosr , sinr , 0.0f
                      , 0.0f , -sinr , cosr , 0.0f
                      , 0.0f ,  0.0f , 0.0f , 1.0f
                      }};
}

/**
 * @brief Forms a matrix rotated along the y-axis by the specified amount.
 * @param r The rotation angle (in radians)
 * @return A rotation matrix that fits the parameter.
 */
INLINE
mat4_t
mat4_euler_y
(   f32 r
)
{
    const f32 cosr = cos ( r );
    const f32 sinr = sin ( r );
    return ( mat4_t ){{ cosr , 0.0f , -sinr , 0.0f
                      , 0.0f , 1.0f ,  0.0f , 0.0f
                      , sinr , 0.0f ,  cosr , 0.0f
                      , 0.0f , 0.0f ,  0.0f , 1.0f
                      }};
}

/**
 * @brief Forms a matrix rotated along the z-axis by the specified amount.
 * @param r The rotation angle (in radians)
 * @return A rotation matrix that fits the parameter.
 */
INLINE
mat4_t
mat4_euler_z
(   f32 r
)
{
    const f32 cosr = cos ( r );
    const f32 sinr = sin ( r );
    return ( mat4_t ){{  cosr , sinr , 0.0f , 0.0f
                      , -sinr , cosr , 0.0f , 0.0f
                      ,  0.0f , 0.0f , 1.0f , 0.0f
                      ,  0.0f , 0.0f , 0.0f , 1.0f
                      }};
}

/**
 * @brief Forms a matrix rotated by the specified amount.
 * @param r The rotation angles for each axis (in radians).
 * @return A rotation matrix that fits the parameter.
 */
INLINE
mat4_t
mat4_euler
(   vec3_t r
)
{
    return mat4_product ( mat4_product ( mat4_euler_x ( r.x )
                                       , mat4_euler_y ( r.y )
                                       )
                        , mat4_euler_z ( r.z )
                        );
}

/**
 * @brief Forms a rotation matrix from the specified quaternion and center
 *        point.
 * @param q A quaternion.
 * @param c The center point.
 * @return A rotation matrix that fits the parameters.
 */
INLINE
mat4_t
mat4_rotation_from_quaternion
(   quaternion_t    q
,   vec3_t          c
)
{
    mat4_t m;
    m.data[ 0 ] = ( q.i * q.i ) - ( q.j * q.j ) - ( q.k * q.k ) + ( q.l * q.l );
    m.data[ 1 ] = 2.0f * ( q.i * q.j + q.k * q.l );
    m.data[ 2 ] = 2.0f * ( q.i * q.k - q.j * q.l );
    m.data[ 3 ] = c.x - c.x * m.data[ 0 ] - c.y * m.data[ 1 ] - c.z * m.data[ 2 ];
    m.data[ 4 ] = 2.0f * ( q.i * q.j - q.k * q.l );
    m.data[ 5 ] = -q.i * q.i + q.j * q.j - q.k * q.k + q.l * q.l;
    m.data[ 6 ] = 2.0f * ( q.j * q.k + q.i * q.l );
    m.data[ 7 ] = c.y - c.x * m.data[ 4 ] - c.y * m.data[ 5 ] - c.z * m.data[ 6 ];
    m.data[ 8 ] = 2.0f * ( q.i * q.k + q.j * q.l );
    m.data[ 9 ] = 2.0f * ( q.j * q.k - q.i * q.l );
    m.data[ 10 ] = -q.i * q.i - q.j * q.j + q.k * q.k + q.l * q.l;
    m.data[ 11 ] = c.z - c.x * m.data[ 8 ] - c.y * m.data[ 9 ] - c.z * m.data[ 10 ];
    m.data[ 12 ] = 0.0f;
    m.data[ 13 ] = 0.0f;
    m.data[ 14 ] = 0.0f;
    m.data[ 15 ] = 1.0f;
    return m;
}

/**
 * @brief Forms an orthographic projection matrix.
 * @param l The left boundary of the view frustum.
 * @param r The right boundary of the view frustum.
 * @param b The bottom boundary of the view frustum.
 * @param t The top boundary of the view frustum.
 * @param n The distance to the near clipping plane.
 * @param f The distance to the far clipping plane.
 * @return An orthographic projection matrix that fits the parameters.
 */
INLINE
mat4_t
mat4_orthographic
(   f32 l , f32 r
,   f32 t , f32 b
,   f32 n , f32 f
)
{
    const f32 lr = 1.0f / ( l - r );
    const f32 bt = 1.0f / ( b - t );
    const f32 nf = 1.0f / ( n - f );
    return ( mat4_t ){{      2.0f * lr ,           0.0f ,           0.0f , 0.0f
                      ,           0.0f ,     -2.0f * bt ,           0.0f , 0.0f
                      ,           0.0f ,           0.0f ,      2.0f * nf , 0.0f
                      , lr * ( l + r ) , bt * ( b + t ) , nf * ( n + f ) , 1.0f
                      }};
}

/**
 * @brief Forms a perspective projection matrix.
 * @param fov The field of view (in radians).
 * @param ar The aspect ratio.
 * @param n The distance to the near clipping plane.
 * @param f The distance to the far clipping plane.
 * @return A perspective projection matrix that fits the parameters.
 */
INLINE
mat4_t
mat4_perspective
(   f32 fov
,   f32 ar
,   f32 n , f32 f
)
{
    const f32 x = tan ( 0.5f * fov );
    return ( mat4_t ){{ 1.0f / ( x * ar ) ,     0.0f ,                              0.0f ,  0.0f 
                      ,              0.0f , 1.0f / x ,                              0.0f ,  0.0f
                      ,              0.0f ,     0.0f ,        -( ( f + n ) / ( f - n ) ) , -1.0f
                      ,              0.0f ,     0.0f , -( ( 2.0f * f * n ) / ( f - n ) ) ,  0.0f
                      }};
}

/**
 * @brief Forms a matrix which is "looking at" a target from the
 * perspective of the provided position.
 * @param p  The position.
 * @param t  The target to look at.
 * @param up The up vector.
 */
INLINE
mat4_t
mat4_look_at
(   vec3_t p
,   vec3_t t
,   vec3_t up
)
{
    const vec3_t z = vec3_norm ( ( vec3_t ){{ t.x - p.x
                                            , t.y - p.y
                                            , t.z - p.z
                                            }});
    const vec3_t x = vec3_norm ( vec3_cross ( z , up ) );
    const vec3_t y = vec3_cross ( x , z );
    return ( mat4_t ){{                 x.x ,                 y.x ,               -z.x , 0.0f
                      ,                 x.y ,                 y.y ,               -z.y , 0.0f
                      ,                 x.z ,                 y.z ,               -z.z , 0.0f
                      , -vec3_dot ( x , p ) , -vec3_dot ( y , p ) , vec3_dot ( z , p ) , 1.0f
                      }};
}

/**
 * @brief Forms the forward vector relative to a 4x4 matrix.
 * @param m A 4x4 matrix.
 * @return A normalized 3-component directional vector.
 */
INLINE
vec3_t
mat4_f
(   mat4_t m
)
{
    vec3_t f = ( vec3_t ){{ -m.data[ 2 ] , -m.data[ 6 ] , -m.data[ 10 ] }};
    vec3_normalize ( &f );
    return f;
}

/**
 * @brief Forms the back vector relative to a 4x4 matrix.
 * @param m A 4x4 matrix.
 * @return A normalized 3-component directional vector.
 */
INLINE
vec3_t
mat4_b
(   mat4_t m
)
{
    vec3_t b = ( vec3_t ){{ m.data[ 2 ] , m.data[ 6 ] , m.data[ 10 ] }};
    vec3_normalize ( &b );
    return b;
}

/**
 * @brief Forms the up vector relative to a 4x4 matrix.
 * @param m A 4x4 matrix.
 * @return A normalized 3-component directional vector.
 */
INLINE
vec3_t
mat4_u
(   mat4_t m
)
{
    vec3_t u = ( vec3_t ){{ -m.data[ 1 ] , -m.data[ 5 ] , -m.data[ 9 ] }};
    vec3_normalize ( &u );
    return u;
}

/**
 * @brief Forms the down vector relative to a 4x4 matrix.
 * @param m A 4x4 matrix.
 * @return A normalized 3-component directional vector.
 */
INLINE
vec3_t
mat4_d
(   mat4_t m
)
{
    vec3_t d = ( vec3_t ){{ m.data[ 1 ] , m.data[ 5 ] , m.data[ 9 ] }};
    vec3_normalize ( &d );
    return d;
}

/**
 * @brief Forms the left vector relative to a 4x4 matrix.
 * @param m A 4x4 matrix.
 * @return A normalized 3-component directional vector.
 */
INLINE
vec3_t
mat4_l
(   mat4_t m
)
{
    vec3_t l = ( vec3_t ){{ -m.data[ 0 ] , -m.data[ 4 ] , -m.data[ 8 ] }};
    vec3_normalize ( &l );
    return l;
}

/**
 * @brief Forms the right vector relative to a 4x4 matrix.
 * @param m A 4x4 matrix.
 * @return A normalized 3-component directional vector.
 */
INLINE
vec3_t
mat4_r
(   mat4_t m
)
{
    vec3_t r = ( vec3_t ){{ m.data[ 0 ] , m.data[ 4 ] , m.data[ 8 ] }};
    vec3_normalize ( &r );
    return r;
}

#endif  // MATH_MATRIX_H
