/**
 * @file ascii.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief ASCII datatype operations.
 */
#ifndef ASCII_H
#define ASCII_H

INLINE
char
uppercase
(   const char c
)
{
    return c + 32;
}

INLINE
char
lowercase
(   const char c
)
{
    return c - 32;
}

INLINE
bool
alpha
(   const char c
)
{
    return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' );
}

INLINE
bool
numeric
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
    return alpha ( c ) || numeric ( c );
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

#endif  // ASCII_H
