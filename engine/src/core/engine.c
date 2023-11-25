/**
 * @file engine.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Implementation of the engine header.
 * (see engine.h for additional details)
 */
#include "core/engine.h"

#include "core/clock.h"
#include "core/event.h"
#include "core/logger.h"
#include "core/input.h"
#include "core/memory.h"
#include "core/string.h"

#include "chess/chess.h"

#include "memory/linear_allocator.h"

// Defaults.
#define SUBSYSTEM_ALLOCATOR_CAP_DEFAULT ( MEBIBYTES ( 16 ) )

// Type definition for internal engine state.
typedef struct
{
    application_t*      app;

    bool                running;
    bool                suspended;

    i16                 window_w;
    i16                 window_h;

    clock_t             clock;
    f64                 system_time;

    linear_allocator_t  subsystem_allocator;
    
    u64                 logger_subsystem_memory_requirement;
    void*               logger_subsystem_state;
    
    u64                 event_subsystem_memory_requirement;
    void*               event_subsystem_state;

    u64                 platform_subsystem_memory_requirement;
    void*               platform_subsystem_state;

    u64                 chess_subsystem_memory_requirement;
    void*               chess_subsystem_state;
}
state_t;

// Global application state.
static state_t* state;

/**
 * @brief Event handler.
 */
bool
engine_on_event
(   u16             code
,   void*           sender
,   void*           listener
,   event_context_t ctx 
);

bool
engine_startup
(   application_t* app
)
{
    if ( ( *app ).internal )
    {
        LOGERROR ( "engine_startup: called more than once." );
        return false;
    }

    // Initialize memory subsystem.
    if ( !memory_startup ( ( *app ).config.memory_requirement ) )
    {
        LOGERROR ( "Failed to initialize memory subsystem." );
        return false;
    }

    // Initialize internal engine state.
    ( *app ).internal = memory_allocate ( sizeof ( state_t )
                                        , MEMORY_TAG_ENGINE
                                        );
    state = ( *app ).internal;
    ( *state ).app = app;
    ( *state ).running = false;
    ( *state ).suspended = false;
    
    // Initialize subsystem memory allocator.
    const u64 subsystem_allocator_cap = SUBSYSTEM_ALLOCATOR_CAP_DEFAULT;
    linear_allocator_startup ( subsystem_allocator_cap
                             , 0
                             , &( *state ).subsystem_allocator
                             );

    // Initialize subsystems.

    // Logger.
    logger_startup ( &( *state ).logger_subsystem_memory_requirement , 0 );
    ( *state ).logger_subsystem_state = linear_allocator_allocate ( &( *state ).subsystem_allocator
                                                                  , ( *state ).logger_subsystem_memory_requirement
                                                                  );
    if ( !logger_startup ( &( *state ).logger_subsystem_memory_requirement
                         , ( *state ).logger_subsystem_state
                         ))
    {
        LOGERROR ( "engine_startup: Failed to initialize logging subsystem." );
        return false;
    }

    LOGINFO ( "CCE engine (ver. %i.%i.%i) is starting. . ."
            , VERSION_MAJOR , VERSION_MINOR , VERSION_PATCH
            );

    // Event handler.
    event_startup ( &( *state ).event_subsystem_memory_requirement , 0 );
    ( *state ).event_subsystem_state = linear_allocator_allocate ( &( *state ).subsystem_allocator
                                                                 , ( *state ).event_subsystem_memory_requirement
                                                                 );
    if ( !event_startup ( &( *state ).event_subsystem_memory_requirement
                        , ( *state ).event_subsystem_state
                        ))
    {
        LOGERROR ( "engine_startup: Failed to initialize event handling subsystem." );
        return false;
    }

    // Register event listeners.
    event_register ( EVENT_CODE_APPLICATION_QUIT , 0 , engine_on_event );
    
    // Platform.
    platform_startup ( &( *state ).platform_subsystem_memory_requirement
                     , 0
                     );
    ( *state ).platform_subsystem_state = linear_allocator_allocate ( &( *state ).subsystem_allocator
                                                                    , ( *state ).platform_subsystem_memory_requirement
                                                                    );
    if ( !platform_startup ( &( *state ).platform_subsystem_memory_requirement
                           , ( *state ).platform_subsystem_state
                           ))
    {
        LOGERROR ( "engine_startup: Failed to initialize platform subsystem." );
        return false;
    }

    // Chess engine.
    chess_startup ( &( *state ).chess_subsystem_memory_requirement , 0 );
    ( *state ).chess_subsystem_state = linear_allocator_allocate ( &( *state ).subsystem_allocator
                                                                 , ( *state ).chess_subsystem_memory_requirement
                                                                 );
    if ( !chess_startup ( &( *state ).chess_subsystem_memory_requirement
                        , ( *state ).chess_subsystem_state
                        ))
    {
        LOGERROR ( "engine_startup: Failed to initialize chess engine subsystem." );
        return false;
    }

    // Initialize the user application.
    if ( !( *( ( *state ).app ) ).startup ( ( *state ).app ) )
    {
        LOGFATAL ( "engine_startup: Failed to initialize user application." );
        return false;
    }
    
    return true;
}

bool
engine_run
( void )
{
    ( *state ).running = true;
    
    // Initialize the clock and system time.
    clock_start ( &( *state ).clock );
    clock_update ( &( *state ).clock );
    ( *state ).system_time = ( *state ).clock.elapsed;
    f64 runtime = 0;

    // Print memory usage information.
    char* stat = memory_stat ();
    LOGINFO ( stat );
    string_free ( stat );

    while ( ( *state ).running )
    {
        if ( !platform_pump_messages () )
        {
            ( *state ).running = false;
        }

        if ( !( *state ).suspended )
        {
            // Update the clock.
            clock_update ( &( *state ).clock );
            f64 t = ( *state ).clock.elapsed;
            f64 dt = t - ( *state ).system_time;
            f64 t_start = platform_get_absolute_time ();

            // Update the user application.
            if ( !( *( ( *state ).app ) ).update ( ( *state ).app , ( f32 ) dt ) )
            {
                LOGFATAL ( "engine_run: Failed to update user application, shutting down." );
                ( *state ).running = false;
                break;
            }

            // Calculate the time the frame took to process.
            f64 t_end = platform_get_absolute_time ();
            f64 t_elapsed = t_end - t_start;
            runtime += t_elapsed;

            // Update system time again.
            ( *state ).system_time = t;

            LOGDEBUG ( "Application update complete.\n\tTook %f seconds." , t_elapsed );            
        }
    }
    ( *state ).running = false;// Failsafe.
    
    // Shutdown user application.
    ( *( ( *state ).app ) ).shutdown ( ( *state ).app );
     
    // Shutdown subsystems.
    chess_shutdown ( ( *state ).chess_subsystem_state );
    platform_shutdown ( ( *state ).platform_subsystem_state );
    event_shutdown ( ( *state ).event_subsystem_state );
    logger_shutdown ( ( *state ).logger_subsystem_state );    

    // Free memory used by subsystem memory allocator.
    linear_allocator_free ( &( *state ).subsystem_allocator );
    linear_allocator_shutdown ( &( *state ).subsystem_allocator );

    // Free memory used by internal engine state.
    memory_free ( state
                , sizeof ( state_t )                 
                , MEMORY_TAG_ENGINE
                );
    state = 0;
    
    // Shutdown memory subsystem.
    memory_shutdown ();
 
    return true;
}

bool
engine_on_event
(   u16             code
,   void*           sender
,   void*           listener
,   event_context_t ctx
)
{
    switch ( code )
    {
        case EVENT_CODE_APPLICATION_QUIT:
        {
            LOGINFO ( "engine_on_event: EVENT_CODE_APPLICATION_QUIT received, shutting down." );
            ( *state ).running = false;
            return true;
        }
    }

    return false;
}
