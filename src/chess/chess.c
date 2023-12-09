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
#include "chess/input.h"
#include "chess/move.h"
#include "chess/platform.h"
#include "chess/string.h"

#include "core/event.h"
#include "core/logger.h"

#include "math/math.h"

#include <stdio.h> // Temporary.

// Defaults.
#define CHESS_INPUT_TEXTBUFFER_LENGTH 8

// Type definition for chess engine subsystem state.
typedef struct
{
    // Pregenerated attack tables.
    attacks_t   attacks;

    // Game state.
    board_t     board;
    moves_t     moves;
    i64         move;
    u32         ply;
    
    // Internal.
    u64         update;
    char        textbuffer[ CHESS_RENDER_TEXTBUFFER_LENGTH ];
    char        input[ CHESS_INPUT_TEXTBUFFER_LENGTH ];
    bool        render;
}
state_t;

// Global subsystem state.
static state_t* state;

/**
 * @brief Subroutine to process user input.
 */
bool
chess_handle_console_input
( void );

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

    // Initialize internal state.
    ( *state ).update = 0;
    memory_clear ( ( *state ).textbuffer
                 , sizeof ( ( *state ).textbuffer )
                 );
    memory_clear ( ( *state ).input
                 , sizeof ( ( *state ).input )
                 );

    // Pregenerate attack tables.
    attacks_init ( &( *state ).attacks );

    // Initialize game state.
    fen_parse ( FEN_START , &( *state ).board );
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

    /**
     * Do nothing on the first update, so the starting board can render once
     * first.
     */
    if ( !( *state ).update )
    {
        ( *state ).update += 1;
        ( *state ).ply += 1;
        ( *state ).render = true;
        return true;
    }
    ( *state ).update += 1;
    
    // Compute the moves for the current board state.
    moves_get ( &( *state ).moves
              , &( *state ).board
              , &( *state ).attacks
              );
    
    ( *state ).move = -1;

    // Player versus engine.
    if ( ( *state ).board.side == WHITE )
    {
        // Handle user input.
        if ( !chess_handle_console_input () )
        {
            return false;
        }

        // No move to be performed.
        if ( ( *state ).move == -1 )
        {
            return true;
        }

    }
    else
    {
        // Choose a random move to perform.
        ( *state ).move = ( *state ).moves.moves[ random2 ( 0 , ( *state ).moves.count - 1 ) ];
    }
    
    // Perform the move.
    if ( !move_parse ( ( *state ).move
                     , MOVE_FILTER_NONE
                     , &( *state ).attacks
                     , &( *state ).board
                     ))
    {   
        LOGINFO ( "Check: %s."
                , string_move ( ( *state ).textbuffer
                              , ( *state ).move
                              ));
        event_context_t ctx = {};
        event_fire ( EVENT_CODE_APPLICATION_QUIT , 0 , ctx );
        return true;
    }

    ( *state ).ply += 1;
    
    string_format ( ( *state ).textbuffer
                  , "\n\n %s MOVED:  "
                  , ( ( *state ).board.side != WHITE ) ? "WHITE (player)"
                                                       : "BLACK (engine)"
                  );
    platform_console_write ( ( *state ).textbuffer , LOG_ERROR );
    string_move ( ( *state ).textbuffer , ( *state ).move );
    if ( ( *state ).textbuffer[ 4 ] == ' ' )
    {
        ( *state ).textbuffer[ 4 ] = 0;
    }

    platform_console_write ( ( *state ).textbuffer , LOG_FATAL );

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

    if ( !( *state ).render )
    {
        return;
    }

    string_format ( ( *state ).textbuffer
                  , "\n\n PLY #%u:"
                  , ( *state ).ply
                  );
    platform_console_write ( ( *state ).textbuffer
                           , PLATFORM_COLOR_CHESS_INFO
                           );

    // Render the board.
    board_render ( ( *state ).textbuffer , &( *state ).board );
}

bool
chess_handle_console_input
( void )
{
    u32 err = false;
    u8 len;
    
    do
    {
        // Clear any previous user input.
        ( *state ).input[ 0 ] = 0;
    
        if ( err && !( err % 3 ) )
        {
            platform_console_write ( "\n\n\t\tTrying to exit? Pass 'q' as the next move."
                                     "\n\t\tNeed help? Pass 'h' for help options."
                                     "\n"
                                   , LOG_LEVEL_COUNT+1
                                   );
        }

        platform_console_write ( "\n\t\tENTER MOVE:  "
                               , PLATFORM_COLOR_CHESS_INFO
                               );
   
        // Get new user input.
        if ( scanf ( "%8s" , ( *state ).input ) <= 0 )
        {
            LOGERROR ( "chess_handle_console_input: Failed to read user input from stdin." );
            return false;
        }

        // Flush input stream.
        int c;
        while ( ( c = getchar () ) != '\n' && c != EOF );
       
        // Sanitize input string.
        ( *state ).input[ sizeof ( ( *state ).input ) - 2 ] = 0;
        len = string_length ( ( *state ).input ); 
        for ( u8 i = 0; i < len; ++i )
        {
            ( *state ).input[ i ] = to_uppercase ( ( *state ).input[ i ] );
        }
    
        // Parse special commands.
        if ( len == 1 )
        {
            switch ( to_lowercase ( ( *state ).input[ 0 ] ) )
            {
                case KEY_SIGNAL_HELP:
                {
                    string_format ( ( *state ).textbuffer
                                  , "\n\t\t=-=-=-=-=-=-=-=-=-=-=-=-= HELP =-=-=-=-=-=-=-=-=-=-=-=-="
                                    "\n\t\t  %c :    List available commands."
                                    "\n\t\t  %c :    Show the board."
                                    "\n\t\t  %c :    List available moves."
                                    "\n\t\t  %c :    Choose random valid move."
                                    "\n\t\t  %c :    Quit the application.\n"
                                  , KEY_SIGNAL_HELP
                                  , KEY_SIGNAL_SHOW_BOARD
                                  , KEY_SIGNAL_LIST_MOVES
                                  , KEY_SIGNAL_CHOOSE_RANDOM_MOVE
                                  , KEY_SIGNAL_QUIT
                                  );
                    platform_console_write ( ( *state ).textbuffer
                                           , LOG_LEVEL_COUNT+1
                                           );
                    
                    ( *state ).render = false;
                    return true;
                }

                case KEY_SIGNAL_SHOW_BOARD:
                {
                    ( *state ).render = true;
                    return true;
                }

                case KEY_SIGNAL_LIST_MOVES:
                {
                    string_format ( ( *state ).textbuffer
                                  , "\n\n%s"
                                  , string_moves ( ( *state ).textbuffer
                                                 , &( *state ).moves
                                                 )
                                  , PLATFORM_COLOR_CHESS_INFO
                                  );
                    platform_console_write ( ( *state ).textbuffer
                                           , PLATFORM_COLOR_CHESS_INFO
                                           );
                    
                    ( *state ).render = false;
                    return true;
                }

                case KEY_SIGNAL_CHOOSE_RANDOM_MOVE:
                {
                    ( *state ).move = ( *state ).moves.moves[ random2 ( 0 , ( *state ).moves.count - 1 ) ];
                    ( *state ).render = true;
                    return true;
                }

                case KEY_SIGNAL_QUIT:
                {
                    event_context_t ctx = {};
                    event_fire ( EVENT_CODE_APPLICATION_QUIT , 0 , ctx );
                    
                    ( *state ).render = false;
                    return true;
                }

                default:
                {}
                break;
            }
        }

        // Validate input string.
        if ( len != 4 && len != 5 )
        {
            if ( len )
            {
                platform_console_write ( "\t\tINVALID MOVE ENTERED:  "
                                       , LOG_ERROR
                                       );
                if ( len > 5 )
                {
                    ( *state ).input[ 5 ] = 0;
                    string_format ( ( *state ).textbuffer
                                  , "%s%s"
                                  , ( *state ).input
                                  , ".."
                                  );
                    platform_console_write ( ( *state ).textbuffer
                                           , LOG_FATAL
                                           );
                }
                else
                {
                    platform_console_write ( ( *state ).input
                                           , LOG_FATAL
                                           );
                }
            }
            else
            {
                platform_console_write ( "\t\tNO MOVE ENTERED."
                                       , LOG_ERROR
                                       );
            }
            
            err += 1;
        }
        else
        {
            if ( len == 4 )
            {
                ( *state ).input[ 4 ] = ' ';
            }

            for ( u32 i = 0; i < ( *state ).moves.count; ++i )
            {
                if ( string_equal ( ( *state ).input
                                  , string_move ( ( *state ).textbuffer
                                                , ( *state ).moves.moves[ i ]
                                                )))
                {
                    ( *state ).move = ( *state ).moves.moves[ i ];
                    break;
                }
            }

            ( *state ).input[ len ] = 0;

            if ( ( *state ).move >= 0 )
            {
                err = false;
            }
            else
            {
    
                platform_console_write ( "\t\tINVALID MOVE ENTERED:  "
                                       , LOG_ERROR
                                       );
                platform_console_write ( ( *state ).input
                                       , LOG_FATAL
                                       );
                err += 1;
            }
            
        }
    }
    while ( err );

    ( *state ).render = true;
    return true;
}
