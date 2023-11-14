/**
 * @file main.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Definition for a test application that runs on the chess engine.
 * (see core/application.h for additional details)
 */
#include "main.h"   // Engine backend.
#include "cce.h"   // Test application (CCE).

bool
application
(   application_t*  app
)
{
    ( *app ).config.memory_requirement = MEBIBYTES ( 64 );
    
    ( *app ).startup = cce_startup;
    ( *app ).shutdown = cce_shutdown;
    ( *app ).update = cce_update;

    ( *app ).internal = 0;
    ( *app ).app_state = 0;
    
    return true;
}
