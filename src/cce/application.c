/**
 * @file application.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Implementation of the application header.
 * (see application.h for additional details)
 */
#include "cce/application.h"

#include "cce/input.h"
#include "cce/platform.h"

#include "core/clock.h"
#include "core/event.h"
#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"

#include "chess/chess.h"

// Defines textbuffer sizes.
#define CCE_RENDER_TEXTBUFFER_LENGTH    16384
#define CCE_COMMAND_TEXTBUFFER_LENGTH   8

// Type definition for game variant.
typedef enum
{
    CCE_GAME_PLAYER_VERSUS_PLAYER
,   CCE_GAME_PLAYER_VERSUS_ENGINE
,   CCE_GAME_ENGINE_VERSUS_ENGINE
,   CCE_GAME_TAG_COUNT
}
CCE_GAME_TAG;

// Type definition for game state.
typedef enum
{
    CCE_GAME_STATE_GAME_INIT
,   CCE_GAME_STATE_GAME_START
,   CCE_GAME_STATE_GAME_END
,   CCE_GAME_STATE_PROMPT_GAME_TYPE
,   CCE_GAME_STATE_PROMPT_COMMAND
,   CCE_GAME_STATE_EXECUTE_COMMAND
,   CCE_GAME_STATE_EXECUTE_MOVE_PLAYER
,   CCE_GAME_STATE_EXECUTE_MOVE_ENGINE
,   CCE_GAME_STATE_COUNT
}
CCE_GAME_STATE;

// Type definition for render tag.
typedef enum
{
    CCE_RENDER_NONE
,   CCE_RENDER_TITLE
,   CCE_RENDER_START
,   CCE_RENDER_END
,   CCE_RENDER_GAME_SELECTED
,   CCE_RENDER_PROMPT_GAME_TYPE
,   CCE_RENDER_PROMPT_COMMAND
,   CCE_RENDER_TAG_COUNT
}
CCE_RENDER_TAG;

// Type definition for internal application state.
typedef struct
{
    // Game state.
    CCE_GAME_TAG    game;
    CCE_GAME_STATE  state;
    CCE_RENDER_TAG  render;
    u64             update;
    u32             ioerr;

    // Chess.
    attacks_t       attacks;
    board_t         board;
    moves_t         moves;
    move_t          move;
    u32             ply;

    // Textbuffers.
    char            textbuffer[ CCE_RENDER_TEXTBUFFER_LENGTH ];
    char            cmd[ CCE_COMMAND_TEXTBUFFER_LENGTH ];

    // Benchmarking.
    clock_t         clock;
    f64             elapsed;
}
state_t;

// Global application state.
static cce_t* cce;

/**
 * @brief Primary implementation of cce_update and cce_render.
 * (see cce_update, cce_render).
 */
bool cce_game_init               ( void );
bool cce_game_start              ( void );
bool cce_game_end                ( void );
bool cce_prompt_game_type        ( void );
bool cce_prompt_command          ( void );
bool cce_execute_command         ( void );
bool cce_execute_move_player     ( void );
bool cce_execute_move_engine     ( void );
void cce_render_title            ( void );
void cce_render_start            ( void );
void cce_render_end              ( void );
void cce_render_game_selected    ( void );
void cce_render_prompt_game_type ( void );
void cce_render_prompt_command   ( void );
void cce_render_list_commands    ( void );
void cce_render_list_moves       ( void );
void cce_render_board            ( void );
void cce_render_buffer           ( void );

bool
cce_startup
(   application_t*  app
)
{
    // Allocate memory for application state, including internal game state.
    cce = memory_allocate ( sizeof ( cce_t ) + sizeof ( state_t )
                          , MEMORY_TAG_APPLICATION
                          );
    ( *app ).app_state = cce;
    ( *cce ).internal = ( ( void* ) cce ) + sizeof ( cce_t );

    // Initialize game state.
    
    state_t* state = ( *cce ).internal;
    
    ( *state ).update = 0;
    ( *state ).ioerr = 0;

    memory_clear ( ( *state ).textbuffer
                 , sizeof ( ( *state ).textbuffer )
                 );
    memory_clear ( ( *state ).cmd
                 , sizeof ( ( *state ).cmd )
                 );

    // Pregenerate attack tables.
    attacks_init ( &( *state ).attacks );

    ( *state ).state = CCE_GAME_STATE_GAME_INIT;
    return true;
}

void
cce_shutdown
(   application_t*  app
)
{
    if ( !cce )
    {
        return;
    }

    // Free memory used by application.
    memory_free ( cce
                , sizeof ( cce_t ) + sizeof ( state_t )
                , MEMORY_TAG_APPLICATION
                );
    cce = 0;
}

bool
cce_update
(   application_t*  app
,   f32             dt
)
{
    if ( !cce )
    {
        return false;
    }
    
    state_t* state = ( *cce ).internal;
    
    ( *state ).update += 1;
    
    switch ( ( *state ).state )
    {
        case CCE_GAME_STATE_GAME_INIT          : return cce_game_init ()           ;
        case CCE_GAME_STATE_GAME_START         : return cce_game_start ()          ;
        case CCE_GAME_STATE_GAME_END           : return cce_game_end ()            ;
        case CCE_GAME_STATE_PROMPT_GAME_TYPE   : return cce_prompt_game_type ()    ;
        case CCE_GAME_STATE_PROMPT_COMMAND     : return cce_prompt_command ()      ;
        case CCE_GAME_STATE_EXECUTE_COMMAND    : return cce_execute_command ()     ;
        case CCE_GAME_STATE_EXECUTE_MOVE_PLAYER: return cce_execute_move_player () ;
        case CCE_GAME_STATE_EXECUTE_MOVE_ENGINE: return cce_execute_move_engine () ;
        default                                : break                             ;
    }

    return true;
}

void
cce_render
(   application_t*  app
,   f32             dt
)
{
    if ( !cce )
    {
        return;
    }

    state_t* state = ( *cce ).internal; 
    const CCE_RENDER_TAG render = ( *state ).render;  
    
    if ( render == CCE_RENDER_NONE )
    {
        return;
    }

    if ( render == CCE_RENDER_PROMPT_COMMAND )
    {
        if ( ( *state ).ioerr )
        {
        // ---------- TEMPORARY ----------
        cce_render_buffer ();           // Refresh console by rendering newlines.
        // -------------------------------

            if ( ( *state ).ioerr > 3 )
            {
                platform_console_write ( "\n\n\tTrying to exit? Pass 'q' as the next move."
                                           "\n\tNeed help? Pass 'h' for help options."
                                           "\n"
                                       , CCE_COLOR_HINT
                                       );
            }
        }

        // Render movetext.
        if (   ( *state ).state == CCE_GAME_STATE_EXECUTE_MOVE_ENGINE
            || ( *state ).state == CCE_GAME_STATE_PROMPT_COMMAND
           )
        {
            const char* s_side;
            if ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE )
            {
                s_side = ( ( *state ).board.side != WHITE ) ? "WHITE (player)"
                                                            : "BLACK (engine)"
                                                            ;
            }
            else
            {
                s_side = ( ( *state ).board.side != WHITE ) ? "WHITE"
                                                            : "BLACK"
                                                            ;
            }

            if ( move_decode_capture ( ( *state ).move ) )
            {
                if ( move_decode_promotion ( ( *state ).move ) )
                {
                    string_format ( ( *state ).textbuffer
                                  , "\n\n\n\t%s: %s ON %s TO CAPTURE %s on %s. PROMOTED TO %s."
                                  , s_side
                                  , piecewchr ( move_decode_piece ( ( *state ).move ) )
                                  , string_square ( move_decode_src ( ( *state ).move ) )
                                  , piecewchr ( ( *state ).board.capture )
                                  , string_square ( move_decode_dst ( ( *state ).move ) )
                                  , piecewchr ( move_decode_promotion ( ( *state ).move ) )
                                  );
                }
                else
                {
                    string_format ( ( *state ).textbuffer
                                  , "\n\n\n\t%s: %s ON %s TO CAPTURE %s on %s."
                                  , s_side
                                  , piecewchr ( move_decode_piece ( ( *state ).move ) )
                                  , string_square ( move_decode_src ( ( *state ).move ) )
                                  , piecewchr ( ( *state ).board.capture )
                                  , string_square ( move_decode_dst ( ( *state ).move ) )
                                  );
                }
            }
            else
            {
                if ( move_decode_promotion ( ( *state ).move ) )
                {
                    string_format ( ( *state ).textbuffer
                                  , "\n\n\n\t%s: %s ON %s TO %s. PROMOTED TO %s."
                                  , s_side
                                  , piecewchr ( move_decode_piece ( ( *state ).move ) )
                                  , string_square ( move_decode_src ( ( *state ).move ) )
                                  , string_square ( move_decode_dst ( ( *state ).move ) )
                                  , piecewchr ( move_decode_promotion ( ( *state ).move ) )
                                  );
                }
                else
                {
                    string_format ( ( *state ).textbuffer
                                  , "\n\n\n\t%s: %s ON %s TO %s."
                                  , s_side
                                  , piecewchr ( move_decode_piece ( ( *state ).move ) )
                                  , string_square ( move_decode_src ( ( *state ).move ) )
                                  , string_square ( move_decode_dst ( ( *state ).move ) )
                                  );
                }
            }
            platform_console_write ( ( *state ).textbuffer , CCE_COLOR_PLUS );
        }

        // Render the board state.
        cce_render_board ();

        // Render the prompt.
        cce_render_prompt_command ();

        return;
    }
    
    // ---------- TEMPORARY ----------
    cce_render_buffer ();           // Refresh console by rendering newlines.
    // -------------------------------

    switch ( render )
    {
        case CCE_RENDER_START           : cce_render_start ()            ;break;
        case CCE_RENDER_TITLE           : cce_render_title ()            ;break;
        case CCE_RENDER_END             : cce_render_end ()              ;break;
        case CCE_RENDER_GAME_SELECTED   : cce_render_game_selected ()    ;break;
        case CCE_RENDER_PROMPT_GAME_TYPE: cce_render_prompt_game_type () ;break;
        default                         :                                 break;
    }
}

// ---------- TEMPORARY ----------
#include <stdio.h>              // TODO: Eliminate scanf calls for user input.
// -------------------------------

bool
cce_game_init
( void )
{
    state_t* state = ( *cce ).internal;

    ( *state ).render = CCE_RENDER_START;
    ( *state ).state = CCE_GAME_STATE_PROMPT_GAME_TYPE;
    return true;
}

bool
cce_game_start
( void )
{
    state_t* state = ( *cce ).internal;

    ( *state ).ioerr = 0;
    ( *state ).ply = 0;

    ( *state ).elapsed = 0;

    // Initialize chess board.
    ( *state ).board.history = 0;
    fen_parse ( FEN_START , &( *state ).board );
    
    // Populate move list.
    moves_get ( &( *state ).moves
              , &( *state ).board
              , &( *state ).attacks
              );

    if ( ( *state ).game == CCE_GAME_ENGINE_VERSUS_ENGINE )
    {
        ( *state ).render = CCE_RENDER_GAME_SELECTED;
        ( *state ).state = CCE_GAME_STATE_EXECUTE_MOVE_ENGINE;
    }
    else
    {
        ( *state ).render = CCE_RENDER_GAME_SELECTED;
        ( *state ).state = CCE_GAME_STATE_PROMPT_COMMAND;
    }

    return true;
}

bool
cce_game_end
( void )
{
    state_t* state = ( *cce ).internal;

    // Render ending splashtext.
    cce_render_end ();

    // Signal engine to shutdown the application.
    event_context_t ctx = {};
    event_fire ( EVENT_CODE_APPLICATION_QUIT , 0 , ctx );

    ( *state ).render = CCE_RENDER_NONE;
    ( *state ).state = CCE_GAME_STATE_COUNT;
    return true;
}

bool
cce_prompt_game_type
( void )
{
    state_t* state = ( *cce ).internal; 
    
    // Clear any previous user input.
    ( *state ).cmd[ 0 ] = 0;

    // Read new user input.
    if ( scanf ( "%8s" , ( *state ).cmd ) <= 0 )
    {
        LOGERROR ( "cce_prompt_game_type: Failed to read user input from stdin." );
        return false;
    }
    
    // Flush the input stream.
    int garbage;
    while ( ( garbage = getchar () ) != '\n' && garbage != EOF );
       
    // Sanitize input string.
    ( *state ).cmd[ sizeof ( ( *state ).cmd ) - 2 ] = 0;
    
    // Validate input string.
    if ( !(   string_length ( ( *state ).cmd ) == 1
           && (   ( *state ).cmd[ 0 ] == '1'
               || ( *state ).cmd[ 0 ] == '2'
               || ( *state ).cmd[ 0 ] == '3'
              )))
    {
        ( *state ).ioerr += 1;
        return true;
    }
    ( *state ).ioerr = 0;

    // Set the game type.
    ( *state ).game = chri ( ( *state ).cmd[ 0 ] ) - 1;

    ( *state ).render = CCE_RENDER_NONE;
    ( *state ).state = CCE_GAME_STATE_GAME_START;
    return true;
}

bool
cce_prompt_command
( void )
{
    state_t* state = ( *cce ).internal;

    ( *state ).render = CCE_RENDER_PROMPT_COMMAND;
    
    // Clear any previous user input.
    ( *state ).cmd[ 0 ] = 0;

    // Read new user input.
    if ( scanf ( "%8s" , ( *state ).cmd ) <= 0 )
    {
        LOGERROR ( "cce_prompt_game_type: Failed to read user input from stdin." );
        return false;
    }
    
    // Flush the input stream.
    int garbage;
    while ( ( garbage = getchar () ) != '\n' && garbage != EOF );
       
    // Sanitize input string.
    ( *state ).cmd[ sizeof ( ( *state ).cmd ) - 2 ] = 0;
    
    // Validate input string.
    
    bool valid;
    const u8 len = string_length ( ( *state ).cmd );

    // Validate if input is a command key signal.
    if ( len == 1 )
    {
        ( *state ).cmd[ 0 ] = to_lowercase ( ( *state ).cmd[ 0 ] );
        switch ( ( *state ).cmd[ 0 ] )
        {
            case CCE_KEY_SIGNAL_COMMAND_HELP              :
            case CCE_KEY_SIGNAL_COMMAND_LIST_MOVES        :
            case CCE_KEY_SIGNAL_COMMAND_CHOOSE_RANDOM_MOVE:
            case CCE_KEY_SIGNAL_COMMAND_QUIT              : valid = true  ;break;
            default                                       : valid = false ;break;
        }
        
        if ( !valid )
        {
            ( *state ).ioerr += 1;
            return true;
        }
        ( *state ).ioerr = 0;

        ( *state ).render = CCE_RENDER_NONE;
        ( *state ).state = CCE_GAME_STATE_EXECUTE_COMMAND;
        return true;
    }

    // Validate move.

    // Sanitize.
    for ( u8 i = 0; i < MOVE_STRING_LENGTH; ++i )
    {
        ( *state ).cmd[ i ] = to_uppercase ( ( *state ).cmd[ i ] );
    }

    // Validate input format.
    if ( len < MOVE_STRING_LENGTH - 1 || len > MOVE_STRING_LENGTH )
    {
        ( *state ).ioerr += 1;
        return true;
    }
    else if (   ( *state ).cmd[ 0 ] < 'A'
             || ( *state ).cmd[ 0 ] > 'H'
             || ( *state ).cmd[ 1 ] < '1'
             || ( *state ).cmd[ 1 ] > '8'
             || ( *state ).cmd[ 2 ] < 'A'
             || ( *state ).cmd[ 2 ] > 'H'
             || ( *state ).cmd[ 3 ] < '1'
             || ( *state ).cmd[ 3 ] > '8'
            )
    {
        ( *state ).ioerr += 1;
        return true;
    }
    else if ( len == MOVE_STRING_LENGTH )
    {
        if (   ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] != 'N'
            && ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] != 'B'
            && ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] != 'R'
            && ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] != 'Q'
           )
        {
            ( *state ).ioerr += 1;
            return true;
        }
    }
    else
    {
        // Sanitize for string comparison.
        ( *state ).cmd[ MOVE_STRING_LENGTH - 1 ] = ' ';
        ( *state ).cmd[ MOVE_STRING_LENGTH ] = 0;
    }
    ( *state ).ioerr += 0;
    
    // Check if present in list of valid move options.
    valid = false;
    for ( u32 i = 0; i < ( *state ).moves.count; ++i )
    {
        if ( string_equal ( ( *state ).cmd
                          , string_move ( ( *state ).textbuffer
                                        , ( *state ).moves.moves[ i ]
                                        )))
        {
            ( *state ).move = ( *state ).moves.moves[ i ];
            valid = true;
            break;
        }
    }
    
    if ( !valid )
    {
        ( *state ).ioerr += 1;
            return true;
    }
    ( *state ).ioerr = 0;

    ( *state ).render = CCE_RENDER_NONE;
    ( *state ).state = CCE_GAME_STATE_EXECUTE_MOVE_PLAYER;
    return true;
}

bool
cce_execute_command
( void )
{
    state_t* state = ( *cce ).internal; 
    
    // Parse command.
    switch ( ( *state ).cmd[ 0 ] )
    {
            case CCE_KEY_SIGNAL_COMMAND_HELP:
            {
                cce_render_buffer ();
                cce_render_list_commands ();
            }
            break;
            
            case CCE_KEY_SIGNAL_COMMAND_LIST_MOVES:
            {
                cce_render_buffer ();
                cce_render_list_moves ();
            }
            break;

            case CCE_KEY_SIGNAL_COMMAND_CHOOSE_RANDOM_MOVE:
            {
                ( *state ).move = ( *state ).moves.moves[ random2 ( 0 , ( *state ).moves.count - 1 ) ];
                ( *state ).render = CCE_RENDER_NONE;
                ( *state ).state = CCE_GAME_STATE_EXECUTE_MOVE_PLAYER;
                return true;
            }
            
            case CCE_KEY_SIGNAL_COMMAND_QUIT:
            {
                ( *state ).render = CCE_RENDER_NONE;
                ( *state ).state = CCE_GAME_STATE_GAME_END;
                return true;
            }
            
            default:
            {}
            break;
    }
    
    ( *state ).render = CCE_RENDER_PROMPT_COMMAND;
    ( *state ).state = CCE_GAME_STATE_PROMPT_COMMAND;
    return true;
}

bool
cce_execute_move_player
( void )
{
    state_t* state = ( *cce ).internal;

    // Parse the move.
    if ( !move_parse ( ( *state ).move
                     , MOVE_FILTER_NONE
                     , &( *state ).attacks
                     , &( *state ).board
                     ))
    {
        const char* s_side;
        if ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE )
        {
            s_side = "WHITE (player)";
        }
        else
        {
            s_side = ( ( *state ).board.side != WHITE ) ? "WHITE" : "BLACK";
        }

        string_format ( ( *state ).textbuffer
                      , "\n\n\t%s CHECK: %s\n"
                      , s_side
                      , string_move ( ( *state ).textbuffer
                                    , ( *state ).move
                                    ));
        platform_console_write ( ( *state ).textbuffer , CCE_COLOR_PLUS );
        
        ( *state ).render = CCE_RENDER_NONE;
        ( *state ).state = CCE_GAME_STATE_GAME_END;
        return true;
    }

    ( *state ).ply += 1;

    // Populate move list.
    moves_get ( &( *state ).moves
              , &( *state ).board
              , &( *state ).attacks
              );
    
    ( *state ).render = CCE_RENDER_PROMPT_COMMAND;
    ( *state ).state = ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? CCE_GAME_STATE_EXECUTE_MOVE_ENGINE
                                                                            : CCE_GAME_STATE_PROMPT_COMMAND
                                                                            ;
    return true;
}

bool
cce_execute_move_engine
( void )
{
    state_t* state = ( *cce ).internal;
    
    platform_console_write ( "Calculating best move. . .  " , CCE_COLOR_HINT );

    // Start clock.
    clock_start ( &( *state ).clock );
    
    // Calculate best move.     ( TODO: Make this not just a random move LOL. . . )
    ( *state ).move = ( *state ).moves.moves[ random2 ( 0 , ( *state ).moves.count - 1 ) ];

    // Stop clock.
    clock_update ( &( *state ).clock );
    ( *state ).elapsed += ( *state ).clock.elapsed;

    string_move ( ( *state ).textbuffer , ( *state ).move );
    if ( ( *state ).textbuffer[ 4 ] == ' ' )
    {
        ( *state ).textbuffer[ 4 ] = 0;
    }
    platform_console_write ( ( *state ).textbuffer , CCE_COLOR_HIGHLIGHT );

    // Parse the move.
    if ( !move_parse ( ( *state ).move
                     , MOVE_FILTER_NONE
                     , &( *state ).attacks
                     , &( *state ).board
                     ))
    {
        const char* s_side;
        if ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE )
        {
            s_side = "BLACK (engine)";
        }
        else
        {
            s_side = ( ( *state ).board.side != WHITE ) ? "WHITE" : "BLACK";
        }

        string_format ( ( *state ).textbuffer
                      , "\n\n\t%s CHECK: %s\n"
                      , s_side
                      , string_move ( ( *state ).textbuffer
                                    , ( *state ).move
                                    ));
        platform_console_write ( ( *state ).textbuffer , CCE_COLOR_PLUS );

        string_format ( ( *state ).textbuffer
                      , "\n\tEngine calculations took %f seconds."
                      , ( *state ).elapsed
                      );
        platform_console_write ( ( *state ).textbuffer , CCE_COLOR_HINT );
        
        ( *state ).render = CCE_RENDER_NONE;
        ( *state ).state = CCE_GAME_STATE_GAME_END;
        return true;
    }
    
    string_format ( ( *state ).textbuffer
                  , "\n\tTook %f seconds."
                  , ( *state ).clock.elapsed
                  );
    platform_console_write ( ( *state ).textbuffer , CCE_COLOR_HINT );

    ( *state ).ply += 1;

    // Populate move list.
    moves_get ( &( *state ).moves
              , &( *state ).board
              , &( *state ).attacks
              );
    
    ( *state ).render = CCE_RENDER_PROMPT_COMMAND;
    ( *state ).state = ( ( *state ).game == CCE_GAME_ENGINE_VERSUS_ENGINE ) ? CCE_GAME_STATE_EXECUTE_MOVE_ENGINE
                                                                            : CCE_GAME_STATE_PROMPT_COMMAND
                                                                            ;
    return true;
}

void
cce_render_title
( void )
{
    state_t* state = ( *cce ).internal;
    
    string_format ( ( *state ).textbuffer
                  , "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                    "\n\t~                                                 ~"
                    "\n\t~                CCE CHESS ENGINE.                ~"
                    "\n\t~                (   v. %i.%i.%i   )                 ~"
                    "\n\t~           created by Matthew Weissel.           ~"
                    "\n\t~                                                 ~"
                    "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                    "\n\n\n"
                  , VERSION_MAJOR , VERSION_MINOR , VERSION_PATCH
                  );
    platform_console_write ( ( *state ).textbuffer , CCE_COLOR_INFO );
}

void
cce_render_start
( void )
{
    // Render splashtext.
    cce_render_title ();

    // Peek first prompt.
    cce_render_prompt_game_type ();
}

void
cce_render_end
( void )
{
    platform_console_write ( "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                             "\n\t~                                                 ~"
                             "\n\t~                                                 ~"
                             "\n\t~               Thanks for playing!               ~"
                             "\n\t~                                                 ~"
                             "\n\t~                                                 ~"
                             "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                             "\n\n\n"
                           , CCE_COLOR_INFO
                           );
}

void
cce_render_game_selected
( void )
{
    state_t* state = ( *cce ).internal;
    
    const char* s_game;
    const CCE_GAME_TAG game = ( *state ).game;
    switch ( game )
    {
        case CCE_GAME_PLAYER_VERSUS_PLAYER: s_game = "PLAYER VERSUS PLAYER" ;break;
        case CCE_GAME_PLAYER_VERSUS_ENGINE: s_game = "PLAYER VERSUS ENGINE" ;break;
        case CCE_GAME_ENGINE_VERSUS_ENGINE: s_game = "ENGINE VERSUS ENGINE" ;break;
        default                           : s_game = ""                     ;break;
    }
    
    // Render splashtext.
    cce_render_title ();

    // Render info.
    string_format ( ( *state ).textbuffer
                  , "\n\tStarting new game of %s. . ."
                    "\n"
                    "\n"
                    "\n"
                  , s_game
                  );
    platform_console_write ( ( *state ).textbuffer
                           , CCE_COLOR_PLUS
                           );
    
    // Render starting board state.
    cce_render_board ();

    // Render prompt.
    cce_render_prompt_command ();
}

void
cce_render_prompt_game_type
( void )
{
    state_t* state = ( *cce ).internal;
    if ( !( *state ).ioerr )
    {
        platform_console_write ( "\n"
                                 "\n\t                CHOOSE A GAME STYLE:               "
                                 "\n\t                                                   "
                                 "\n\t            [ 1 ]  PLAYER VERSUS PLAYER.           "
                                 "\n\t            [ 2 ]  PLAYER VERSUS ENGINE.           "
                                 "\n\t            [ 3 ]  ENGINE VERSUS ENGINE.           "
                                 "\n\t                                                   "
                                 "\n\tCHOICE:       "
                               , CCE_COLOR_INFO
                               );
    }
    else
    {
        platform_console_write ( "\n\tPLEASE CHOOSE FROM THE OPTIONS PROVIDED."
                               , CCE_COLOR_ALERT
                               );
        platform_console_write ( "\n\n"
                                 "\n\t                CHOOSE A GAME STYLE:               "
                                 "\n\t                                                   "
                                 "\n\t            [ 1 ]  PLAYER VERSUS PLAYER.           "
                                 "\n\t            [ 2 ]  PLAYER VERSUS ENGINE.           "
                                 "\n\t            [ 3 ]  ENGINE VERSUS ENGINE.           "
                                 "\n\t                                                   "
                               , CCE_COLOR_INFO
                               );
        platform_console_write ( "\n\tCHOICE:       "
                               , CCE_COLOR_INFO
                               );
    }
}

void
cce_render_prompt_command
( void )
{
    state_t* state = ( *cce ).internal;

    if ( ( *state ).ioerr )
    {
        platform_console_write ( "\n\t" , CCE_COLOR_DEFAULT );
        platform_console_write ( ( *state ).cmd , CCE_COLOR_HIGHLIGHT );
        platform_console_write ( " is not a known command or valid chess move."
                               , CCE_COLOR_ALERT
                               );
        platform_console_write ( "\n\tISSUE MOVE OR COMMAND:  "
                               , CCE_COLOR_INFO
                               );
    }

    else
    {
        platform_console_write ( "\n\n\tISSUE MOVE OR COMMAND:  "
                               , CCE_COLOR_INFO
                               );
    }
}

void
cce_render_list_commands
( void )
{
    state_t* state = ( *cce ).internal;

    string_format ( ( *state ).textbuffer
                  , "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                    "\n\t                                                   "
                    "\n\t         =-=-=- AVAILABLE COMMANDS -=-=-=          "
                    "\n\t                                                   "
                    "\n\t  %c :      List available commands.               "
                    "\n\t  %c :      List available moves.                  "
                    "\n\t  %c :      Choose random valid move.              "
                    "\n                                                     "
                    "\n\t  %c :      Quit the application.                  "
                    "\n\t                                                   "
                    "\n\t         =-=-=-                    -=-=-=          "
                  , CCE_KEY_SIGNAL_COMMAND_HELP
                  , CCE_KEY_SIGNAL_COMMAND_LIST_MOVES
                  , CCE_KEY_SIGNAL_COMMAND_CHOOSE_RANDOM_MOVE
                  , CCE_KEY_SIGNAL_COMMAND_QUIT
                  );
    platform_console_write ( ( *state ).textbuffer
                           , CCE_COLOR_HINT
                           );
}

void
cce_render_list_moves
( void )
{
    state_t* state = ( *cce ).internal;
    platform_console_write ( string_moves ( ( *state ).textbuffer
                                          , &( *state ).moves
                                          )
                           , CCE_COLOR_INFO
                           );
}

void
cce_render_board
( void )
{
    state_t* state = ( *cce ).internal;
    board_render ( ( *state ).textbuffer
                 , &( *state ).board
                 , CCE_COLOR_INFO
                 );
}

void
cce_render_buffer
( void )
{
    platform_console_write ( "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                           , CCE_COLOR_DEFAULT
                           );
}
