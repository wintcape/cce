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
    u32         ply;

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
    fen_parse ( "r3k2r/p11pqpb1/bn2pnp1/2pPN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1" , &( *state ).board );
    ( *state ).ply = 0; 

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

    ( *state ).ply += 1;

    // Generate move options.
    moves_get ( &( *state ).moves
              , &( *state ).board
              , &( *state ).attacks
              );

    return true;
}

#include <stdio.h> /*  Temporary  */

void
chess_render
( void )
{
    if ( !state )
    {
        return;
    }
    
    string_format ( ( *state ).textbuffer
                  , "\n PLY: %d\n"
                  , ( *state ).ply
                  );
    platform_console_write ( ( *state ).textbuffer
                           , PLATFORM_COLOR_CHESS_INFO
                           );
    
    // Render the board.
    // board_render ( ( *state ).textbuffer , &( *state ).board );

    // Temporary.
    for ( u32 i = 0; i < ( *state ).moves.count; ++i )
    {
        const move_t move = ( *state ).moves.moves[ i ];
        if ( move_parse ( move , MOVE_FILTER_NONE , &( *state ).board ) )
        {
            string_format ( ( *state ).textbuffer
                          , "Move %s:"
                          , string_move ( ( *state ).textbuffer , move )
                          );
            platform_console_write ( ( *state ).textbuffer
                                   , PLATFORM_COLOR_CHESS_INFO
                                   );
            board_render ( ( *state ).textbuffer , &( *state ).board );
        }
        else
        {
            string_format ( ( *state ).textbuffer
                          , "Move %s skipped."
                          , string_move ( ( *state ).textbuffer , move )
                          );
            platform_console_write ( ( *state ).textbuffer
                                   , PLATFORM_COLOR_CHESS_INFO
                                   );
        }
        getchar ();
    }
}

