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

#include "memory/linear_allocator.h"

// Defaults.
#define SUBSYSTEM_ALLOCATOR_CAP_DEFAULT ( MEBIBYTES ( 1 ) )

// Type definition for internal engine state.
typedef struct
{
    application_t*      app;

    bool                running;
    bool                suspended;

    bool                window;
    i16                 window_w;
    i16                 window_h;

    clock_t             clock;
    f64                 system_time;

    linear_allocator_t  subsystem_allocator;
    
    u64                 logger_subsystem_memory_requirement;
    void*               logger_subsystem_state;
    
    u64                 event_subsystem_memory_requirement;
    void*               event_subsystem_state;

    u64                 input_subsystem_memory_requirement;
    void*               input_subsystem_state;

    u64                 platform_subsystem_memory_requirement;
    void*               platform_subsystem_state;
}
state_t;

// Global application state.
static state_t* state;

/**
 * @brief Alias for a windowed application runtime loop (see engine_run).
 */
INLINE
void
ENGINE_RUN_WINDOWED
( void );

/**
 * @brief Alias for a windowless application runtime loop (see engine_run).
 */
INLINE
void
ENGINE_RUN_WINDOWLESS
( void );

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

/**
 * @brief Key event handler.
 */
bool
engine_on_key
(   u16             code
,   void*           sender
,   void*           listener
,   event_context_t ctx
);

/**
 * @brief Window resize event handler.
 */
bool
engine_on_resize
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
        LOGERROR ( "engine_startup: Failed to initialize memory subsystem." );
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
    linear_allocator_create ( subsystem_allocator_cap
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

    // User input.
    if ( ( *( ( *state ).app ) ).config.user_input )
    {
        input_startup ( &( *state ).input_subsystem_memory_requirement , 0 );
        ( *state ).input_subsystem_state = linear_allocator_allocate ( &( *state ).subsystem_allocator
                                                                     , ( *state ).input_subsystem_memory_requirement
                                                                     );
        if ( !input_startup ( &( *state ).input_subsystem_memory_requirement
                            , ( *state ).input_subsystem_state
                            ))
        {
            LOGERROR ( "engine_startup: Failed to initialize user input subsystem." );
            return false;
        }

        // Register input event listeners.
        event_register ( EVENT_CODE_KEY_PRESS , 0 , engine_on_key );
        event_register ( EVENT_CODE_KEY_RELEASE , 0 , engine_on_key );
    }
    else
    {
        ( *state ).input_subsystem_state = 0;
    }
    
    // Platform.
    platform_startup ( &( *state ).platform_subsystem_memory_requirement
                     , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
                     );
    ( *state ).platform_subsystem_state = linear_allocator_allocate ( &( *state ).subsystem_allocator
                                                                    , ( *state ).platform_subsystem_memory_requirement
                                                                    );
    if ( !platform_startup ( &( *state ).platform_subsystem_memory_requirement
                           , ( *state ).platform_subsystem_state
                           , ( *app ).config.window
                           , ( *app ).config.wm_title
                           , ( *app ).config.wm_class
                           , ( *app ).config.window_x
                           , ( *app ).config.window_y
                           , ( *app ).config.window_w
                           , ( *app ).config.window_h
                           ))
    {
        LOGERROR ( "engine_startup: Failed to initialize platform subsystem." );
        return false;
    }

    // Windowed application? Y/N
    ( *state ).window = ( *( ( *state ).app ) ).config.window;
    if ( ( *state ).window )
    {
        ( *state ).window_w = -1;
        ( *state ).window_h = -1;

        // Register window event listeners.
        event_register ( EVENT_CODE_RESIZE , 0 , engine_on_resize );
    }    

    // Initialize the user application.
    if ( !( *( ( *state ).app ) ).startup ( ( *state ).app ) )
    {
        LOGFATAL ( "engine_startup: Failed to initialize user application." );
        return false;
    }
    if ( ( *state ).window )
    {
        ( *( ( *state ).app ) ).on_resize ( ( *state ).app
                                          , ( *state ).window_w
                                          , ( *state ).window_h
                                          );
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
    
    // Print memory usage information.
    char* stat = memory_stat ();
    LOGINFO ( stat );
    string_free ( stat );

    // Runtime loop.
    if ( ( *state ).window )
    {
        ENGINE_RUN_WINDOWED ();
    }
    else
    {
        ENGINE_RUN_WINDOWLESS ();
    }

    ( *state ).running = false;
    
    // Shutdown user application.
    ( *( ( *state ).app ) ).shutdown ( ( *state ).app );
     
    // Shutdown subsystems.
    platform_shutdown ( ( *state ).platform_subsystem_state );
    input_shutdown ( ( *state ).input_subsystem_state );
    event_shutdown ( ( *state ).event_subsystem_state );
    logger_shutdown ( ( *state ).logger_subsystem_state );    

    // Free memory used by subsystem memory allocator.
    linear_allocator_destroy ( &( *state ).subsystem_allocator );

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

/**
 * @brief Runtime loop for a windowed application.
 */
INLINE
void
ENGINE_RUN_WINDOWED
( void )
{
    f64 t;
    f64 dt;
    f64 t_start;
    f64 t_end;
    f64 t_elapsed;
    f64 t_start_update;
    f64 t_end_update;
    f64 t_elapsed_update;
    f64 t_start_render;
    f64 t_end_render;
    f64 t_elapsed_render;

    f64 runtime = 0;  
    
    u8 frame = 0;
    f64 target_fps = 1.0f / 60.0f;

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
            t = ( *state ).clock.elapsed;
            dt = t - ( *state ).system_time;
            t_start = platform_get_absolute_time ();

            // Update the user application.
            t_start_update = t_start;
            if ( !( *( ( *state ).app ) ).update ( ( *state ).app , ( f32 ) dt ) )
            {
                LOGFATAL ( "engine_run: Failed to update user application, shutting down." );
                ( *state ).running = false;
                break;
            }

            // Calculate the time the application took to update.
            t_end_update = platform_get_absolute_time ();
            t_elapsed_update = t_end_update - t_start_update;
            LOGDEBUG ( "Application update complete.\n\tTook %f seconds."
                     , t_elapsed_update
                     );

            // Render the user application.
            t_start_render = platform_get_absolute_time ();
            ( *( ( *state ).app ) ).render ( ( *state ).app , ( f32 ) dt );

            // Calculate the time the application took to render.
            t_end_render = platform_get_absolute_time ();
            t_elapsed_render = t_end_render - t_start_render;
            LOGDEBUG ( "Application render complete.\n\tTook %f seconds."
                     , t_elapsed_render
                     );

            // Update runtime.
            t_end = platform_get_absolute_time ();
            t_elapsed = t_end - t_start;
            runtime += t_elapsed;

            // If there is time remaining (based on target_fps), hand over
            // control to the OS.
            f64 t_rem = target_fps - t_elapsed; 
            if ( t_rem > 0 )
            {
                u64 t_rem_ms = t_rem * 1000;
                /*bool limit_fps = false;*/
                if ( t_rem_ms /*&& limit_fps*/ )
                {
                    platform_sleep ( t_rem_ms - 1 );
                }
                frame += 1;
            }

            // Handle user input last (changes are applied to the next frame).
            if ( ( *state ).input_subsystem_state )
            {
                input_update ( dt );
            }

            // Update system time again.
            ( *state ).system_time = t;
        }
    }
}

/**
 * @brief Runtime loop for a windowless application.
 */
INLINE
void
ENGINE_RUN_WINDOWLESS
( void )
{
    f64 t;
    f64 dt;
    f64 t_start;
    f64 t_end;
    f64 t_elapsed;
    f64 t_start_update;
    f64 t_end_update;
    f64 t_elapsed_update;
    f64 t_start_render;
    f64 t_end_render;
    f64 t_elapsed_render;

    f64 runtime = 0;    
    
    while ( ( *state ).running )
    {
        if ( !( *state ).suspended )
        {
            // Update the clock.
            clock_update ( &( *state ).clock );
            t = ( *state ).clock.elapsed;
            dt = t - ( *state ).system_time;
            t_start = platform_get_absolute_time ();

            // Update the user application.
            t_start_update = t_start;
            if ( !( *( ( *state ).app ) ).update ( ( *state ).app , ( f32 ) dt ) )
            {
                LOGFATAL ( "engine_run: Failed to update user application, shutting down." );
                ( *state ).running = false;
                break;
            }

            // Calculate the time the application took to update.
            t_end_update = platform_get_absolute_time ();
            t_elapsed_update = t_end_update - t_start_update;
            LOGDEBUG ( "Application update complete.\n\tTook %f seconds."
                     , t_elapsed_update
                     );

            // Render the user application.
            t_start_render = platform_get_absolute_time ();
            ( *( ( *state ).app ) ).render ( ( *state ).app , ( f32 ) dt );

            // Calculate the time the application took to render.
            t_end_render = platform_get_absolute_time ();
            t_elapsed_render = t_end_render - t_start_render;
            LOGDEBUG ( "Application render complete.\n\tTook %f seconds."
                     , t_elapsed_render
                     );

            // Update runtime.
            t_end = platform_get_absolute_time ();
            t_elapsed = t_end - t_start;
            runtime += t_elapsed;

            // Update system time.
            ( *state ).system_time = t;
        }
    }
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

bool
engine_on_key
(   u16             code
,   void*           sender
,   void*           listener
,   event_context_t ctx
)
{
    if ( code == EVENT_CODE_KEY_PRESS )
    {
        u16 key = ctx.data.u16[ 0 ];
        if ( key >= KEY_A && key <= KEY_Z )
        {
        }
        else
        {
            switch ( key )
            {
                case KEY_ESCAPE:
                {
                    break;
                }
                case KEY_LSHIFT:
                {
                    break;
                }
                case KEY_RSHIFT:
                {
                    break;
                }
                case KEY_LCTRL:
                {
                    break;
                }
                case KEY_RCTRL:
                {
                    break;
                }
                case KEY_LALT:
                {
                    break;
                }
                case KEY_RALT:
                {
                    break;
                }
                default:
                {
                }
            }// End switch.
        }
    }
    else if ( code == EVENT_CODE_KEY_RELEASE )
    {
        u16 key = ctx.data.u16[ 0 ];
        switch ( key )
        {
            default:
            {
            }
        }
    }
    return false;
}

bool
engine_on_resize
(   u16             code
,   void*           sender
,   void*           listener
,   event_context_t ctx
)
{
    if ( code != EVENT_CODE_RESIZE )
    {
        return false;
    }

    u16 w = ctx.data.u16[ 0 ];
    u16 h = ctx.data.u16[ 1 ];
    
    if ( w == ( *state ).window_w && h == ( *state ).window_h )
    {
        return false;
    }

    ( *state ).window_w = w;
    ( *state ).window_h = h;

    if ( ( *state ).window_w == 0 || ( *state ).window_h == 0 )
    {
        LOGDEBUG ( "Window minimized. Suspending application. . ." );
        ( *state ).suspended = true;
        return true;
    }
    else
    {
        if ( ( *state ).suspended )
        {
            LOGDEBUG ( "Window restored. Resuming application." );
            ( *state ).suspended = false;
        }

        ( *( ( *state ).app ) ).on_resize ( ( *state ).app , w , h );

        LOGDEBUG ( "Window resized: w / h :\t%i / %i"
                 , ( *state ).window_w
                 , ( *state ).window_h
                 );
    }

    return false;
}

void
application_query_window_size
(   i32* w
,   i32* h
)
{
    *w = ( *state ).window_w;
    *h = ( *state ).window_h;
}
