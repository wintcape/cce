/**
 * @file engine.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Main entry point for the engine. Launches the engine from the
 * main program and then runs the user-implemented application.
 * (see also application.h)
 */
#ifndef ENGINE_H
#define ENGINE_H

#include "core/application.h"
#include "platform/platform.h"

/**
 * @brief Initializes the engine.
 * @param app The application to be run on the engine.
 * @return false on error, true otherwise.
 */
bool
engine_startup
(   application_t* app
);

/**
 * @brief Runs the engine application.
 * @return false on error, true otherwise.
 */
bool
engine_run
( void );

#endif  // ENGINE_H
