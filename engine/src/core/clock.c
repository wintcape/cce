/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file clock.c
 * @brief Implementation of the clock header.
 * (see clock.h for additional details)
 */
#include "core/clock.h"

#include "platform/platform.h"

void
clock_update
(   clock_t* clock
)
{
    if ( ( *clock ).start )
    {
        ( *clock ).elapsed = platform_get_absolute_time () - ( *clock ).start;
    }
}

/**
 * @brief Resets the elapsed time for the provided clock and starts it.
 * @param clock The clock to mutate.
 */
void
clock_start
(   clock_t* clock
)
{
    ( *clock ).start = platform_get_absolute_time ();
    ( *clock ).elapsed = 0;
}

/**
 * @brief Stops the provided clock.
 * @param clock The clock to mutate.
 */
void
clock_stop
(   clock_t* clock
)
{
    ( *clock ).start = 0;
}
