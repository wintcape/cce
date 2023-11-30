/**
 * @file main.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Main program entry point template.
 * (see core/application.h for additional details)
 */
#ifndef MAIN_H
#define MAIN_H

#include "core/application.h"
#include "core/engine.h"
#include "core/logger.h"
#include "core/memory.h"

int
main
( void )
{
    /**
     * Require properly-defined external implementation of application_t and
     * application(application_t*).
     */
    application_t app;
    if ( !application ( &app ) )
    {
        LOGFATAL ( "Failed to create user application ( call to application(application_t*) failed! )" );
        return -1;
    }
    if ( !app.startup )
    {
        LOGFATAL ( "User application data structure is ill-defined: no 'startup' routine specified for application." );
        return -2;        
    }
    if ( !app.shutdown )
    {
        LOGFATAL ( "User application data structure is ill-defined: no 'shutdown' routine specified for application." );
        return -3;        
    }
    if ( !app.update )
    {
        LOGFATAL ( "User application data structure is ill-defined: no 'update' routine specified for application." );
        return -4;        
    }
    if ( !app.render )
    {
        LOGFATAL ( "User application data structure is ill-defined: no 'render' routine specified for application." );
        return -5;        
    }
    if ( !app.on_resize && app.config.window )
    {
        LOGFATAL ( "User application data structure is ill-defined: no 'on_resize' window-resize routine specified for windowed application." );
        return -6;        
    }
    if ( app.on_resize && !app.config.window )
    {
        LOGWARN ( "Application is windowless, but 'on_resize' window-resize routine was specified." );
    }

    // Validate app configuration parameters.
    if ( app.config.window && (   !app.config.window_w
                               || !app.config.window_h
                               || !app.config.wm_title
                               || !app.config.wm_class
                              ))
    {
        LOGFATAL ( "Invalid configuration parameters for windowed application. The following must each be nonzero:"
                   "\n\twm_title = %s"
                   "\n\twm_class = %s"
                   "\n\twindow_w = %i"
                   "\n\twindow_h = %i"
                 , ( app.config.wm_title ) ? app.config.wm_title
                                           : "0"
                 , ( app.config.wm_class ) ? app.config.wm_title
                                           : "0"
                 , app.config.window_w
                 , app.config.window_h
                 );
        return -7;        
    }
    if ( !app.config.window && (   app.config.wm_title
                                || app.config.wm_class
                                || app.config.window_x
                                || app.config.window_y
                                || app.config.window_w
                                || app.config.window_h
                                || app.config.wm_title
                                || app.config.wm_class
                               ))
    {
        LOGWARN ( "Application is windowless, but window configuration parameters were supplied." );
    }
    if ( app.config.user_input && !app.config.window )
    {
        LOGFATAL ( "User input enabled for windowless application, but user input subsystem requires a window." );
        return -8;        
    } 

    // Initialize the engine.
    if ( !engine_startup ( &app ) )
    {
        LOGFATAL ( "Failed to initialize the engine." );
        return 1; 
    }
    
    // Launch the engine.
    if ( !engine_run () )
    {
        LOGFATAL ( "Engine terminated with error." );
        return 2;
    }

    return 0;
}

#endif // MAIN_H
