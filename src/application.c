/**
 * @file application.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Implementation of the cce header.
 * (see cce.h for additional details)
 */
#include "cce.h"

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
    state = memory_allocate ( sizeof ( cce_t )
                            , MEMORY_TAG_APPLICATION
                            );
    ( *app ).app_state = state;
   
    return true;
}

void
cce_shutdown
(   application_t*  app
)
{
    memory_free ( state
                , sizeof ( cce_t )
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
    
    chess_run_tests ();

    // Quit the application.
    event_context_t ctx = {};
    event_fire ( EVENT_CODE_APPLICATION_QUIT , 0 , ctx );

    return true;
}
