/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file chess.c
 * @brief Implementation of the chess header.
 * (see chess.h for additional details)
 */
#include "chess/chess.h"

#include "chess/attack.h"
#include "chess/board.h"
#include "chess/fen.h"
#include "chess/move.h"
#include "chess/perft.h"
#include "chess/platform.h"
#include "chess/string.h"

#include "core/logger.h"

// Type definition for chess engine subsystem state.
typedef struct
{
    // Pregenerated attack tables.
    attacks_t   attacks;

    // Game state.
    board_t     board;

    // Pregenerated move options.
    moves_t     moves;
    
    // Render textbuffer.
    char        textbuffer[ CHESS_RENDER_TEXTBUFFER_LENGTH ];

}
state_t;

// Global subsystem state.
static state_t* state;

bool
chess_startup
(   u64*    memory_requirement
,   void*   state_
)
{
    *memory_requirement = sizeof ( state_t );

    if ( !state_ )
    {
        return true;
    }

    state = state_;

    // Pregenerate attack tables.
    attacks_init ( &( *state ).attacks );

    // Initialize game state.
    fen_parse ( FEN_TRICKY , &( *state ).board );

    return true;
}

void
chess_shutdown
(   void* state_
)
{
    if ( !state )
    {
        return;
    }

    state = 0;
}

bool
chess_update
( void )
{
    if ( !state )
    {
        return false;
    }
    
    /*  Temporary.  */
    perft ( &( *state ).board , &( *state ).attacks , 2 );

    return true;
}

void
chess_render
( void )
{
    if ( !state )
    {
        return;
    }
    
    // Render the board.
    board_render ( ( *state ).textbuffer , &( *state ).board );
}

