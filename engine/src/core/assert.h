/**
 * @file assert.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines runtime assertion behavior.
 */
#ifndef ASSERT_H
#define ASSERT_H

#include "common.h"

// Enable runtime assert statements? Y/N
#define ASSERT_ENABLED  1

#if ASSERT_ENABLED == 1

// Compiler-independent definition for code breakpoint.
#if _MSC_VER
    #include <intrin.h>
    #define brkdbg()    ( __debugbreak () )
#else
    #define brkdbg()    ( __builtin_trap () )
#endif

/**
 * @brief Interface for logging information on assertion failure.
 * @param expr The expression to assert.
 * @param mesg The message to log on assertion failure.
 * @param file The file containing the caller of the brkdbg invocation that was
 * triggered.
 * @param line The line number in file containing the caller of the brkdbg
 * invocation that was triggered.
 */
void
assertf
(   const char* expr
,   const char* mesg
,   const char* file
,   const i32   line
);

/**
 * @brief Definition for runtime assertion.
 * @param expr The expression to assert.
 */
#define ASSERT(expr)                                    \
{                                                       \
    if ( expr ) {}                                      \
    else                                                \
    {                                                   \
        assertf ( #expr , "" , __FILE__ , __LINE__ );   \
        brkdbg ();                                      \
    }                                                   \
}

/**
 * @brief Definition for runtime assertion (with message).
 * @param expr The expression to assert.
 * @param mesg The message to log on assertion failure.
 */
#define ASSERTM(expr,mesg)                              \
{                                                       \
    if ( expr ) {}                                      \
    else                                                \
    {                                                   \
        assertf ( #expr , mesg , __FILE__ , __LINE__ ); \
        brkdbg ();                                      \
    }                                                   \
}

/**
 * @brief Definition for debug-mode-only runtime assertions.
 * @param expr The expression to assert.
 */
#if VERSION_DEBUG == 1
#define ASSERT_DBG(expr)                                \
{                                                       \
    if ( expr ) {}                                      \
    else                                                \
    {                                                   \
        assertf ( #expr , "" , __FILE__ , __LINE__ );   \
        brkdbg ();                                      \
    }                                                   \
}
#else
#define ASSERT_DBG(expr)
#endif

// Disable all if ASSERT_ENABLED != 1.
#else
    #define ASSERT(expr)
    #define ASSERTM(expr,mesg)
    #define ASSERT_DBG(expr)

#endif  // ASSERT_ENABLED
#endif  // ASSERT_H
