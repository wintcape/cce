/**
 * @file string.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines common string operations.
 */
#ifndef STRING_H
#define STRING_H

#include "common.h"

// Global constants.
#define STACK_STRING_MAX_LENGTH 32000

/**
 * @brief Calculates the number of characters in a string.
 * @param s The string to read.
 * @return The number of characters in s.
 */
u64
string_length
(   const char* s
);

/**
 * @brief Generates a copy of a provided string.
 * @param s The string to copy.
 * @return A copy of s.
 */
char*
string_allocate_from
(   const char* s
);

/**
 * @brief Frees the memory used by a provided string.
 * @param s The string to free.
 */
void
string_free
(   char* s
);

/**
 * @brief Calculates if two strings of data are of equal.
 * @param s1.
 * @param s2.
 * @return A copy of s.
 */
bool
string_equal
(   const char* s1
,   const char* s2
);

/**
 * @brief String format function.
 * @param dst  Output buffer.
 * @param fmt  Formatting string.
 * @param args The variadic argument list.
 * @return The number of bytes written to dst.
 */
i32
string_format_v
(   char*       dst
,   const char* fmt
,   void*       args
);

/**
 * @see string_format_v
 */
i32
string_format
(   char*       dst
,   const char* fmt
,   ...
);

/**
 * @brief Bytesize to string. Converts size into appropriate units. The unit abbreviation
 * string is returned, and the converted size is written into the output buffer.
 * @param size Size in bytes.
 * @param amt Output buffer for amount (after conversion to appropriate units.
 * @return GiB | MiB | KiB | B
 */
const char*
string_bytesize
(   u64     size
,   f32*    amt
);

#endif  // STRING_H