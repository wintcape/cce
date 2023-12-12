/**
 * @file vector.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines vector datatypes and operators.
 */
#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include "common.h"

#include "math/float.h"
#include "math/root.h"

// Type definition for a vector with two components.
typedef union
{
    f32 c[ 2 ];
    struct
    {
        union { f32 i , x , r , s , u; };
        union { f32 j , y , g , t , v; };
    };
}
vec2_t;

// Type definition for a vector with three components.
typedef union
{
    f32 c[ 3 ];
    struct
    {
        union { f32 i , x , r , s , u; };
        union { f32 j , y , g , t , v; };
        union { f32 k , z , b , p , w; };
    };
}
vec3_t;

// Type definition for a vector with four components.
typedef union
{
    f32 c[ 4 ];
    struct
    {
        union { f32 i , x , r , s; };
        union { f32 j , y , g , t; };
        union { f32 k , z , b , p; };
        union { f32 l , w , a , q; };
    };
}
vec4_t;


/**
 * @brief Forms a vector from the two provided components.
 * @param i .
 * @param j .
 * @return < i , j >
 */
INLINE
vec2_t
vec2
(   f32 i
,   f32 j
)
{
    return ( vec2_t ){{ i , j }};
}

/**
 * @brief Forms the two-component zero vector.
 * @return  < 0 , 0 >
 */
INLINE
vec2_t
vec2_0
( void )
{
    return ( vec2_t ){{ 0.0f , 0.0f }};
}

/**
 * @brief Forms the two-component unit vector.
 * @return < 1 , 1 >
 */
INLINE
vec2_t
vec2_1
( void )
{
    return ( vec2_t ){{ 1.0f , 1.0f }};
}

/**
 * @brief Forms the two-component up vector.
 * @return < 0 , 1 >
 */
INLINE
vec2_t
vec2_u
( void )
{
    return ( vec2_t ){{ 0.0f , 1.0f }};
}

/**
 * @brief Forms the two-component down vector.
 * @return < 0 , -1 >
 */
INLINE
vec2_t
vec2_d
( void )
{
    return ( vec2_t ){{ 0.0f , -1.0f }};
}

/**
 * @brief Forms the two-component left vector.
 * @return < -1 , 0 >
 */
INLINE
vec2_t
vec2_l
( void )
{
    return ( vec2_t ){{ -1.0f , 0.0f }};
}

/**
 * @brief Forms the two-component right vector.
 * @return < 1 , 0 >
 */
INLINE
vec2_t
vec2_r
( void )
{
    return ( vec2_t ){{ 1.0f , 0.0f }};
}

/**
 * @brief Forms a two-component vector by scaling a provided
 * two-component vector by a constant factor.
 * @param v A two-component vector of the form < i , j >
 * @param s A scalar.
 * @return < s*i , s*j >
 */
INLINE
vec2_t
vec2_scale
(   vec2_t  v
,   f32     s
)
{
    return ( vec2_t ){{ s * v.i , s * v.j }};
}

/**
 * @brief Computes the sum of two two-component vectors.
 * @param a A two-component vector.
 * @param b A two-component vector.
 * @return a + b
 */
INLINE
vec2_t
vec2_sum
(   vec2_t a
,   vec2_t b
)
{
    return ( vec2_t ){{ a.i + b.i , a.j + b.j }};
}

/**
 * @brief Computes the difference of two two-component vectors.
 * @param a A two-component vector.
 * @param b A two-component vector.
 * @return a - b
 */
INLINE
vec2_t
vec2_dif
(   vec2_t a
,   vec2_t b
)
{
    return ( vec2_t ){{ a.i - b.i , a.j - b.j }};
}

/**
 * @brief Computes the scalar product of two two-component vectors.
 * @param a A two-component vector.
 * @param b A two-component vector.
 * @return The scalar product of a and b.
 */
INLINE
f32
vec2_dot
(   vec2_t a
,   vec2_t b
)
{
    return a.i * b.i + a.j * b.j;
}

/**
 * @brief Computes the magnitude of a two-component vector.
 * @param v A two-component vector of the form < i , j >
 * @return || v ||
 */
INLINE
f32
vec2_mag
(   vec2_t v
)
{
    return sqrt ( vec2_dot ( v , v ) );
}

/**
 * @brief Normalizes a provided two-component vector.
 * @param v The two-component vector to normalize.
 */
INLINE
void
vec2_normalize
(   vec2_t* v
)
{
    const f32 mag = vec2_mag ( *v );
    ( *v ).i /= mag;
    ( *v ).j /= mag;
}

/**
 * @brief Forms a normalized two-component vector from a provided 
 * two-component vector.
 * @param v The two-component vector to normalize.
 * @return v * ( 1 / || v || )
 */
INLINE
vec2_t
vec2_norm
(   vec2_t v
)
{
    const f32 mag = vec2_mag ( v );
    return ( vec2_t ){{ v.i / mag , v.j / mag }};
}

/**
 * @brief Compares each component of a two-component vector to the
 * corresponding component of the other two-component vector. Ensures no 
 * component difference is greater than the provided tolerance.
 * @param a A two-component vector.
 * @param b A two-component vector.
 * @param t The difference tolerance.
 * @return true if all components within tolerance; false otherwise.
 */
INLINE
bool
vec2_cmp
(   vec2_t  a
,   vec2_t  b
,   f32     t
)
{
    return !(  abs ( a.i - b.i ) > t
            || abs ( a.j - b.j ) > t
            );
}

/**
 * @brief Calculates the displacement between two two-component vectors.
 * @param a A two-component vector.
 * @param b A two-component vector.
 * @return The displacement from a to b.
 */
INLINE
f32
vec2_disp
(   vec2_t a
,   vec2_t b
)
{
    return vec2_mag ( vec2_dif ( a , b ) );
}

/**
 * @brief Forms a vector from the three provided components.
 * @param i .
 * @param j .
 * @param k .
 * @return < i , j , k >
 */
INLINE
vec3_t
vec3
(   f32 i
,   f32 j
,   f32 k
)
{
    return ( vec3_t ){{ i , j , k }};
}

/**
 * @brief Forms a three-component vector from a four-component vector by
 * dropping the final component.
 * @param v A four-component vector of the form < i , j , k , l >
 * @return < i , j , k >
 */
INLINE
vec3_t
vec3_from_vec4
(   vec4_t v
)
{
    return ( vec3_t ){{ v.i , v.j , v.k }};
}

/**
 * @brief Forms the three-component zero vector.
 * @return < 0 , 0 , 0 >
 */
INLINE
vec3_t
vec3_0
( void )
{
    return ( vec3_t ){{ 0.0f , 0.0f , 0.0f }};
}

/**
 * @brief Forms the three-component unit vector.
 * @return < 1 , 1 , 1 >
 */
INLINE
vec3_t
vec3_1
( void )
{
    return ( vec3_t ){{ 1.0f , 1.0f , 1.0f }};
}

/**
 * @brief Forms the three-component up vector.
 * @return < 0 , 1 , 0 >
 */
INLINE
vec3_t
vec3_u
( void )
{
    return ( vec3_t ){{ 0.0f , 1.0f , 0.0f }};
}

/**
 * @brief Forms the three-component down vector.
 * @return < 0 , -1 , 0 >
 */
INLINE
vec3_t
vec3_d
( void )
{
    return ( vec3_t ){{ 0.0f , -1.0f , 0.0f }};
}

/**
 * @brief Forms the three-component left vector.
 * @return < -1 , 0 , 0 >
 */
INLINE
vec3_t
vec3_l
( void )
{
    return ( vec3_t ){{ -1.0f , 0.0f , 0.0f }};
}

/**
 * @brief Forms the three-component right vector.
 * @return < 1 , 0 , 0 >
 */
INLINE
vec3_t
vec3_r
( void )
{
    return ( vec3_t ){{ 1.0f , 0.0f , 0.0f }};
}

/**
 * @brief Forms the three-component forward vector.
 * @return < 1 , 0 , 0 >
 */
INLINE
vec3_t
vec3_f
( void )
{
    return ( vec3_t ){{ 0.0f , 0.0f , 1.0f }};
}

/**
 * @brief Forms the three-component back vector.
 * @return < 1 , 0 , 0 >
 */
INLINE
vec3_t
vec3_b
( void )
{
    return ( vec3_t ){{ 0.0f , 0.0f, -1.0f }};
}

/**
 * @brief Forms a three-component vector by scaling a provided
 * three-component vector by a constant factor.
 * @param v A three-component vector of the form < i , j , k >
 * @param s A scalar.
 * @return < s*i , s*j , s*k >
 */
INLINE
vec3_t
vec3_scale
(   vec3_t  v
,   f32     s
)
{
    return ( vec3_t ){{ s * v.i , s * v.j , s * v.k }};
}

/**
 * @brief Computes the sum of two three-component vectors.
 * @param a A three-component vector.
 * @param b A three-component vector.
 * @return a + b
 */
INLINE
vec3_t
vec3_sum
(   vec3_t a
,   vec3_t b
)
{
    return ( vec3_t ){{ a.i + b.i , a.j + b.j , a.k + b.k }};
}

/**
 * @brief Computes the difference of two three-component vectors.
 * @param a A three-component vector.
 * @param b A three-component vector.
 * @return a - b
 */
INLINE
vec3_t
vec3_dif
(   vec3_t a
,   vec3_t b
)
{
    return ( vec3_t ){{ a.i - b.i , a.j - b.j , a.k - b.k }};
}

/**
 * @brief Computes the scalar product of two three-component vectors.
 * @param a A three-component vector.
 * @param b A three-component vector.
 * @return The scalar product of a and b.
 */
INLINE
f32
vec3_dot
(   vec3_t a
,   vec3_t b
)
{
    return a.i * b.i + a.j * b.j + a.k * b.k;
}

/**
 * @brief Computes the cross product of two three-component vectors.
 * @param a A three-component vector.
 * @param b A three-component vector.
 * @return a x b
 */
INLINE
vec3_t
vec3_cross
(   vec3_t a
,   vec3_t b
)
{
    return ( vec3_t ){{ a.j * b.k - a.k * b.j
                      , a.k * b.i - a.i * b.k
                      , a.i * b.j - a.j * b.i
                     }};
}

/**
 * @brief Computes the magnitude of a three-component vector.
 * @param v A three-component vector.
 * @return || v ||
 */
INLINE
f32
vec3_mag
(   vec3_t v
)
{
    return sqrt ( vec3_dot ( v , v ) );
}

/**
 * @brief Normalizes a provided three-component vector.
 * @param v The three-component vector to normalize.
 */
INLINE
void
vec3_normalize
(   vec3_t* v
)
{
    const f32 mag = vec3_mag ( *v );
    ( *v ).i /= mag;
    ( *v ).j /= mag;
    ( *v ).k /= mag;
}

/**
 * @brief Forms a normalized three-component vector from a provided 
 * three-component vector.
 * @param v A three-component vector.
 * @return v / || v ||
 */
INLINE
vec3_t
vec3_norm
(   vec3_t v
)
{
    const f32 mag = vec3_mag ( v );
    return ( vec3_t ){{ v.i / mag , v.j / mag , v.k / mag }};
}

/**
 * @brief Compares each component of a three-component vector to the
 * corresponding component of the other three-component vector. Ensures no
 * component difference is greater than the provided tolerance.
 * @param a A three-component vector.
 * @param b A three-component vector.
 * @param t The difference tolerance.
 * @return true if all components within tolerance; false otherwise.
 */
INLINE
bool
vec3_cmp
(   vec3_t  a
,   vec3_t  b
,   f32     t
)
{
    return !(  abs ( a.i - b.i ) > t
            || abs ( a.j - b.j ) > t
            || abs ( a.k - b.k ) > t
            );
}

/**
 * @brief Calculates the displacement between two three-component vectors.
 * @param a A three-component vector.
 * @param b A three-component vector.
 * @return The displacement from a to b.
 */
INLINE
f32
vec3_disp
(   vec3_t a
,   vec3_t b
)
{
    return vec3_mag ( vec3_dif ( a , b ) );
}

/**
 * @brief Forms a vector from the four provided components.
 * @param i .
 * @param j .
 * @param k .
 * @param l .
 * @return < i , j , k , l >
 */
INLINE
vec4_t
vec4
(   f32 i
,   f32 j
,   f32 k
,   f32 l
)
{
    return ( vec4_t ){{ i , j , k , l }};
}

/**
 * @brief Forms a four-component vector given a three-component vector and
 * a final component.
 * @param v A three-component vector of the form < i , j , k >
 * @param l The final component.
 * @return < i , j , k , l >
 */
INLINE
vec4_t
vec4_from_vec3
(   vec3_t  v
,   f32     l
)
{
    return ( vec4_t ){{ v.i , v.j , v.k , l }};
}

/**
 * @brief Forms the four-component zero vector.
 * @return < 0 , 0 , 0 , 0 >
 */
INLINE
vec4_t
vec4_0
( void )
{
    return ( vec4_t ){{ 0.0f , 0.0f , 0.0f , 0.0f }};
}

/**
 * @brief Forms the four-component unit vector.
 * @return < 1 , 1 , 1 , 1 >
 */
INLINE
vec4_t
vec4_1
( void )
{
    return ( vec4_t ){{ 1.0f , 1.0f , 1.0f , 1.0f }};
}

/**
 * @brief Forms a four-component vector by scaling a provided
 * four-component vector by a constant factor.
 * @param v A four-component vector of the form < i , j , k , l >
 * @param s A scalar.
 * @return < s*i , s*j , s*k , s*l >
 */
INLINE
vec4_t
vec4_scale
(   vec4_t  v
,   f32     s
)
{
    return ( vec4_t ){{ s * v.i , s * v.j , s * v.k , s * v.l }};
}

/**
 * @brief Computes the sum of two four-component vectors.
 * @param a A three-component vector.
 * @param b A three-component vector.
 * @return a + b
 */
INLINE
vec4_t
vec4_sum
(   vec4_t a
,   vec4_t b
)
{
    return ( vec4_t ){{ a.i + b.i , a.j + b.j , a.k + b.k , a.l + b.l }};
}

/**
 * @brief Computes the difference of two four-component vectors.
 * @param a A four-component vector.
 * @param b A four-component vector.
 * @return a - b
 */
INLINE
vec4_t
vec4_dif
(   vec4_t a
,   vec4_t b
)
{
    return ( vec4_t ){{ a.i - b.i , a.j - b.j , a.k - b.k , a.l - b.l }};
}

/**
 * @brief Computes the scalar product of two four-component vectors.
 * @param a A four-component vector.
 * @param b A four-component vector.
 * @return The scalar product of a and b.
 */
INLINE
f32
vec4_dot
(   vec4_t a
,   vec4_t b
)
{
    return a.i * b.i + a.j * b.j + a.k * b.k + a.l * b.l;
}

/**
 * @brief Computes the magnitude of a four-component vector.
 * @param v A four-component vector of the form < i , j , k >
 * @return || v ||
 */
INLINE
f32
vec4_mag
(   vec4_t v
)
{
    return sqrt ( vec4_dot ( v , v ) );
}

/**
 * @brief Normalizes a provided four-component vector.
 * @param v The four-component vector to normalize.
 */
INLINE
void
vec4_normalize
(   vec4_t* v
)
{
    const f32 mag = vec4_mag ( *v );
    ( *v ).i /= mag;
    ( *v ).j /= mag;
    ( *v ).k /= mag;
    ( *v ).l /= mag;
}

/**
 * @brief Forms a normalized four-component vector from a provided 
 * four-component vector.
 * @param v The four-component vector to normalize.
 * @return v / || v ||
 */
INLINE
vec4_t
vec4_norm
(   vec4_t v
)
{
    const f32 mag = vec4_mag ( v );
    return ( vec4_t ){{ v.i / mag , v.j / mag , v.k / mag , v.l / mag }};
}

/**
 * @brief Compares each component of a four-component vector to the
 * corresponding component of the other four-component vector. Ensures no
 * component difference is greater than the provided tolerance.
 * @param a A four-component vector.
 * @param b A four-component vector.
 * @param t The difference tolerance.
 * @return true if all components within tolerance; false otherwise.
 */
INLINE
bool
vec4_cmp
(   vec4_t  a
,   vec4_t  b
,   f32     t
)
{
    return !(  abs ( a.i - b.i ) > t
            || abs ( a.j - b.j ) > t
            || abs ( a.k - b.k ) > t
            || abs ( a.l - b.l ) > t
            );
}

/**
 * @brief Calculates the displacement between two four-component vectors.
 * @param a A four-component vector.
 * @param b A four-component vector.
 * @return The displacement from a to b.
 */
INLINE
f32
vec4_disp
(   vec4_t a
,   vec4_t b
)
{
    return vec4_mag ( vec4_dif ( a , b ) );
}

#endif  // MATH_VECTOR_H
