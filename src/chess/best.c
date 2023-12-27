/**
 * @file best.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Implementation of the best header.
 * (see best.h for additional details)
 */
#include "chess/best.h"

#include "chess/board.h"

// Defines a material score for each piece.
static const i32 material_scores[] = { [ P ] = 100
                                     , [ N ] = 300
                                     , [ B ] = 350
                                     , [ R ] = 500
                                     , [ Q ] = 1000
                                     , [ K ] = 10000
                                     , [ p ] = -100
                                     , [ n ] = -300
                                     , [ b ] = -350
                                     , [ r ] = -500
                                     , [ q ] = -1000
                                     , [ k ] = -10000
                                     };

// Defines a positional score table for each piece.
static const i32 pawn_positional_scores[] = { 90 ,  90 ,  90 ,  90 ,  90 ,  90 ,  90 ,  90
                                            , 30 ,  30 ,  30 ,  40 ,  40 ,  30 ,  30 ,  30
                                            , 20 ,  20 ,  20 ,  30 ,  30 ,  30 ,  20 ,  20
                                            , 10 ,  10 ,  10 ,  20 ,  20 ,  10 ,  10 ,  10
                                            ,  5 ,   5 ,  10 ,  20 ,  20 ,   5 ,   5 ,   5
                                            ,  0 ,   0 ,   0 ,   5 ,   5 ,   0 ,   0 ,   0
                                            ,  0 ,   0 ,   0 , -10 , -10 ,   0 ,   0 ,   0
                                            ,  0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0
                                            };
static const i32 knight_positional_scores[] = { -5 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,  -5
                                              , -5 ,   0 ,   0 ,  10 ,  10 ,   0 ,   0 ,  -5
                                              , -5 ,   5 ,  20 ,  20 ,  20 ,  20 ,   5 ,  -5
                                              , -5 ,  10 ,  20 ,  30 ,  30 ,  20 ,  10 ,  -5
                                              , -5 ,  10 ,  20 ,  30 ,  30 ,  20 ,  10 ,  -5
                                              , -5 ,   5 ,  20 ,  10 ,  10 ,  20 ,   5 ,  -5
                                              , -5 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,  -5
                                              , -5 , -10 ,   0 ,   0 ,   0 ,   0 , -10 ,  -5
                                              };
static const i32 bishop_positional_scores[] = {  0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0
                                              ,  0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0
                                              ,  0 ,   0 ,   0 ,  10 ,  10 ,   0 ,   0 ,   0
                                              ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                              ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                              ,  0 ,  10 ,   0 ,   0 ,   0 ,   0 ,  10 ,   0
                                              ,  0 ,  30 ,   0 ,   0 ,   0 ,   0 ,  30 ,   0
                                              ,  0 ,   0 , -10 ,   0 ,   0 , -10 ,   0 ,   0
                                              };
static const i32 rook_positional_scores[] = { 50 ,  50 ,  50 ,  50 ,  50 ,  50 ,  50 ,  50
                                            , 50 ,  50 ,  50 ,  50 ,  50 ,  50 ,  50 ,  50
                                            ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                            ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                            ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                            ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                            ,  0 ,   0 ,  10 ,  20 ,  20 ,  10 ,   0 ,   0
                                            ,  0 ,   0 ,   0 ,  20 ,  20 ,   0 ,   0 ,   0
                                            };
static const i32 king_positional_scores[] = {  0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0 ,   0
                                            ,  0 ,   0 ,   5 ,   5 ,   5 ,   5 ,   0 ,   0
                                            ,  0 ,   5 ,   5 ,  10 ,  10 ,   5 ,   5 ,   0
                                            ,  0 ,   5 ,  10 ,  20 ,  20 ,  10 ,   5 ,   0
                                            ,  0 ,   5 ,  10 ,  20 ,  20 ,  10 ,   5 ,   0
                                            ,  0 ,   0 ,   5 ,  10 ,  10 ,   5 ,   0 ,   0
                                            ,  0 ,   5 ,   5 ,  -5 ,  -5 ,   0 ,   5 ,   0
                                            ,  0 ,   0 ,   5 ,   0 , -15 ,   0 ,  10 ,   0
                                            };

// Defines the mirror score table indices for calculating the opposite side's
// score.
static const SQUARE mirror_position[ 128 ] = { A1 , B1 , C1 , D1 , E1 , F1 , G1 , H1
	                                         , A2 , B2 , C2 , D2 , E2 , F2 , G2 , H2
	   	                                     , A3 , B3 , C3 , D3 , E3 , F3 , G3 , H3
	                                         , A4 , B4 , C4 , D4 , E4 , F4 , G4 , H4
	                                         , A5 , B5 , C5 , D5 , E5 , F5 , G5 , H5
	                                         , A6 , B6 , C6 , D6 , E6 , F6 , G6 , H6
	                                         , A7 , B7 , C7 , D7 , E7 , F7 , G7 , H7
	                                         , A8 , B8 , C8 , D8 , E8 , F8 , G8 , H8
	                                         };

// Defines most-valuable victim versus least-valuable attacker table.
static const i32 mvv_lva[ 12 ][ 12 ] = { { 105 , 205 , 305 , 405 , 505 , 605 ,   105 , 205 , 305 , 405 , 505 , 605 }
	                                   , { 104 , 204 , 304 , 404 , 504 , 604 ,   104 , 204 , 304 , 404 , 504 , 604 }
	                                   , { 103 , 203 , 303 , 403 , 503 , 603 ,   103 , 203 , 303 , 403 , 503 , 603 }
	                                   , { 102 , 202 , 302 , 402 , 502 , 602 ,   102 , 202 , 302 , 402 , 502 , 602 }
	                                   , { 101 , 201 , 301 , 401 , 501 , 601 ,   101 , 201 , 301 , 401 , 501 , 601 }
	                                   , { 100 , 200 , 300 , 400 , 500 , 600 ,   100 , 200 , 300 , 400 , 500 , 600 }

	                                   , { 105 , 205 , 305 , 405 , 505 , 605 ,   105 , 205 , 305 , 405 , 505 , 605 }
	                                   , { 104 , 204 , 304 , 404 , 504 , 604 ,   104 , 204 , 304 , 404 , 504 , 604 }
	                                   , { 103 , 203 , 303 , 403 , 503 , 603 ,   103 , 203 , 303 , 403 , 503 , 603 }
	                                   , { 102 , 202 , 302 , 402 , 502 , 602 ,   102 , 202 , 302 , 402 , 502 , 602 }
	                                   , { 101 , 201 , 301 , 401 , 501 , 601 ,   101 , 201 , 301 , 401 , 501 , 601 }
	                                   , { 100 , 200 , 300 , 400 , 500 , 600 ,   100 , 200 , 300 , 400 , 500 , 600 }
	                                   };

/**
 * @brief Negamax search.
 * @param alpha Alpha negamax cutoff.
 * @param beta Beta negamax cutoff.
 * @param depth Current recursion depth.
 * @param args Static function arguments.
 * @return Current negamax score.
 */
i32
negamax
(   i32             alpha
,   i32             beta
,   u32             depth
,   move_search_t*  args
);

/**
 * @brief Quiescence search.
 * @param alpha Alpha quiescence cutoff.
 * @param beta Beta quiescence cutoff.
 * @param args Static function arguments.
 * @return Current quiescence score.
 */
i32
quiescence
(   i32             alpha
,   i32             beta
,   move_search_t*  args
);

/**
 * @brief Board state evaluation function.
 * @param board A chess board state.
 * @return A score corresponding to the board state.
 */
i32
score_board
(   const board_t* board
);

/**
 * @brief Move evaluation function.
 * @param move A move.
 * @param args Static function arguments.
 * @return A score corresponding to the move.
 */
i32
score_move
(   const move_t    move
,   move_search_t*  args
);

/**
 * @brief Sorts a move list by move score.
 * @param moves A pregenerated list of valid moves.
 * @param args Static function arguments.
 * @return moves.
 */
moves_t*
moves_sort_by_score
(   moves_t*        moves
,   move_search_t*  args
);

/**
 * @brief Enables scoring of the principal variation moves.
 * @param moves A pregenerated list of valid moves.
 * @param args Static function arguments.
 * @return moves.
 */
moves_t*
moves_enable_pv_scoring
(   moves_t*        moves
,   move_search_t*  args
);

#include "chess/string.h"   // Temporary.
char s[1024];               // Temporary.

move_t
board_best_move
(   const board_t*      board
,   const attacks_t*    attacks
,   const u32           depth
,   move_search_t*      args
)
{
    // Initialize search arguments.
    memory_copy ( &( *args ).board , board , sizeof ( board_t ) );
    memory_clear ( &( *args ).killer_moves , sizeof ( ( *args ).killer_moves ) );
    memory_clear ( &( *args ).history_moves , sizeof ( ( *args ).history_moves ) );
    memory_clear ( &( *args ).pv , sizeof ( ( *args ).pv ) );
    memory_clear ( &( *args ).pv_len , sizeof ( ( *args ).pv_len ) );
    ( *args ).pv_follow = false;
    ( *args ).pv_score = false;
    ( *args ).attacks = attacks;
    ( *args ).ply = 0;
    ( *args ).leaf_count = 0;
    ( *args ).move_count = 0;

    // Perform search with iterative deepening.
    for ( u32 i = 1; i <= depth; ++i )
    {
        ( *args ).pv_follow = true;
        negamax ( -50000 , 50000 , i , args );
        LOGDEBUG ( "BEST MOVE (%u): %s, CURRENT NODE COUNT: %u"
                 , i
                 , string_move ( s , ( *args ).pv[ 0 ][ 0 ] )
                 , ( *args ).leaf_count
                 );
    }

    // Best move.
    return ( *args ).pv[ 0 ][ 0 ];
}

i32
negamax
(   i32             alpha
,   i32             beta
,   u32             depth
,   move_search_t*  args
)
{
    // Initialize PV table.
    bool pv_found = false;
    ( *args ).pv_len[ ( *args ).ply ] = ( *args ).ply;

    // Base case.
    if ( !depth )
    {
        return quiescence ( alpha , beta , args );
    }

    // If search has gone too deep, stop recursing to prevent overflowing the
    // move tables.
    if ( ( *args ).ply >= MOVE_SEARCH_MAX_PLY )
    {
        return score_board ( &( *args ).board );
    }

    ( *args ).leaf_count += 1;

    // Check? Y/N
    const bool check = board_check ( &( *args ).board
                                   , ( *args ).attacks
                                   , ( *args ).board.side
                                   );
    if ( check )
    {
        depth += 1;
    }

    // Generate move options.
    moves_t moves;
    moves_compute ( &moves
                  , &( *args ).board
                  , ( *args ).attacks
                  );
    if ( ( *args ).pv_follow )
    {
        moves_enable_pv_scoring ( &moves , args );
    }
    moves_sort_by_score ( &moves
                        , args
                        );
    
    // Iterate over move options.
    for ( u8 i = 0; i < moves.count; ++i )
    {
        // Preserve board state.
        board_t board_prev;
        memory_copy ( &board_prev , &( *args ).board , sizeof ( board_t ) );
        ( *args ).ply += 1;
        
        // Perform next move.
        board_move ( &( *args ).board
                   , moves.moves[ i ]
                   , ( *args ).attacks
                   );

        // Filter the move if it put the moving side into check.
        if ( board_check ( &( *args ).board
                         , ( *args ).attacks
                         , !( *args ).board.side
                         ))
        {
            // Restore board state.
            memory_copy ( &( *args ).board , &board_prev , sizeof ( board_t ) );
            ( *args ).ply -= 1;
            continue;
        }
        ( *args ).move_count += 1;

        // Score the move.
        i32 score;
        if ( pv_found )
        {
            score = -negamax ( -alpha - 1 , -alpha , depth - 1 , args );
            if ( ( score > alpha ) && ( score < beta ) )
            {
                score = -negamax ( -beta , -alpha , depth - 1 , args );
            }
        }
        else
        {
            score = -negamax ( -beta , -alpha , depth - 1 , args );
        }

        // Restore board state.
        memory_copy ( &( *args ).board , &board_prev , sizeof ( board_t ) );
        ( *args ).ply -= 1;

        // Beta cutoff - no move found.
        if ( score >= beta )
        {
            // If move is quiet, update killer move table.
            if ( !move_decode_capture ( moves.moves[ i ] ) )
            {
                ( *args ).killer_moves[ 1 ][ ( *args ).ply ] = ( *args ).killer_moves[ 0 ][ ( *args ).ply ];
                ( *args ).killer_moves[ 0 ][ ( *args ).ply ] = moves.moves[ i ];
            }
            
            return beta;
        }

        // Alpha cutoff - new best move.
        if ( score > alpha )
        {
            // If move is quiet, update history move table.
            if ( !move_decode_capture ( moves.moves[ i ] ) )
            {
                ( *args ).history_moves[ move_decode_piece ( moves.moves[ i ] ) ][ move_decode_dst ( moves.moves[ i ] ) ] += depth;
            }

            alpha = score;

            // Update PV table.
            pv_found = true;
            ( *args ).pv[ ( *args ).ply ][ ( *args ).ply ] = moves.moves[ i ];
            for ( u32 j = ( *args ).ply + 1; j < ( *args ).pv_len[ ( *args ).ply + 1 ]; ++j )
            {
                ( *args ).pv[ ( *args ).ply ][ j ] = ( *args ).pv[ ( *args ).ply + 1 ][ j ];
            }
            ( *args ).pv_len[ ( *args ).ply ] = ( *args ).pv_len[ ( *args ).ply + 1 ];
        }
    }// END for.

    // No legal moves.
    if ( !( ( *args ).move_count ) )
    {
        if ( check ) // Checkmate.
        {       
            return -49000 + ( *args ).ply;
        }
        return 0;    // Stalemate.
    }

    return alpha;
}

i32
quiescence
(   i32             alpha
,   i32             beta
,   move_search_t*  args
)
{
    i32 score;

    ( *args ).leaf_count += 1;
    
    score = score_board ( &( *args ).board );

    // Beta cutoff - no move found.
    if ( score >= beta )
    {
        return beta;
    }

    // Alpha cutoff - new best move.
    if ( score > alpha )
    {
        alpha = score;
    }

    // Generate move options.
    moves_t moves;
    moves_sort_by_score ( moves_compute ( &moves
                                        , &( *args ).board
                                        , ( *args ).attacks
                                        )
                        , args
                        );
    
    for ( u8 i = 0; i < moves.count; ++i )
    {
        // Filter quiet moves.
        if ( !move_decode_capture ( moves.moves[ i ] ) )
        {
            continue;
        }

        // Preserve board state.
        board_t board_prev;
        memory_copy ( &board_prev , &( *args ).board , sizeof ( board_t ) );
        ( *args ).ply += 1;
        
        // Perform next capture.
        board_move ( &( *args ).board
                   , moves.moves[ i ]
                   , ( *args ).attacks
                   );

        // Filter the move if it put the moving side into check.
        if ( board_check ( &( *args ).board
                         , ( *args ).attacks
                         , !( *args ).board.side
                         ))
        {
            // Restore board state.
            memory_copy ( &( *args ).board , &board_prev , sizeof ( board_t ) );
            ( *args ).ply -= 1;
            continue;
        }
        ( *args ).move_count += 1;

        // Score the capture.
        score = -quiescence ( -beta , -alpha , args );

        // Restore board state.
        memory_copy ( &( *args ).board , &board_prev , sizeof ( board_t ) );
        ( *args ).ply -= 1;

        // Beta cutoff - no move found.
        if ( score >= beta )
        {
            return beta;
        }

        // Alpha cutoff - new best move.
        if ( score > alpha )
        {
            alpha = score;
        }
    }// END for.

    return alpha;
}

i32
score_board
(   const board_t* board
)
{
    i32 score = 0;

    bitboard_t bitboard;
    PIECE piece;
    SQUARE square;
    for ( PIECE piece_ = P; piece_ <= k; ++piece_ )
    {
        bitboard = ( *board ).pieces[ piece_ ];
        while ( bitboard )
        {
            piece = piece_;
            square = bitboard_lsb ( bitboard );

            score += material_scores[ piece ];
            switch ( piece )
            {
                case P: score += pawn_positional_scores[ square ]   ;break;
                case N: score += knight_positional_scores[ square ] ;break;
                case B: score += bishop_positional_scores[ square ] ;break;
                case R: score += rook_positional_scores[ square ]   ;break;
                case K: score += king_positional_scores[ square ]   ;break;

                case p: score -= pawn_positional_scores[ mirror_position[ square ] ]   ;break;
                case n: score -= knight_positional_scores[ mirror_position[ square ] ] ;break;
                case b: score -= bishop_positional_scores[ mirror_position[ square ] ] ;break;
                case r: score -= rook_positional_scores[ mirror_position[ square ] ]   ;break;
                case k: score -= king_positional_scores[ mirror_position[ square ] ]   ;break;

                default: break;
            }

            BITCLR ( bitboard , square );
        }
    }

    return ( ( *board ).side == WHITE ) ? score : -score;
}

i32
score_move
(   const move_t    move
,   move_search_t*  args
)
{
    // PV scoring enabled? Y/N
    if ( ( *args ).pv_score )
    {
        if ( ( *args ).pv[ 0 ][ ( *args ).ply ] == move )
        {
            ( *args ).pv_score = false;
            return 20000;
        }
    }

    // Quiet.
    if ( !move_decode_capture ( move ) )
    {
        if ( ( *args ).killer_moves[ 0 ][ ( *args ).ply ] == move )
        {
            return 9000;
        }
        if ( ( *args ).killer_moves[ 1 ][ ( *args ).ply ] == move )
        {
            return 8000;
        }
        return ( *args ).history_moves[ move_decode_piece ( move ) ][ move_decode_dst ( move ) ];
    }
    
    // Capture.
    PIECE target = P;
    const PIECE start = ( ( *args ).board.side == WHITE ) ? p : P;
    const PIECE end = ( ( *args ).board.side == WHITE ) ? k : K;
    for ( PIECE piece = start; piece <= end; ++piece )
    {
        if ( bit ( ( *args ).board.pieces[ piece ]
                 , move_decode_dst ( move )
                 ))
        {
            target = piece;
            break;
        }
    }
    return 10000 + mvv_lva[ move_decode_piece ( move ) ][ target ];
}

moves_t*
moves_sort_by_score
(   moves_t*        moves
,   move_search_t*  args
)
{
    u32 i;
    u32 j;
    move_t key;

    i = 0;
    while ( i < ( *moves ).count )
    {
        key = ( *moves ).moves[ i ];
        j = i;
        while ( j && score_move ( ( *moves ).moves[ j - 1 ], args ) < score_move ( key , args ) )
        {
            ( *moves ).moves[ j ] = ( *moves ).moves[ j - 1 ];
            j -= 1;
        }
        ( *moves ).moves[ j ] = key;
        i += 1;
    }
    return moves;
}

moves_t*
moves_enable_pv_scoring
(   moves_t*        moves
,   move_search_t*  args
)
{
    ( *args ).pv_follow = false;
    for ( u32 i = 0; i < ( *moves ).count; ++i )
    {
        if ( ( *args ).pv[ 0 ][ ( *args ).ply ] != ( *moves ).moves[ i ] )
        {
            continue;
        }
        ( *args ).pv_follow = true;
        ( *args ).pv_score = true;
    }
    return moves;
}