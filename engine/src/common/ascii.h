/**
 * @file ascii.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief ASCII datatype operations.
 */
#ifndef ASCII_H
#define ASCII_H

#include "common/inline.h"
#include "common/types.h"

INLINE
bool
uppercase
(   const char c
)
{
    return c >= 'A' && c <= 'Z';
}

INLINE
bool
lowercase
(   const char c
)
{
    return c >= 'a' && c <= 'z';
}

INLINE
bool
alpha
(   const char c
)
{
    return uppercase ( c ) || lowercase ( c );
}

INLINE
bool
digit
(   const char c
)
{
    return c >= '0' && c <= '9';
}

INLINE
bool
alphanumeric
(   const char c
)
{
    return alpha ( c ) || digit ( c );
}

INLINE
bool
whitespace
(   const char c
)
{
    return c ==  ' ' || c == '\n' || c == '\t'
        || c == '\v' || c == '\f' || c == '\r'
        ;
}

INLINE
bool
newline
(   const char c
)
{
    return c == '\n' || c == '\r';
}

INLINE
char
to_uppercase
(   const char c
)
{
    return lowercase ( c ) ? c - 32
                           : c
                           ;
}

INLINE
char
to_lowercase
(   const char c
)
{
    return uppercase ( c ) ? c + 32
                           : c
                           ;
}

INLINE
u8
to_digit
(   const unsigned char c
)
{
    return digit ( c ) ? c - ( ( unsigned char ) '0' )
                       : c
                       ;
}

INLINE
unsigned char
char_digit
(   const u8 i
)
{
    return ( ( unsigned char ) '0' ) + i;
}

#endif  // ASCII_H
