/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file math.c
 * @brief Implementation of the math header.
 * (see math.h for additional details)
 */
#include "math/math.h"

#include "core/string.h"

#include "platform/platform.h"

// Undefine preprocessor bindings which have name conflicts with <math.h> and <stdlib.h>
#undef abs
#undef sqrt
#undef sin
#undef cos
#undef tan
#undef asin
#undef acos
#undef atan
#undef random

#include <math.h>
#include <stdlib.h>

// Defaults.
#define MATRIX_STRING_MAX_LENGTH 512

// ( see random() ).
static bool random_seeded = false;

f32
_abs
(   f32 x
)
{
    return fabsf ( x );
}

f32
_sqrt
(   f32 x
)
{
    return sqrtf ( x );
}


f32
_sin
(   f32 x
)
{
    return sinf ( x );
}

f32
_cos
(   f32 x
)
{
    return cosf ( x );
}

f32
_tan
(   f32 x
)
{
    return tanf ( x );
}

f32
_asin
(   f32 x
)
{
    return asinf ( x );
}

f32
_acos
(   f32 x
)
{
    return acosf ( x );
}

f32
_atan
(   f32 x
)
{
    return atanf ( x );
}

i32
_random
( void )
{
    if ( !random_seeded )
    {
        srand ( ( u32 ) platform_get_absolute_time () );
        random_seeded = true;
    }
    return rand ();
}

i32
random2
(   i32 min
,   i32 max
)
{
    if ( !random_seeded )
    {
        srand ( ( u32 ) platform_get_absolute_time () );
        random_seeded = true;
    }
    return ( rand () % ( max - min + 1 ) ) + min;
}

i64
random64
( void )
{
    const u64 a = _random () & 0xFFFF;
    const u64 b = _random () & 0xFFFF;
    const u64 c = _random () & 0xFFFF;
    const u64 d = _random () & 0xFFFF;
    return a | ( b << 16 ) | ( c << 32 ) | ( d << 48 );
}

f32
randomf
( void )
{
    return ( f32 ) _random () / ( f32 ) RAND_MAX;
}

f32
randomf2
(   f32 min
,   f32 max
)
{
    return min + ( f32 ) _random () / ( ( f32 ) RAND_MAX / ( max - min ) );
}

char*
string_mat4
(   const mat4_t m
)
{
    char buf[ MATRIX_STRING_MAX_LENGTH ];
    string_format ( buf
                  , "\n\t|\t%.2f\t|\t%.2f\t|\t%.2f\t|\t%.2f\t|"
                    "\n\t|\t%.2f\t|\t%.2f\t|\t%.2f\t|\t%.2f\t|"
                    "\n\t|\t%.2f\t|\t%.2f\t|\t%.2f\t|\t%.2f\t|"
                    "\n\t|\t%.2f\t|\t%.2f\t|\t%.2f\t|\t%.2f\t|"
                  , m.data[ 0 ]
                  , m.data[ 1 ]
                  , m.data[ 2 ]
                  , m.data[ 3 ]
                  , m.data[ 4 ]
                  , m.data[ 5 ]
                  , m.data[ 6 ]
                  , m.data[ 7 ]
                  , m.data[ 8 ]
                  , m.data[ 9 ]
                  , m.data[ 10 ]
                  , m.data[ 11 ]
                  , m.data[ 12 ]
                  , m.data[ 13 ]
                  , m.data[ 14 ]
                  , m.data[ 15 ]
                  );
    return string_allocate_from ( buf );
}
