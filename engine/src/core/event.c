/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file event.c
 * @brief Implementation of the event header.
 * (see event.h for additional details)
 */
#include "core/event.h"

#include "core/logger.h"
#include "core/memory.h"

#include "container/array.h"

// Event registry buffer size.
#define MAX_MESSAGE_CODES 16384

// Type definition for an event.
typedef struct
{
    void*       listener;
    on_event_t  callback;
}
event_t;

// Type definition for a single registry entry.
typedef struct
{
    event_t* events;
}
event_registry_t;

// Type definition for event subsystem state.
typedef struct 
{
    event_registry_t registry[ MAX_MESSAGE_CODES ];
}
state_t;

// Global subsystem state.
static state_t* state;

bool
event_startup
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
event_shutdown
(   void* state_
)
{
    if ( !state )
    {
        return;
    }

    // Free the event arrays and their elements.
    for ( u16 i = 0; i < MAX_MESSAGE_CODES; ++i )
    {
        if ( ( *state ).registry[ i ].events )
        {
            array_destroy ( ( *state ).registry[ i ].events );
            ( *state ).registry[ i ].events = 0;
        }
    }

    state = 0;
}

bool
event_register
(   u16         code
,   void*       listener
,   on_event_t  on_event
)
{
    if ( !state )
    {
        return false;
    }
   
    if ( !( *state ).registry[ code ].events )
    {
        ( *state ).registry[ code ].events = array_allocate_new ( event_t );
    }
    
    // Check for a duplicate event in the registry.
    for ( u64 i = 0; i < array_length ( ( *state ).registry[ code ].events ); ++i )
    {
        if ( ( *state ).registry[ code ].events[ i ].listener == listener )
        {
            LOGWARN ( "event_register: duplicate event listener!" );
            return false;
        }
    }

    // Register the event.
    event_t event;
    event.listener = listener;
    event.callback = on_event;
    array_push ( ( *state ).registry[ code ].events , event );

    return true;
}

bool
event_unregister
(   u16         code
,   void*       listener
,   on_event_t  on_event
)
{
    if ( !state )
    {
        return false;
    }
  
    // Unused code.
    if ( !( *state ).registry[ code ].events )
    {
        LOGWARN ( "event_unregister: unused code!" );
        return false;
    }
    
    // Remove the specified event from the registry.
    for ( u64 i = 0; i < array_length ( ( *state ).registry[ code ].events ); ++i )
    {
        event_t event = ( *state ).registry[ code ].events[ i ];
        if ( event.listener == listener && event.callback == on_event )
        {
            event_t null;
            array_remove ( ( *state ).registry[ code ].events , i , &null );
            return true;
        }
    }// Not found.
    
    return false;
}

bool
event_fire
(   u16             code
,   void*           sender
,   event_context_t ctx
)
{
    if ( !state )
    {
        return false;
    }

    // Unused code.
    if ( !( *state ).registry[ code ].events )
    {
        return false;
    }
    
    // Search the registry for the event callback function.
    for ( u64 i = 0; i < array_length ( ( *state ).registry[ code ].events ); ++i )
    {
        event_t event = ( *state ).registry[ code ].events[ i ];
        if ( event.callback ( code , sender , event.listener , ctx ) )
        {
            // Event has been handled, so halt.
            return true;
        }
    }// Not found.
    
    return false;
}
