/**
 * @file application.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Implementation of the application header.
 * (see application.h for additional details)
 */
#include "cce/application.h"

#include "core/clock.h"
#include "core/event.h"
#include "core/input.h"
#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"

#include "math/math.h"

#include "platform/platform.h"
#include "platform/filesystem.h"

#include "chess/chess.h"

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
,   CCE_GAME_STATE_DEBUG

,   CCE_GAME_STATE_COUNT
}
CCE_GAME_STATE;

// Defines the various endgame conditions.
typedef enum
{
    CCE_GAME_END_CHECKMATE
,   CCE_GAME_END_STALEMATE
,   CCE_GAME_END_DRAW

,   CCE_GAME_END_TAG_COUNT
}
CCE_GAME_END_TAG;

// Type definition for render tag.
typedef enum
{
    CCE_RENDER_NONE
,   CCE_RENDER_START
,   CCE_RENDER_END
,   CCE_RENDER_GAME_SELECTED
,   CCE_RENDER_PROMPT_GAME_TYPE
,   CCE_RENDER_PROMPT_COMMAND
,   CCE_RENDER_EXECUTE_COMMAND
,   CCE_RENDER_EXECUTE_MOVE_PLAYER
,   CCE_RENDER_EXECUTE_MOVE_ENGINE

,   CCE_RENDER_TAG_COUNT
}
CCE_RENDER_TAG;

// Type definition for an in-game executable command.
typedef enum
{
    CCE_COMMAND_QUIT
,   CCE_COMMAND_HELP
,   CCE_COMMAND_LIST_MOVES
,   CCE_COMMAND_CHOOSE_RANDOM_MOVE
,   CCE_COMMAND_DRAW
,   CCE_COMMAND_DEBUG

,   CCE_COMMAND_COUNT
}
CCE_COMMAND;

// Defines command strings.
static const char* cce_command_strings[] = { [ CCE_COMMAND_QUIT ]               = "Q"
                                           , [ CCE_COMMAND_HELP ]               = "H"
                                           , [ CCE_COMMAND_LIST_MOVES ]         = "L"
                                           , [ CCE_COMMAND_CHOOSE_RANDOM_MOVE ] = "R"
                                           , [ CCE_COMMAND_DRAW ]               = "D"
                                           , [ CCE_COMMAND_DEBUG ]              = "\\"
                                           };

// Defines keycodes to automatically issue commands.
#define CCE_KEY_SIGNAL_COMMAND_QUIT KEY_ESCAPE

// Defines textbuffer sizes.
#define CCE_RENDER_TEXTBUFFER_LENGTH 65535
#define CCE_INPUT_TEXTBUFFER_LENGTH  8

// Type definition for internal application state.
typedef struct
{
    // Game state.
    CCE_GAME_TAG        game;
    CCE_GAME_STATE      state;
    CCE_GAME_END_TAG    end;
    CCE_RENDER_TAG      render;
    u64                 update;
    u32                 ioerr;
    
    // Chess.
    attacks_t           attacks;
    board_t             board;
    moves_t             moves;
    move_t              move;
    u32                 ply;
    u32                 fifty;

    // Benchmarking.
    clock_t             clock;
    f64                 elapsed;

    // Command.
    CCE_COMMAND         cmd;
    char                in[ CCE_INPUT_TEXTBUFFER_LENGTH ];

    // Render textbuffer.
    u64                 textbuffer_offs;
    char                textbuffer[ CCE_RENDER_TEXTBUFFER_LENGTH ];

    // Logger.
    file_handle_t       log;
    u64                 logbuffer_offs;
    char                logbuffer[ CCE_RENDER_TEXTBUFFER_LENGTH ];
}
state_t;

// Global application state.
static cce_t* cce;

/**
 * @brief Primary implementation of cce_update (see cce_update).
 */
bool cce_game_init                  ( void );
bool cce_game_start                 ( void );
bool cce_game_end                   ( void );
bool cce_prompt_game_type           ( void );
bool cce_prompt_command             ( void );
bool cce_execute_command            ( void );
bool cce_execute_move_player        ( void );
bool cce_execute_move_engine        ( void );
bool cce_debug                      ( void );

/**
 * @brief Primary implementation of cce_render (see cce_render).
 */
void cce_render_start               ( void );
void cce_render_end                 ( void );
void cce_render_game_selected       ( void );
void cce_render_prompt_game_type    ( void );
void cce_render_prompt_command      ( void );
void cce_render_execute_command     ( void );
void cce_render_execute_move_player ( void );
void cce_render_execute_move_engine ( void );
void cce_render_splash_title        ( void );
void cce_render_splash_end          ( void );
void cce_render_list_commands       ( void );
void cce_render_list_moves          ( void );
void cce_render_board               ( void );
void cce_render_move                ( void );
void cce_render_buffer              ( void );

// Defines engine search depth.
#define CCE_ENGINE_SEARCH_DEPTH 4

/**
 * @brief User input handler.
 * @param char_count Number of characters to prompt for.
 * @return true on success, false otherwise.
 */
bool
cce_handle_user_input
(   const u8 char_count
);

/**
 * @brief User input filter predicate.
 * @param key The key to test.
 * @return true if key valid, false otherwise.
 */
INLINE
bool
cce_filter_user_input
(   const KEY key
)
{
    return key >= 32 && key <= 126;
}

// Log file interface.
#define CCE_LOG_FILEPATH "console.game.log"
void cce_log_splash_title ( void );
void cce_log_board        ( void );
void cce_log_move         ( void );

// Console color code indices for text rendering.
#define CCE_COLOR_DEFAULT   ANSI_CC ( ANSI_CC_NONE )
#define CCE_COLOR_HINT      ANSI_CC ( ANSI_CC_FG_GRAY )
#define CCE_COLOR_ALERT     ANSI_CC ( ANSI_CC_FG_RED )
#define CCE_COLOR_HIGHLIGHT ANSI_CC ( ANSI_CC_BG_DARK_RED )
#define CCE_COLOR_PLUS      ANSI_CC ( ANSI_CC_FG_DARK_GREEN )
#define CCE_COLOR_INFO      ANSI_CC ( ANSI_CC_FG_DARK_MAGENTA )

/**
 * @brief Renders the application state to the console and log file.
 */
#define RENDER()                                                                    \
({                                                                                  \
    platform_console_write ( ( *state ).textbuffer );                               \
    u64 written;                                                                    \
    if ( !file_write ( &( *state ).log                                              \
                     , ( *state ).logbuffer_offs                                    \
                     , ( *state ).logbuffer                                         \
                     , &written                                                     \
                     ))                                                             \
    {                                                                               \
        LOGWARN ( "Failed to write game data to log file '"CCE_LOG_FILEPATH"'." );  \
    }                                                                               \
})

/**
 * @brief **Effectively** clears the render and log textbuffers.
 */
#define RENDER_CLEAR()              \
({                                  \
    ( *state ).textbuffer_offs = 0; \
    ( *state ).logbuffer_offs = 0;  \
})

/**
 * @brief Appends a formatted string to the render textbuffer.
 */
#define RENDER_PUSH(fmt,...)                                                                            \
({                                                                                                      \
    ( *state ).textbuffer_offs += string_format (  ( *state ).textbuffer + ( *state ).textbuffer_offs   \
                                                , fmt , ##__VA_ARGS__                                   \
                                                );                                                      \
 })

/**
 * @brief Appends a formatted string to the log textbuffer.
 */
#define LOG_PUSH(fmt,...)                                                                           \
({                                                                                                  \
    ( *state ).logbuffer_offs += string_format (  ( *state ).logbuffer + ( *state ).logbuffer_offs  \
                                               , fmt , ##__VA_ARGS__                                \
                                               );                                                   \
})

bool
cce_startup
(   application_t*  app
)
{
    // Allocate memory for the application, including internal game state.
    cce = memory_allocate ( sizeof ( cce_t ) + sizeof ( state_t )
                          , MEMORY_TAG_APPLICATION
                          );
    ( *app ).app_state = cce;
    ( *cce ).internal = ( ( void* ) cce ) + sizeof ( cce_t );

    // Initialize game state.
    
    state_t* state = ( *cce ).internal;
    
    ( *state ).update = 0;
    ( *state ).ioerr = 0;

    memory_clear ( ( *state ).in
                 , sizeof ( ( *state ).in )
                 );

    ( *state ).textbuffer_offs = 0;
    memory_clear ( ( *state ).textbuffer
                 , sizeof ( ( *state ).textbuffer )
                 );

    ( *state ).logbuffer_offs = 0;
    memory_clear ( ( *state ).logbuffer
                 , sizeof ( ( *state ).logbuffer )
                 );

    // Open log file for writing.
    if ( !file_open ( CCE_LOG_FILEPATH
                    , FILE_MODE_WRITE
                    , false
                    , &( *state ).log
                    ))
    {
        LOGERROR ( "cce_startup: Unable to open game log file '"CCE_LOG_FILEPATH"' for writing." );
        return false;
    }

    // Pregenerate attack tables.
    attacks_init ( &( *state ).attacks );

    ( *state ).render = CCE_RENDER_NONE;
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

    // Free memory used by the application.
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
        case CCE_GAME_STATE_DEBUG              : return cce_debug ()               ;
        default                                : return true                       ;
    }
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

    // Write game state info to the textbuffers.
    // =>  Plaintext info into logbuffer.
    // =>  ANSI-formatted info into textbuffer.
    switch ( ( *state ).render )
    {
        case CCE_RENDER_NONE               : return          ;
        case CCE_RENDER_START              : RENDER_CLEAR () ; cce_render_start ()               ;break;
        case CCE_RENDER_END                : RENDER_CLEAR () ; cce_render_end ()                 ;break;
        case CCE_RENDER_GAME_SELECTED      : RENDER_CLEAR () ; cce_render_game_selected ()       ;break;
        case CCE_RENDER_PROMPT_GAME_TYPE   : RENDER_CLEAR () ; cce_render_prompt_game_type ()    ;break;
        case CCE_RENDER_PROMPT_COMMAND     : RENDER_CLEAR () ; cce_render_prompt_command ()      ;break;
        case CCE_RENDER_EXECUTE_COMMAND    : RENDER_CLEAR () ; cce_render_execute_command ()     ;break;
        case CCE_RENDER_EXECUTE_MOVE_PLAYER: RENDER_CLEAR () ; cce_render_execute_move_player () ;break;
        case CCE_RENDER_EXECUTE_MOVE_ENGINE: RENDER_CLEAR () ; cce_render_execute_move_engine () ;break;
        default                            : RENDER_CLEAR () ;                                    break;
    }

    // Render the textbuffers to the console and log file.
    RENDER ();
}

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

    ( *state ).ply = 0;
    ( *state ).fifty = 0;
    ( *state ).ioerr = 0;
    ( *state ).elapsed = 0;

    // Initialize chess board.
    fen_parse ( FEN_START , &( *state ).board );
    
    // Populate move list.
    moves_compute ( &( *state ).moves
                  , &( *state ).board
                  , &( *state ).attacks
                  );

    ( *state ).render = CCE_RENDER_GAME_SELECTED;
    ( *state ).state = ( ( *state ).game == CCE_GAME_ENGINE_VERSUS_ENGINE ) ? CCE_GAME_STATE_EXECUTE_MOVE_ENGINE
                                                                            : CCE_GAME_STATE_PROMPT_COMMAND
                                                                            ;
    return true;
}

bool
cce_game_end
( void )
{
    state_t* state = ( *cce ).internal;

    // Render ending splashtext.
    // Render immediately, since shutdown is coming next.
    RENDER_CLEAR ();
    cce_render_end ();
    RENDER ();

    if ( ( *state ).ply )
    {
        LOGINFO ( "A copy of the game was written to the log file: "CCE_LOG_FILEPATH"." );
    }

    // Signal engine to shutdown the application.
    event_context_t ctx = {};
    event_fire ( EVENT_CODE_APPLICATION_QUIT , 0 , ctx );

    // Turn off rendering and clear the game state.
    ( *state ).render = CCE_RENDER_NONE;
    ( *state ).state = CCE_GAME_STATE_COUNT;
    return true;
}

bool
cce_prompt_game_type
( void )
{
    state_t* state = ( *cce ).internal;

    // Read user response.
    if ( !cce_handle_user_input ( 1 ) )
    {
        return false;
    }
    
    // Sanitize input string.
    ( *state ).in[ sizeof ( ( *state ).in ) - 2 ] = 0;
    
    // Validate input string.
    if ( string_length ( ( *state ).in ) != 1 )
    {
        ( *state ).ioerr += 1;
        return true;
    }

    // Debug command issued? Y/N
    else if ( string_equal ( ( *state ).in
                           , cce_command_strings[ CCE_COMMAND_DEBUG ]
                           ))
    {
        // Issue the 'debug' command.
        ( *state ).cmd = CCE_COMMAND_DEBUG;
        ( *state ).render = CCE_RENDER_NONE;
        ( *state ).state = CCE_GAME_STATE_EXECUTE_COMMAND;
        return true;
    }

    // Validate input string.
    else if (   ( *state ).in[ 0 ] != '1'
             && ( *state ).in[ 0 ] != '2'
             && ( *state ).in[ 0 ] != '3'
            )
    {
        ( *state ).ioerr += 1;
        return true;
    }
    ( *state ).ioerr = 0;

    // Set the game type.
    ( *state ).game = to_digit ( ( *state ).in[ 0 ] ) - 1;

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
    
    // Read user response.
    if ( !cce_handle_user_input ( MOVE_STRING_LENGTH ) )
    {
        return false;
    }
       
    // Sanitize input string.
    ( *state ).in[ sizeof ( ( *state ).in ) - 2 ] = 0;
    string_trim ( ( *state ).in );
    
    // Attempt to parse command.
    for ( CCE_COMMAND i = 0; i < CCE_COMMAND_COUNT; ++i )
    {
        if ( string_equal ( ( *state ).in , cce_command_strings[ i ] ) )
        {
            ( *state ).ioerr = 0;
            ( *state ).cmd = i;
            ( *state ).render = CCE_RENDER_NONE;
            ( *state ).state = CCE_GAME_STATE_EXECUTE_COMMAND;
            return true;
        }
    }

    // Not a command. Attempt to parse move instead.
    if ( !move_parse ( ( *state ).in
                     , &( *state ).moves
                     , &( *state ).attacks
                     , &( *state ).move
                     ))
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

    ( *state ).ioerr = 0;
    
    // Evaluate command.
    switch ( ( *state ).cmd )
    {
            case CCE_COMMAND_HELP:
            case CCE_COMMAND_LIST_MOVES:
            {
                ( *state ).render = CCE_RENDER_EXECUTE_COMMAND;
                ( *state ).state = CCE_GAME_STATE_PROMPT_COMMAND;
                return true;
            }

            case CCE_COMMAND_CHOOSE_RANDOM_MOVE:
            {
                ( *state ).move = ( *state ).moves.moves[ random2 ( 0 , ( *state ).moves.count - 1 ) ];
                ( *state ).render = CCE_RENDER_NONE;
                ( *state ).state = CCE_GAME_STATE_EXECUTE_MOVE_PLAYER;
                return true;
            }

            case CCE_COMMAND_DRAW:
            {
                if ( ( *state ).fifty < 50 )
                {
                    ( *state ).render = CCE_RENDER_EXECUTE_COMMAND;
                    ( *state ).state = CCE_GAME_STATE_PROMPT_COMMAND;
                }
                else
                {
                    ( *state ).end = CCE_GAME_END_DRAW;
                    ( *state ).render = CCE_RENDER_NONE;
                    ( *state ).state = CCE_GAME_STATE_GAME_END;
                }
                return true;
            }

            case CCE_COMMAND_DEBUG:
            {
                ( *state ).render = CCE_RENDER_NONE;
                ( *state ).state = CCE_GAME_STATE_DEBUG;
                return true;
            }
            
            case CCE_COMMAND_QUIT:
            {
                ( *state ).end = CCE_GAME_END_TAG_COUNT;
                ( *state ).render = CCE_RENDER_NONE;
                ( *state ).state = CCE_GAME_STATE_GAME_END;
                return true;
            }
            
            default:
            {
                LOGERROR ( "cce_execute_command: called, but '%s' is not a known command."
                         , ( *state ).in    // see cce_prompt_command()
                         );
                return false;
            }
    }
    return true;
}

bool
cce_execute_move_player
( void )
{
    state_t* state = ( *cce ).internal;
    
    // Perform the move.
    board_move ( &( *state ).board
               , ( *state ).move
               , &( *state ).attacks
               );

    // Populate move list.
    moves_compute ( &( *state ).moves
                  , &( *state ).board
                  , &( *state ).attacks
                  );

    // Filter moves from the list which would put the player's own side into check.
    if ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_PLAYER )
    {
        board_t board;
        u32 count = 0;
        for ( u32 i = 0; i < ( *state ).moves.count; ++i )
        {
            // Preserve board state.
            memory_copy ( &board , &( *state ).board , sizeof ( board_t ) );

            // Perform move.
            board_move ( &board
                       , ( *state ).moves.moves[ i ]
                       , &( *state ).attacks
                       );

            // Check? Y/N
            if ( board_check ( &board
                             , &( *state ).attacks
                             , ( *state ).board.side
                             ))
            {
                continue;
            }

            // In-place filter.
            if ( i != count )
            {
                ( *state ).moves.moves[ count ] = ( *state ).moves.moves[ i ];
            }
            count += 1;
        }
 
        // Adjust move list count to account for filter.
        ( *state ).moves.count = count;
    }
    
    // Update fifty move and ply.
    const PIECE piece = move_decode_piece ( ( *state ).move );
    if ( move_decode_capture ( ( *state ).move ) || piece == P || piece == p )
    {
        ( *state ).fifty = 0;
    }
    else
    {
        ( *state ).fifty += 1;
    }
    ( *state ).ply += 1;

    // Evaluate endgame conditions.
    if ( board_stalemate ( &( *state ).board
                         , &( *state ).attacks
                         , &( *state ).moves
                         ))
    {
        ( *state ).end = CCE_GAME_END_STALEMATE;
        ( *state ).state = CCE_GAME_STATE_GAME_END;
    }
    else if ( board_checkmate ( &( *state ).board
                              , &( *state ).attacks
                              , &( *state ).moves
                              ))
    {
        ( *state ).end = CCE_GAME_END_CHECKMATE;
        ( *state ).state = CCE_GAME_STATE_GAME_END;
    }
    else
    {
        ( *state ).state = ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? CCE_GAME_STATE_EXECUTE_MOVE_ENGINE
                                                                                : CCE_GAME_STATE_PROMPT_COMMAND
                                                                                ;
    }
    
    ( *state ).render = CCE_RENDER_EXECUTE_MOVE_PLAYER;
    return true;
}

bool
cce_execute_move_engine
( void )
{
    state_t* state = ( *cce ).internal;
    
    // Render simulated input.
    RENDER_CLEAR ();
    RENDER_PUSH ( CCE_COLOR_HINT "Calculating best move. . .  " );
    RENDER ();

    // Start clock.
    clock_start ( &( *state ).clock );
    
    // Calculate best move.
    ( *state ).move = board_best_move ( &( *state ).board
                                      , &( *state ).attacks
                                      , CCE_ENGINE_SEARCH_DEPTH
                                      );

    // Stop clock.
    clock_update ( &( *state ).clock );
    ( *state ).elapsed += ( *state ).clock.elapsed;

    // Render simulated input.
    RENDER_CLEAR ();
    string_format ( ( *state ).textbuffer
                  , CCE_COLOR_HIGHLIGHT "%s"
                  , string_move ( ( *state ).textbuffer , ( *state ).move )
                  );
    const u64 indx = string_length ( ( *state ).textbuffer ) - 1;
    if ( ( *state ).textbuffer[ indx ] == ' ' )
    {
        ( *state ).textbuffer[ indx ] = 0;
    }
    RENDER ();

    // Perform the move.
    board_move ( &( *state ).board
               , ( *state ).move
               , &( *state ).attacks
               );

    // Populate move list.
    moves_compute ( &( *state ).moves
                  , &( *state ).board
                  , &( *state ).attacks
                  );

    // Filter moves from the list which would put the player's own side into check.
    if ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE )
    {
        board_t board;
        u32 count = 0;
        for ( u32 i = 0; i < ( *state ).moves.count; ++i )
        {
            // Preserve board state.
            memory_copy ( &board , &( *state ).board , sizeof ( board_t ) );

            // Perform move.
            board_move ( &board
                       , ( *state ).moves.moves[ i ]
                       , &( *state ).attacks
                       );

            // Check? Y/N
            if ( board_check ( &board
                             , &( *state ).attacks
                             , ( *state ).board.side
                             ))
            {
                continue;
            }

            // In-place filter.
            if ( i != count )
            {
                ( *state ).moves.moves[ count ] = ( *state ).moves.moves[ i ];
            }
            count += 1;
        }
 
        // Adjust move list count to account for filter.
        ( *state ).moves.count = count;
    }
    
    // Update fifty move and ply.
    const PIECE piece = move_decode_piece ( ( *state ).move );
    if ( move_decode_capture ( ( *state ).move ) || piece == P || piece == p )
    {
        ( *state ).fifty = 0;
    }
    else
    {
        ( *state ).fifty += 1;
    }
    ( *state ).ply += 1;

    // Evaluate endgame conditions.
    if ( board_stalemate ( &( *state ).board
                         , &( *state ).attacks
                         , &( *state ).moves
                         ))
    {
        ( *state ).end = CCE_GAME_END_STALEMATE;
        ( *state ).state = CCE_GAME_STATE_GAME_END;
    }
    else if ( board_checkmate ( &( *state ).board
                              , &( *state ).attacks
                              , &( *state ).moves
                              ))
    {
        ( *state ).end = CCE_GAME_END_CHECKMATE;
        ( *state ).state = CCE_GAME_STATE_GAME_END;
    }
    else if (   ( *state ).game == CCE_GAME_ENGINE_VERSUS_ENGINE
             && ( *state ).fifty >= 50
            )
    {
        ( *state ).end = CCE_GAME_END_DRAW;
        ( *state ).state = CCE_GAME_STATE_GAME_END;
    }
    else
    {
        ( *state ).state = ( ( *state ).game == CCE_GAME_ENGINE_VERSUS_ENGINE ) ? CCE_GAME_STATE_EXECUTE_MOVE_ENGINE
                                                                                : CCE_GAME_STATE_PROMPT_COMMAND
                                                                                ;
    }
    
    ( *state ).render = CCE_RENDER_EXECUTE_MOVE_ENGINE;
    return true;
}

bool
cce_handle_user_input
(   const u8 char_count
)
{
    state_t* state = ( *cce ).internal;

    // Clear any previous user input.
    memory_clear ( ( *state ).in , sizeof ( ( *state ).in ) );

    KEY key;
    u8 indx = 0;
    for (;;)
    {
        key = platform_console_read_key ();
        
        if ( key == KEY_COUNT )
        {
            RENDER_CLEAR ();
            RENDER_PUSH ( "\n" );
            RENDER ();
            LOGERROR ( "cce_handle_user_input: Failed to get user input keystroke from stdin." );
            return false;
        }
        
        // Submit response? Y/N
        if ( key == KEY_ENTER )
        {
            return true;
        }
        
        // Quit signal? Y/N
        if ( key == CCE_KEY_SIGNAL_COMMAND_QUIT )
        {
            // Render simulated input.
            RENDER_CLEAR ();
            RENDER_PUSH ( "%s" , cce_command_strings[ CCE_COMMAND_QUIT ] );
            RENDER ();

            // Issue the 'quit' command.
            ( *state ).cmd = CCE_COMMAND_QUIT;
            ( *state ).render = CCE_RENDER_NONE;
            ( *state ).state = CCE_GAME_STATE_EXECUTE_COMMAND;
            return true;
        }

        // Handle backspace.
        if ( key == KEY_BACKSPACE )
        {
            if ( !indx )
            {
                continue;
            }
            indx -= 1;
            ( *state ).in[ indx ] = 0;
        }

        // Response too long? Y/N
        else if ( indx >= char_count )
        {
            indx = char_count;
            continue;
        }

        // Invalid keycode? Y/N
        else if ( !cce_filter_user_input ( key ) )
        {
            continue;
        }

        // Write to input buffer.
        else
        {
            ( *state ).in[ indx ] = to_uppercase ( key );
            indx += 1;
        }

        // Render the character.
        RENDER_CLEAR ();
        RENDER_PUSH ( "%s"
                    , ( key == KEY_BACKSPACE ) ? "\b \b"
                                               : ( char[] ){ key , 0 }
                    );
        RENDER ();
    }
}

void
cce_render_start
( void )
{
    // ---------- TEMPORARY ----------
    cce_render_buffer ();           // Refreshes console by rendering newlines.
    // -------------------------------

    // Render splashtext.
    cce_render_splash_title ();

    // Peek first prompt.
    cce_render_prompt_game_type ();
}

void
cce_render_end
( void )
{
    state_t* state = ( *cce ).internal;

    // Render game result.
    switch ( ( *state ).end )
    {
        case CCE_GAME_END_CHECKMATE:
        {
            RENDER_PUSH ( CCE_COLOR_ALERT "\n\t-=-=-=-=-=-=-=-= " CCE_COLOR_DEFAULT
                          CCE_COLOR_HIGHLIGHT "    CHECKMATE    " CCE_COLOR_DEFAULT
                          CCE_COLOR_ALERT " -=-=-=-=-=-=-=-="
                          "\n\n\t                     %s WINS."
                        , ( ( *state ).board.side != WHITE ) ? "WHITE"
                                                             : "BLACK"
                        );
            LOG_PUSH (   "\n\t-=-=-=-=-=-=-=-=     CHECKMATE     =-=-=-=-=-=-=-=-"
                       "\n\n\t                     %s WINS."
                     , ( ( *state ).board.side != WHITE ) ? "WHITE"
                                                          : "BLACK"                             
                     );
        }
        break;

        case CCE_GAME_END_STALEMATE:
        {
            RENDER_PUSH ( CCE_COLOR_ALERT "\n\t-=-=-=-=-=-=-=-= " CCE_COLOR_DEFAULT
                          CCE_COLOR_HIGHLIGHT "    STALEMATE    " CCE_COLOR_DEFAULT
                          CCE_COLOR_ALERT " =-=-=-=-=-=-=-=-"
                          "\n"
                        );
            LOG_PUSH ( "\n\t-=-=-=-=-=-=-=-=     STALEMATE     =-=-=-=-=-=-=-=-"
                       "\n"
                     );
        }
        break;

        case CCE_GAME_END_DRAW:
        {
            RENDER_PUSH ( CCE_COLOR_ALERT "\n\t-=-=-=-=-=-=-=-= " CCE_COLOR_DEFAULT
                          CCE_COLOR_HIGHLIGHT "      DRAW       " CCE_COLOR_DEFAULT
                          CCE_COLOR_ALERT " =-=-=-=-=-=-=-=-"
                          "\n\n\t%s chose to end the game in a draw."
                        , ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? ( ( *state ).board.side != WHITE ) ? "WHITE (player)"
                                                                                                                    : "BLACK (engine)"
                                                                               : ( ( *state ).board.side != WHITE ) ? "WHITE"
                                                                                                                    : "BLACK"
                        );
            LOG_PUSH ( "\n\t-=-=-=-=-=-=-=-=       DRAW        =-=-=-=-=-=-=-=-"
                       "\n\n\t%s chose to end the game in a draw."
                     , ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? ( ( *state ).board.side != WHITE ) ? "WHITE (player)"
                                                                                                                 : "BLACK (engine)"
                                                                            : ( ( *state ).board.side != WHITE ) ? "WHITE"
                                                                                                                 : "BLACK"
                     );
        }
        break;

        default:
        {}
        break;
    }

    // Render benchmarking info.
    if ( ( *state ).game == CCE_GAME_ENGINE_VERSUS_ENGINE )
    {
        RENDER_PUSH ( CCE_COLOR_HINT "\n\n\tEngine calculations took a total of %f seconds.\n\n"
                    , ( *state ).elapsed
                    );
    }

    // Render splashtext.
    cce_render_splash_end ();
}

void
cce_render_game_selected
( void )
{
    state_t* state = ( *cce ).internal;
    const CCE_GAME_TAG game = ( *state ).game;
        
    // ---------- TEMPORARY ----------
    cce_render_buffer ();           // Refreshes console by rendering newlines.
    // -------------------------------
    
    // Render splashtext.
    cce_render_splash_title ();
    cce_log_splash_title ();

    // Render info.
    const char* s_game;
    switch ( game )
    {
        case CCE_GAME_PLAYER_VERSUS_PLAYER: s_game = "PLAYER VERSUS PLAYER" ;break;
        case CCE_GAME_PLAYER_VERSUS_ENGINE: s_game = "PLAYER VERSUS ENGINE" ;break;
        case CCE_GAME_ENGINE_VERSUS_ENGINE: s_game = "ENGINE VERSUS ENGINE" ;break;
        default                           : s_game = ""                     ;break;
    }
    RENDER_PUSH ( CCE_COLOR_PLUS "\n\tStarting new game of %s. . .\n\n\n"
                , s_game
                );
    LOG_PUSH ( "\n\tNew game of %s started.\n\n\n"
             , s_game
             );
    
    // Render starting board state.
    cce_render_board ();
    cce_log_board ();

    // Render first prompt.
    cce_render_prompt_command ();
}

void
cce_render_prompt_game_type
( void )
{
    state_t* state = ( *cce ).internal;
    RENDER_PUSH ( CCE_COLOR_DEFAULT "\n\t%s" CCE_COLOR_INFO "\n"
                  "\n\t                CHOOSE A GAME STYLE:               "
                  "\n\t                                                   "
                  "\n\t            [ 1 ]  PLAYER VERSUS PLAYER.           "
                  "\n\t            [ 2 ]  PLAYER VERSUS ENGINE.           "
                  "\n\t            [ 3 ]  ENGINE VERSUS ENGINE.           "
                  "\n\t                                                   "
                  CCE_COLOR_HINT "\n\t                   Press <Esc> to quit."
                  CCE_COLOR_INFO "\n\tCHOICE:       "
                , ( ( *state ).ioerr ) ? CCE_COLOR_ALERT "\tPLEASE CHOOSE FROM THE OPTIONS PROVIDED."
                                       : "\n"
                );
}

void
cce_render_prompt_command
( void )
{
    state_t* state = ( *cce ).internal;

    if ( ( *state ).ioerr )
    {
        // ---------- TEMPORARY ----------
        cce_render_buffer ();           // Refresh console by rendering newlines.
        // -------------------------------

        // Render hint.
        if ( ( *state ).ioerr > 2 )
        {
            RENDER_PUSH ( CCE_COLOR_HINT "\n\n\tTrying to exit? Press <Esc> at any time,"
                                           "\n\tor pass '%s' as the next move."
                                         "\n\n\tNeed help? Pass '%s' for help options.\n"
                        , cce_command_strings[ CCE_COMMAND_QUIT ]
                        , cce_command_strings[ CCE_COMMAND_HELP ]
                        );
        }

        // Render the previous move.
        if ( ( *state ).ply )
        {
            cce_render_move ();
        }

        // Render the board again.
        cce_render_board ();

        // Render prompt.
        const u8 len = string_length ( ( *state ).in );
        const bool too_long = len > MOVE_STRING_LENGTH;
        if ( too_long )
        {
            ( *state ).in[ MOVE_STRING_LENGTH ] = 0;
        }
        if ( len )
        {
            RENDER_PUSH ( CCE_COLOR_INFO "\n\t" CCE_COLOR_DEFAULT
                          CCE_COLOR_HIGHLIGHT "%s%s"
                          CCE_COLOR_DEFAULT CCE_COLOR_ALERT " is not a known command or valid chess move."
                          CCE_COLOR_INFO    "\n\tISSUE MOVE OR COMMAND:  " CCE_COLOR_DEFAULT
                        , ( *state ).in
                        , ( too_long ) ? ".." : ""
                        );
        }
        else
        {
            RENDER_PUSH ( CCE_COLOR_INFO "\n\t"
                          CCE_COLOR_ALERT "Please issue a command or valid chess move."
                          CCE_COLOR_INFO "\n\tISSUE MOVE OR COMMAND:  " CCE_COLOR_DEFAULT
                        );
        }
        
    }
    else
    {
        RENDER_PUSH ( CCE_COLOR_INFO "\n\n\n\tISSUE MOVE OR COMMAND:  " CCE_COLOR_DEFAULT );
    }
}

void
cce_render_execute_command
( void )
{
    state_t* state = ( *cce ).internal;
            
    // ---------- TEMPORARY ----------
    cce_render_buffer ();           // Refreshes console by rendering newlines.
    // -------------------------------
    
    // Evaluate command.
    switch ( ( *state ).cmd )
    {
            case CCE_COMMAND_HELP      : cce_render_list_commands () ;break;
            case CCE_COMMAND_LIST_MOVES: cce_render_list_moves ()    ;break;

            case CCE_COMMAND_DRAW:
            {
                if ( ( *state ).fifty < 50 )
                {
                    RENDER_PUSH ( CCE_COLOR_ALERT "\tYou cannot draw at this time. Fifty move: %u\n"
                                , ( *state ).fifty
                                );
                }
            }
            break;

            default:
            {}
            break;
    }

    // Render the previous move.
    if ( ( *state ).ply )
    {
        cce_render_move ();
    }
    
    // Render the board state.
    cce_render_board ();

    // Render the prompt.
    cce_render_prompt_command ();
}

void
cce_render_execute_move_player
( void )
{
    state_t* state = ( *cce ).internal;

    // Render the move.
    cce_render_move ();
    cce_log_move ();

    // Render the board state.
    cce_render_board ();
    cce_log_board ();

    // Render the prompt.
    if ( ( *state ).state != CCE_GAME_STATE_GAME_END )
    {
        cce_render_prompt_command ();
    }
}

void
cce_render_execute_move_engine
( void )
{
    state_t* state = ( *cce ).internal;

    // Render the time taken to choose the move.
    RENDER_PUSH ( CCE_COLOR_HINT "\n\t\t\t\tTook %f seconds."
                , ( *state ).clock.elapsed
                );

    // Render the move.
    cce_render_move ();
    cce_log_move ();

    // Render the board state.
    cce_render_board ();
    cce_log_board ();

    // Render the (phony) prompt.
    if ( ( *state ).state != CCE_GAME_STATE_GAME_END )
    {
        cce_render_prompt_command ();
    }
}

void
cce_render_splash_title
( void )
{
    state_t* state = ( *cce ).internal;
    RENDER_PUSH ( CCE_COLOR_INFO
                  "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                  "\n\t~                                                 ~"
                  "\n\t~                CCE CHESS ENGINE.                ~"
                  "\n\t~                (   v. %i.%i.%i   )                 ~"
                  "\n\t~           created by Matthew Weissel.           ~"
                  "\n\t~                                                 ~"
                  "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                  "\n\n\n"
                , VERSION_MAJOR , VERSION_MINOR , VERSION_PATCH
                );
}

void
cce_render_splash_end
( void )
{
    state_t* state = ( *cce ).internal;
    RENDER_PUSH ( CCE_COLOR_INFO
                  "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                  "\n\t~                                                 ~"
                  "\n\t~                                                 ~"
                  "\n\t~               Thanks for playing!               ~"
                  "\n\t~                                                 ~"
                  "\n\t~                                                 ~"
                  "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
                  "\n\n\n" ANSI_CC_RESET
                );
}

void
cce_render_list_commands
( void )
{
    state_t* state = ( *cce ).internal;
    const bool fifty = ( *state ).fifty >= 50;
    RENDER_PUSH ( CCE_COLOR_HINT
                  "\n\t                                                   "
                  "\n\t         =-=-=- AVAILABLE COMMANDS -=-=-=          "
                  "\n\t                                                   "
                  "\n\t  %s :      List available commands.               "
                  "\n                                                     "
                  "\n\t  %s :      List available moves.                  "
                  "\n\t  %s :      Choose random valid move.              "
                  "\n                                                     "
                  "\n\t  %s :      End the game in a draw.                "
                  "\n\t            Eligibility: %s"
                , cce_command_strings[ CCE_COMMAND_HELP ]
                , cce_command_strings[ CCE_COMMAND_LIST_MOVES ]
                , cce_command_strings[ CCE_COMMAND_CHOOSE_RANDOM_MOVE ]
                , cce_command_strings[ CCE_COMMAND_DRAW ]
                , ( fifty ) ? CCE_COLOR_PLUS "ELIGIBLE" CCE_COLOR_HINT
                            : CCE_COLOR_ALERT "NOT ELIGIBLE" CCE_COLOR_HINT
                );
    if ( !fifty )
    {
        RENDER_PUSH ( CCE_COLOR_HINT "\n\t            Eligible in "
                      CCE_COLOR_PLUS "%i" CCE_COLOR_HINT " moves."
                      "\n                                                     "
                    , 50 - ( *state ).fifty
                    );
    }
    else
    {
        RENDER_PUSH ( CCE_COLOR_HINT
                      "\n                                                     "
                      "\n                                                     "
                    );
    }

    RENDER_PUSH ( "\n\t  %s :      Quit the application.                  "
                  "\n\t                                                   "
                  "\n\t         =-=-=-                    -=-=-=          "
                  "\n"
                , cce_command_strings[ CCE_COMMAND_QUIT ]
                );
}

void
cce_render_list_moves
( void )
{
    state_t* state = ( *cce ).internal;
    RENDER_PUSH ( CCE_COLOR_HINT "%s"
                , string_moves ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                               , &( *state ).moves
                               ));
}

void
cce_render_board
( void )
{
    state_t* state = ( *cce ).internal;

    RENDER_PUSH ( CCE_COLOR_INFO "\n\tBOARD: %s\n"
                , fen_from_board ( ( *state ).textbuffer + ( *state ).textbuffer_offs
                                 , &( *state ).board
                                 ));

    u8 i = 0;
    u8 r = 0;
    u8 f = 0;

    while ( r < 8 )
    {
        RENDER_PUSH ( CCE_COLOR_INFO "\n\t\t  " );

        while ( f < 8 )
        {
            if ( !f )
            {
                RENDER_PUSH ( "%u   " , 8 - r );
            }

            PIECE piece = EMPTY_SQ; 
            for ( PIECE piece_ = P; piece_ < 12; ++piece_ )
            {
                if ( bit ( ( *state ).board.pieces[ piece_ ]
                         , SQUAREINDX ( r , f )
                         ))
                {
                    piece = piece_;
                }
            }
            
            const bool blackchr = piece >= p && piece <= k;
            
            // Render Unicode or ASCII piece, depending on what the platform-
            // layer console supports.
            const char* s_piece;
            #if PLATFORM_WINDOWS == 1
                s_piece = ( piece != EMPTY_SQ ) ? ( blackchr ) ? piecewchr ( piece - p )
                                                               : piecewchr ( piece )
                                                : " "
                                                ;
            #else
                s_piece = ( piece != EMPTY_SQ ) ? ( blackchr ) ? piecewchr ( piece )
                                                               : piecewchr ( p + piece )
                                                : " "
                                                ;
            #endif
        
            RENDER_PUSH ( CCE_COLOR_DEFAULT "%s %s " CCE_COLOR_DEFAULT
                        , even ( i ) ? ( blackchr ) ? ANSI_CC2 ( ANSI_CC_BG_RED , ANSI_CC_FG_BLACK ) 
                                                    : ANSI_CC2 ( ANSI_CC_BG_RED , ANSI_CC_FG_WHITE )
                                     : ( blackchr ) ? ANSI_CC2 ( ANSI_CC_BG_DARK_RED , ANSI_CC_FG_BLACK ) 
                                                    : ANSI_CC2 ( ANSI_CC_BG_DARK_RED , ANSI_CC_FG_WHITE )
                        , s_piece
                        );

            f += 1;
            i += 1;
        }

        f = 0;
        r += 1;
        i += 1;
    }
    
    RENDER_PUSH ( CCE_COLOR_INFO "\n\n\t\t      " );

    r = 0;
    while ( r < 8 )
    {
        RENDER_PUSH ( " %c " , 'A' + r );
        r += 1;
    }

    RENDER_PUSH ( "\n\n\n\t\t  Side:         %s\n\t\t  En passant:      %s\n\t\t  Castling:      %c%c%c%c\n\n"
                , ( ( *state ).board.side == WHITE ) ? "white"
                                                     : "black"
                , ( ( *state ).board.enpassant != NO_SQ ) ? string_square ( ( *state ).board.enpassant )
                                                          : "no"
                , ( ( *state ).board.castle & CASTLE_WK ) ? 'K' : '-'
                , ( ( *state ).board.castle & CASTLE_WQ ) ? 'Q' : '-'
                , ( ( *state ).board.castle & CASTLE_BK ) ? 'k' : '-'
                , ( ( *state ).board.castle & CASTLE_BQ ) ? 'q' : '-'
                );
}

void
cce_render_move
( void )
{
    state_t* state = ( *cce ).internal;

    // Capture.
    if ( move_decode_capture ( ( *state ).move ) )
    {
        RENDER_PUSH ( CCE_COLOR_PLUS "\n\n\t%s (%u): %s ON %s CAPTURED %s ON %s."
                    , ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? ( ( *state ).board.side != WHITE ) ? "WHITE (player)"
                                                                                                                : "BLACK (engine)"
                                                                           : ( ( *state ).board.side != WHITE ) ? "WHITE"
                                                                                                                : "BLACK"
                    , ( *state ).ply / 2 + 1
                    , piecewchr ( move_decode_piece ( ( *state ).move ) )
                    , string_square ( move_decode_src ( ( *state ).move ) )
                    , piecewchr ( ( *state ).board.capture )
                    , string_square ( move_decode_dst ( ( *state ).move ) )
                    );
    }

    // Quiet.
    else
    {
        RENDER_PUSH ( CCE_COLOR_PLUS "\n\n\t%s (%u): %s ON %s TO %s."
                    , ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? ( ( *state ).board.side != WHITE ) ? "WHITE (player)"
                                                                                                                : "BLACK (engine)"
                                                                           : ( ( *state ).board.side != WHITE ) ? "WHITE"
                                                                                                                : "BLACK"
                    , ( *state ).ply / 2 + 1
                    , piecewchr ( move_decode_piece ( ( *state ).move ) )
                    , string_square ( move_decode_src ( ( *state ).move ) )
                    , string_square ( move_decode_dst ( ( *state ).move ) )
                    );
    }

    // Promotion.
    if ( move_decode_promotion ( ( *state ).move ) )
    {
        RENDER_PUSH ( " PROMOTED TO %s."
                    , piecewchr ( move_decode_promotion ( ( *state ).move ) )
                    );
    }
    
    // Check.
    if ( board_check ( &( *state ).board
                     , &( *state ).attacks
                     , ( *state ).board.side
                     ))
    {
        RENDER_PUSH ( CCE_COLOR_ALERT "\tCHECK." );
    }
}

void
cce_render_buffer
( void )
{
    state_t* state = ( *cce ).internal;
    RENDER_PUSH ( CCE_COLOR_DEFAULT "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );
}

void
cce_log_splash_title
( void )
{
    state_t* state = ( *cce ).internal;
    LOG_PUSH ( "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
               "\n\t~                                                 ~"
               "\n\t~                CCE CHESS ENGINE.                ~"
               "\n\t~                (   v. %i.%i.%i   )                 ~"
               "\n\t~           created by Matthew Weissel.           ~"
               "\n\t~                                                 ~"
               "\n\t=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="
               "\n\n\n"
              , VERSION_MAJOR , VERSION_MINOR , VERSION_PATCH
             );
}

void
cce_log_board
( void )
{
    state_t* state = ( *cce ).internal;
    LOG_PUSH ( "%s"
             , string_board ( ( *state ).logbuffer + ( *state ).logbuffer_offs
                            , &( *state ).board
                            ));
}

void
cce_log_move
( void )
{
    state_t* state = ( *cce ).internal;

    // Capture.
    if ( move_decode_capture ( ( *state ).move ) )
    {
        LOG_PUSH ( "\n\n\t%s (%u): %s ON %s CAPTURED %s ON %s."
                 , ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? ( ( *state ).board.side != WHITE ) ? "WHITE (player)"
                                                                                                             : "BLACK (engine)"
                                                                        : ( ( *state ).board.side != WHITE ) ? "WHITE"
                                                                                                             : "BLACK"
                 , ( *state ).ply / 2 + 1
                 , piecewchr ( move_decode_piece ( ( *state ).move ) )
                 , string_square ( move_decode_src ( ( *state ).move ) )
                 , piecewchr ( ( *state ).board.capture )
                 , string_square ( move_decode_dst ( ( *state ).move ) )
                 );
    }

    // Quiet.
    else
    {
        LOG_PUSH ( "\n\n\t%s (%u): %s ON %s TO %s."
                 , ( ( *state ).game == CCE_GAME_PLAYER_VERSUS_ENGINE ) ? ( ( *state ).board.side != WHITE ) ? "WHITE (player)"
                                                                                                             : "BLACK (engine)"
                                                                        : ( ( *state ).board.side != WHITE ) ? "WHITE"
                                                                                                             : "BLACK"
                 , ( *state ).ply / 2 + 1
                 , piecewchr ( move_decode_piece ( ( *state ).move ) )
                 , string_square ( move_decode_src ( ( *state ).move ) )
                 , string_square ( move_decode_dst ( ( *state ).move ) )
                 );
    }

    // Promotion.
    if ( move_decode_promotion ( ( *state ).move ) )
    {
        LOG_PUSH ( " PROMOTED TO %s."
                 , piecewchr ( move_decode_promotion ( ( *state ).move ) )
                 );
    }
    
    // Check.
    if ( board_check ( &( *state ).board
                     , &( *state ).attacks
                     , ( *state ).board.side
                     ))
    {
        LOG_PUSH ( "\tCHECK." );
    }
}

#include "chess/negamax.h"
bool
cce_debug
( void )
{
    state_t* state = ( *cce ).internal;

    // Render newline.
    RENDER_CLEAR ();
    RENDER_PUSH ( "\n" );
    RENDER ();

    LOGDEBUG ( "cce_debug: Running debug routine. . ." );

    board_t board;
    memory_clear ( &board , sizeof ( board_t ) );
    fen_parse ( FEN_CMK
              , &board
              );

    moves_t moves;
    LOGINFO ( string_moves ( ( *state ).textbuffer
                           , moves_compute ( &moves
                                           , &board
                                           , &( *state ).attacks
                                           )));

    LOGDEBUG ( "cce_debug: Done. Exiting." );

    ( *state ).end = CCE_GAME_END_TAG_COUNT;
    ( *state ).render = CCE_RENDER_NONE;
    ( *state ).state = CCE_GAME_STATE_GAME_END;
    return true;
}