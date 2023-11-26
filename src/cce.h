/**
 * @file cce.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Test application: CCE.
 */
#ifndef CCE_H
#define CCE_H

#include "common.h"

#include "core/application.h"

// Type definition for the application.
typedef struct
{
    f32     dt;
    
    u64     chess_engine_memory_requirement;
    void*   chess_engine_state;
}
cce_t;

/**
 * @brief Application startup function.
 * @param app The cce instance.
 * @return false on error, true otherwise.
 */
bool
cce_startup
(   application_t* app
);

/**
 * @brief Application shutdown function.
 * @param app The cce instance.
 */
void
cce_shutdown
(   application_t* app
);

/**
 * @brief Application update function.
 * @param app The cce instance.
 * @param dt Time elapsed since previous invocation.
 * @return false on error, true otherwise.
 */
bool
cce_update
(   application_t*  app
,   f32             dt
);

/**
 * @brief Application render function.
 * @param app The cce instance.
 * @param dt Time elapsed since previous invocation.
 */
void
cce_render
(   application_t*  app
,   f32             dt
);

#endif  // CCE_H
