/**
 * @file application.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Interface for a user-implemented application
 * that runs on the engine (see also, engine.h).
 */
#ifndef APPLICATION_H
#define APPLICATION_H

#include "common.h"

// Type definition for a container to hold application configuration parameters.
typedef struct
{
    u64     memory_requirement;
}
config_t;

// Type definition for the application state interface.
typedef struct application_t
{
    config_t    config;     // Application configuration parameters.
    void*       internal;   // Internal state (managed by engine).
    void*       app_state;  // User application state.

    bool ( *startup )
         ( struct application_t*    app
         );
    
    void ( *shutdown )
         ( struct application_t*    app
         );

    bool ( *update )
         ( struct application_t*    app
         , f32                      dt
         );
    
    void ( *render )
         ( struct application_t*    app
         , f32                      dt
         );
}
application_t;

/**
 * @brief To be implemented per user application.
 * @param app The application state to mutate.
 * @return false on error, true otherwise.
 */
extern
bool
application
(   application_t* app
);

#endif  // APPLICATION_H
