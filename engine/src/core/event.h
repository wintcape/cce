/**
 * @file event.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Engine subsystem for event handling.
 */
#ifndef EVENT_H
#define EVENT_H

#include "common.h"

// Type definition for event context.
typedef struct
{
    union
    {
        i64     i64[ 2 ];
        u64     u64[ 2 ];
        f64     f64[ 2 ];

        i32     i32[ 4 ];
        u32     u32[ 4 ];
        f32     f32[ 4 ];

        i16     i16[ 8 ];
        u16     u16[ 8 ];

        i8      i8[ 16 ];
        u8      u8[ 16 ];
        
        char    s[ 16 ];
    }
    data;
}
event_context_t;

// Type definition for on_event functions.
typedef bool ( *on_event_t )( u16               code
                            , void*             sender
                            , void*             listener
                            , event_context_t   ctx
                            );

/**
 * @brief Initializes the event subsystem. Call once to read the memory
 * requirement. Call again passing in a state pointer.
 * @param memory_requirement Output buffer to read memory requirement.
 * @param state Pass 0 to read memory requirement. Otherwise, pass a buffer.
 * @return false on error, true otherwise.
 */
bool
event_startup
(   u64*    memory_requirement
,   void*   state
);

/**
 * @brief Terminates the event subsystem.
 * @param state .
 */
void
event_shutdown
(   void* state
);

/**
 * @brief Register to listen for when events are sent with the provided code.
 * Events with duplicate listener / callback combos will not be registered again
 * and will result in a return value of false.
 * @param code The event code to listen for.
 * @param listener A listener instance. Can be null.
 * @param on_event The callback function to be invoked when the event code is
 * fired.
 * @return true if the event is successfully registered, false otherwise.
 */
bool
event_register
(   u16         code
,   void*       listener
,   on_event_t  on_event
);

/**
 * @brief Unregister from listener for when events are sent with the provided
 * code. If no matching regsitration is found, this function will return false.
 * @param code The event code to stop listening for.
 * @param listener A listener instance. Can be null.
 * @param on_event The callback function to be invoked when the event code is
 * fired.
 * @return true if the event is successfully unregistered, false otherwise.
 */
bool
event_unregister
(   u16         code
,   void*       listener
,   on_event_t  on_event
);

/**
 * @brief Serves an event to listeners of the given code. If one returns true,
 * the event is considered handled and is not passed on to any more listeners.
 * @param code The event code to stop listening for.
 * @param sender The sender. Can be null.
 * @param ctx The event ctx.
 * @return true if the event is successfully handled, false otherwise.
 */
bool
event_fire
(   u16             code
,   void*           sender
,   event_context_t ctx
);

/**
 * Definition for system-internal event codes.
 * Application should use codes beyond 255.
 */
typedef enum
{
    EVENT_CODE_APPLICATION_QUIT     = 0x01 // Shutdown application on next frame.

,   EVENT_CODE_KEY_PRESS            = 0x02
,   EVENT_CODE_KEY_RELEASE          = 0x03

,   EVENT_CODE_MOUSE_BUTTON_PRESS   = 0x04
,   EVENT_CODE_MOUSE_BUTTON_RELEASE = 0x05
,   EVENT_CODE_MOUSE_MOVE           = 0x06
,   EVENT_CODE_MOUSE_WHEEL          = 0x07

,   EVENT_CODE_RESIZE               = 0x08 // Window resize or resolution change.

,   EVENT_CODE_MAX                  = 0xFF // Max value.
}
EVENT_CODE;

#endif  // EVENT_H
