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
    if ( !app.startup || !app.shutdown || !app.update )
    {
        LOGFATAL ( "User application data structure is ill-defined." );
        return -2;        
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
