/**
 * @file detect.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Compile-time platform detection.
 */
#ifndef PLATFORM_DETECT_H
#define PLATFORM_DETECT_H

 // MICROSOFT
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    // WINDOWS: 64-bit
    #ifdef _WIN64
    #define PLATFORM_WINDOWS 1
    //  WINDOWS: OTHER
    #else
    #error "64-bit Windows is required."
    #endif

// LINUX
#elif defined(__linux__) || defined(__gnu_linux__)
    #define PLATFORM_LINUX 1
    // LINUX: ANDROID OS
    #if defined(__ANDROID__)
    #define PLATFORM_ANDROID 1
    #endif

// UNIX-BASED OS
#elif defined(__unix__)
    #define PLATFORM_UNIX 1

// OTHER POSIX-COMPLIANT OS
#elif defined(_POSIX_VERSION)
    #define PLATFORM_POSIX 1

// APPLE
#elif __APPLE__
    #define PLATFORM_APPLE 1
    #include <TargetConditionals.h>
    // APPLE: iOS EMULATOR
    #if TARGET_IPHONE_SIMULATOR
    #define PLATFORM_IOS 1
    #define PLATFORM_IOS_EMULATOR 1
    // APPLE: iOS
    #elif TARGET_OS_IPHONE
    #define PLATFORM_IOS 1
    // APPLE: OTHER MAC OS
    #elif TARGET_OS_MAC
    #define PLATFORM_MAC 1
    // APPLE: UNKNOWN
    #else
    #error "Unknown Apple platform."
    #endif

// OTHER
#else
#error "Unknown platform."
#endif

#endif  // PLATFORM_DETECT_H
