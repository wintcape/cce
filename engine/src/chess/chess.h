/**
 * @file chess.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Chess engine subsystem.
 */
#ifndef CHESS_H
#define CHESS_H

#include "chess/bitboard.h"
#include "chess/piece.h"
#include "chess/square.h"

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
 * @brief Temporary test function. 
 * TODO: Don't make this exposed.
 */
void
chess_run_tests
( void );

#endif  // CHESS_H
