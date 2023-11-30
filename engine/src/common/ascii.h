/**
 * @file ascii.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief ASCII datatype operations.
 */
#ifndef ASCII_H
#define ASCII_H

INLINE
char
touppercase
(   const char c
)
{
    return c - 32;
}

INLINE
char
tolowercase
(   const char c
)
{
    return c + 32;
}

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
u8
chri
(   const unsigned char c
)
{
    return c - ( ( unsigned char ) '0' );
}

INLINE
unsigned char
ichr
(   const u8 i
)
{
    return i + ( ( unsigned char ) '0' );
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
