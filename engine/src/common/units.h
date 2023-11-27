/**
 * @file units.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines common measurement units.
 */
#ifndef UNITS_H
#define UNITS_H

#define GIBIBYTES(amt) ( (amt) * 1024ULL*1024ULL*1024ULL )
#define MEBIBYTES(amt) ( (amt) * 1024ULL*1024ULL )
#define KIBIBYTES(amt) ( (amt) * 1024ULL )

#define GIGABYTES(amt) ( (amt) * 1000ULL*1000ULL*1000ULL )
#define MEGABYTES(amt) ( (amt) * 1000ULL*1000ULL )
#define KILOBYTES(amt) ( (amt) * 1000ULL )

#endif  // UNITS_H
