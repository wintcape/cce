/**
 * @file main.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Definition for a test application that runs on the chess engine.
 * (see core/application.h for additional details)
 */
#include "main.h"           // Engine backend.
#include "application.h"    // Test application (CCE).

bool
application
(   application_t*  app
)
{
    ( *app ).config = ( config_t ){ .memory_requirement = MEBIBYTES ( 64 )
                                  , .window             = false
                                  , .user_input         = false
                                  };
    
    ( *app ).startup = cce_startup;
    ( *app ).shutdown = cce_shutdown;
    ( *app ).update = cce_update;
    ( *app ).render = cce_render;
    ( *app ).on_resize = 0;

    ( *app ).internal = 0;
    ( *app ).app_state = 0;
    
    return true;
}
