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
    fen_parse ( "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1" , &( *state ).board );

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

    // Generate move options.
    moves_get ( &( *state ).moves
              , &( *state ).board
              , &( *state ).attacks
              );

    return true;
}

#include <stdio.h> /*  Temporary.  */

void
chess_render
( void )
{
    if ( !state )
    {
        return;
    }
    
    // Render the board.
    // board_render ( ( *state ).textbuffer , &( *state ).board );

    /*  Temporary.  */

    board_t board;

    platform_console_write ( "\n================================================================================\n"
                           , PLATFORM_COLOR_CHESS_INFO
                           );

    for ( u32 i = 0; i < ( *state ).moves.count; ++i )
    {
        const move_t move = ( *state ).moves.moves[ i ];
        
        platform_console_write ( " BOARD INITIAL STATE: \n"
                               , PLATFORM_COLOR_CHESS_INFO
                               );
        board_render ( ( *state ).textbuffer , &( *state ).board );
        
        string_format ( ( *state ).textbuffer
                      , " BOARD AFTER MOVE %s: "
                      , string_move ( ( *state ).textbuffer , move )
                      );
            platform_console_write ( ( *state ).textbuffer
                                   , PLATFORM_COLOR_CHESS_INFO
                                   );

        if ( move_parse ( move
                        , MOVE_FILTER_NONE
                        , memory_copy ( &board
                                      , &( *state ).board
                                      , sizeof ( board_t )
                                      )))
        {
            platform_console_write ( "\n" , PLATFORM_COLOR_CHESS_INFO );
            board_render ( ( *state ).textbuffer , &board );
        }
        else
        {
            platform_console_write ( "No change applied (move filtered)."
                                   , PLATFORM_COLOR_CHESS_INFO
                                   );
        }

        platform_console_write ( "\n================================================================================\n"
                               , PLATFORM_COLOR_CHESS_INFO
                               );
        getchar ();
    }
}

