/**
 * @file application.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Implementation of the application header.
 * (see application.h for additional details)
 */
#include "application.h"

#include "core/event.h"
#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"

#include "chess/chess.h"

// Global application state.
static cce_t* state;

bool
cce_startup
(   application_t*  app
)
{
    // Query chess engine memory requirement.
    u64 memory_requirement;
    chess_startup ( &memory_requirement , 0 );

    // Allocate memory for application state, including chess engine state.
    state = memory_allocate ( memory_requirement + sizeof ( cce_t )
                            , MEMORY_TAG_APPLICATION
                            );
    ( *app ).app_state = state;

    ( *state ).chess_engine_memory_requirement = memory_requirement;
    ( *state ).chess_engine_state = ( ( void* ) state ) + sizeof ( cce_t );

    // Start chess engine.
    if ( !chess_startup ( &( *state ).chess_engine_memory_requirement
                        , ( *state ).chess_engine_state
                        ))
    {
        return false;
    } 
   
    return true;
}

void
cce_shutdown
(   application_t*  app
)
{
    if ( !state )
    {
        return;
    }

    // Shutdown chess engine.
    chess_shutdown ( ( *state ).chess_engine_state );

    // Free memory used by application.
    memory_free ( state
                , ( *state ).chess_engine_memory_requirement + sizeof ( cce_t )
                , MEMORY_TAG_APPLICATION
                );

    state = 0;
}

bool
cce_update
(   application_t*  app
,   f32             dt
)
{
    // Update chess engine.
    if ( !state || !chess_update () )
    {
        return false;
    }
    
    // Quit the application.
    event_context_t ctx = {};
    event_fire ( EVENT_CODE_APPLICATION_QUIT , 0 , ctx );

    return true;
}

void
cce_render
(   application_t*  app
,   f32             dt
)
{
    if ( !state )
    {
        return;
    }

    // Render chess board.
    chess_render ();
}
