/**
 * @file version.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Version info.
 */
#ifndef VERSION_H
#define VERSION_H

// Version number.
#define VERSION_MAJOR   1
#define VERSION_MINOR   0
#define VERSION_PATCH   6

// Release build? Y/N
#define VERSION_RELEASE 0

// Debug mode? Y/N
#define VERSION_DEBUG 1

// Auto-disables debug mode for release builds.
#if VERSION_RELEASE == 1
#define VERSION_DEBUG 0
#endif

#endif  // VERSION_H
