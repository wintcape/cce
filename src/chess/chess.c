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
    fen_parse ( FEN_START , &( *state ).board );

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
    fen_parse ( FEN_TRICKY , &( *state ).board );
    perft ( &( *state ).board , 2 , &( *state ).attacks );

    moves_get ( &( *state ).moves
              , &( *state ).board
              , &( *state ).attacks
              );

    return true;
}

#include <stdio.h>
void
chess_render
( void )
{
    if ( !state )
    {
        return;
    }
    return;
    
    // Render the board.
    //board_render ( ( *state ).textbuffer , &( *state ).board );

    /*  Temporary.  */
    
    const MOVE_FILTER filter = MOVE_FILTER_ONLY_CAPTURE;
    for ( u32 i = 0; i < ( *state ).moves.count; ++i )
    {
        const move_t move = ( *state ).moves.moves[ i ];

        board_t board_prev;
        memory_copy ( &board_prev , &( *state ).board , sizeof ( board_t ) );

        if ( move_parse ( move , filter , &( *state ).attacks  , &( *state ).board) )
        {
            platform_console_write ( " Initial board state:" , PLATFORM_COLOR_CHESS_INFO );        
            board_render ( ( *state ).textbuffer , &board_prev );
            string_format ( ( *state ).textbuffer , " MOVE: %s applied.", string_move ( ( *state ).textbuffer , move ) );
            platform_console_write ( ( *state ).textbuffer , PLATFORM_COLOR_CHESS_INFO );
            board_render ( ( *state ).textbuffer , &( *state ).board ); 
            getchar ();
            memory_copy (&( *state ).board , &board_prev , sizeof ( board_t ) );
        }
        else
        {
            string_format ( ( *state ).textbuffer , " MOVE: %s filtered.", string_move ( ( *state ).textbuffer , move ) );
            platform_console_write ( ( *state ).textbuffer , PLATFORM_COLOR_CHESS_INFO );
        }
        getchar ();
    }
}
