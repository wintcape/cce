/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file input.c
 * @brief Implementation of the input header.
 * (see input.h for additional details)
 */
#include "core/input.h"

#include "core/event.h"
#include "core/logger.h"
#include "core/memory.h"

// Type definition for keyboard state.
typedef struct
{
    bool keys[ 256 ];
}
keyboard_t;

// Type definition for mouse state.
typedef struct
{
    i16 x;
    i16 y;
    u8  buttons[ BUTTON_COUNT ];
}
mouse_t;

// Type definition for user input subsystem state.
typedef struct
{
    keyboard_t  keyboard;
    keyboard_t  keyboard_prev;

    mouse_t     mouse;
    mouse_t     mouse_prev;
}
state_t;

// Global subsystem state.
static state_t* state;

bool
input_startup
(   u64*    memory_requirement
,   void*   state_
)
{
    *memory_requirement = sizeof ( state_t );

    if ( !state_ )
    {
        return true;
    }

    state = state_;
    memory_clear ( state , sizeof ( state_t ) ); 

    return true;
}

void
input_shutdown
(   void* state_
)
{
    state = 0;
}

void
input_update
(   f64 dt
)
{
    if ( !state )
    {
        return;
    }

    // Store the current keyboard and mouse state for the next frame.
    memory_copy ( &( *state ).keyboard_prev
                , &( *state ).keyboard
                , sizeof ( keyboard_t )
                );
    memory_copy ( &( *state ).mouse_prev
                , &( *state ).mouse
                , sizeof ( mouse_t )
                );
}

bool
input_query_button_down
(   BUTTON button
)
{
    if ( !state )
    {
        return false;
    }

    return ( *state ).mouse.buttons[ button ];
}

bool
input_query_button_up
(   BUTTON button
)
{
    if ( !state )
    {
        return true;
    }
    
    return !( *state ).mouse.buttons[ button ];
}

bool
input_query_button_down_prev
(   BUTTON button
)
{
    if ( !state )
    {
        return false;
    }
    
    return ( *state ).mouse_prev.buttons[ button ];
}


bool
input_query_button_up_prev
(   BUTTON button
)
{
    if ( !state )
    {
        return true;
    }
    
    return !( *state ).mouse_prev.buttons[ button ];
}

void
input_query_mouse_position
(   i32* x
,   i32* y
)
{
    if ( !state )
    {
        *x = 0;
        *y = 0;
        return;
    }
    
    *x = ( *state ).mouse.x;
    *y = ( *state ).mouse.y;
}

void
input_query_mouse_position_prev
(   i32* x
,   i32* y
)
{
    if ( !state )
    {
        *x = 0;
        *y = 0;
        return;
    }
    
    *x = ( *state ).mouse_prev.x;
    *y = ( *state ).mouse_prev.y;
}

bool
input_query_key_down
(   KEY key
)
{
    if ( !state )
    {
        return false;
    }
    
    return ( *state ).keyboard.keys[ key ];
}

bool
input_query_key_up
(   KEY key
)
{
    if ( !state )
    {
        return true;
    }

    return !( *state ).keyboard.keys[ key ];
}

bool
input_query_key_down_prev
(   KEY key
)
{
    if ( !state )
    {
        return false;
    }
    
    return ( *state ).keyboard_prev.keys[ key ];
}

bool
input_query_key_up_prev
(   KEY key
)
{
    if ( !state )
    {
        return true;
    }
    
    return !( *state ).keyboard_prev.keys[ key ];
}

void
input_process_button
(   BUTTON  button
,   bool    pressed
)
{
    if ( ( *state ).mouse.buttons[ button ] == pressed )
    {
        return;
    }

    // Update internal state.
    ( *state ).mouse.buttons[ button ] = pressed;

    // Fire the event.
    event_context_t ctx;
    ctx.data.u16[ 0 ] = button;
    event_fire ( pressed ? EVENT_CODE_MOUSE_BUTTON_PRESS
                         : EVENT_CODE_MOUSE_BUTTON_RELEASE
               , 0
               , ctx
               );
}

void
input_process_mouse_move
(   i16 x
,   i16 y
)
{
    if ( ( *state ).mouse.x == x && ( *state ).mouse.y == y )
    {
        return;
    }
    
    // Store the old and new coordinates in the event context.
    event_context_t ctx;
    ctx.data.u16[ 0 ] = x;
    ctx.data.u16[ 1 ] = y;
    ctx.data.u16[ 2 ] = ( *state ).mouse.x;
    ctx.data.u16[ 3 ] = ( *state ).mouse.y;

    // Update internal state.
    ( *state ).mouse.x = x;
    ( *state ).mouse.y = y;

    // Fire the event(s).
    event_fire ( EVENT_CODE_MOUSE_MOVE , 0 , ctx );
}

void
input_process_mouse_wheel
(   i8 dz
)
{
    // Fire the event.
    event_context_t ctx;
    ctx.data.u8[ 0 ] = dz;
    event_fire ( EVENT_CODE_MOUSE_WHEEL , 0 , ctx );
}

void
input_process_key
(   KEY     key
,   bool    pressed
)
{
    if ( ( *state ).keyboard.keys[ key ] == pressed )
    {
        return;
    }

    // Update internal state.
    ( *state ).keyboard.keys[ key ] = pressed;

    // Fire the event.
    event_context_t ctx;
    ctx.data.u16[ 0 ] = key;
    event_fire ( pressed ? EVENT_CODE_KEY_PRESS
                         : EVENT_CODE_KEY_RELEASE
               , 0
               , ctx
               );
}
