/**
 * @file clock.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines a system clock.
 */
#ifndef CLOCK_H
#define CLOCK_H

#include "common.h"

// Type definition for a clock.
typedef struct
{
    f64 start;
    f64 elapsed;
}
clock_t;

/**
 * @brief Updates a provided clock. To be invoked just before checking the
 * elapsed time. Has no effect on clocks that have not been started.
 * @param clock The clock to mutate.
 */
void
clock_update
(   clock_t* clock
);

/**
 * @brief Resets the elapsed time for the provided clock and starts it.
 * @param clock The clock to mutate.
 */
void
clock_start
(   clock_t* clock
);

/**
 * @brief Stops the provided clock.
 * @param clock The clock to mutate.
 */
void
clock_stop
(   clock_t* clock
);

#endif  // CLOCK_H
