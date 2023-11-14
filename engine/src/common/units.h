/**
 * @file units.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines common measurement units.
 */
#ifndef UNITS_H
#define UNITS_H

#define GIBIBYTES(amount) ( (amount) * 1024ULL*1024ULL*1024ULL )
#define MEBIBYTES(amount) ( (amount) * 1024ULL*1024ULL )
#define KIBIBYTES(amount) ( (amount) * 1024ULL )

#define GIGABYTES(amount) ( (amount) * 1000ULL*1000ULL*1000ULL )
#define MEGABYTES(amount) ( (amount) * 1000ULL*1000ULL )
#define KILOBYTES(amount) ( (amount) * 1000ULL )

#endif  // UNITS_H
