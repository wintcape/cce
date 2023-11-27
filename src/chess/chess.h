/**
 * @file chess.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Chess engine subsystem.
 */
#ifndef CHESS_H
#define CHESS_H

#include "chess/common.h"

/**
 * @brief Initializes the chess engine subsystem. Call once to read the memory
 * requirement. Call again passing in a state pointer.
 * @param memory_requirement Output buffer to read memory requirement.
 * @param state Pass 0 to read memory requirement. Otherwise, pass a buffer.
 * @return false on error, true otherwise.
 */
bool
chess_startup
(   u64*    memory_requirement
,   void*   state
);

/**
 * @brief Terminates the chess engine subsystem.
 * @param state .
 */
void
chess_shutdown
(   void* state
);

/**
 * @brief Updates the chess subsystem.
 * @return false on error, true otherwise.
 */
bool
chess_update
( void );

/**
 * @brief Renders the current chess board to the terminal using the logger
 * subsystem.
 */
void
chess_render
( void );

#endif  // CHESS_H
