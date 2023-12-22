/**
 * @file negamax.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Implementation of the negamax header.
 * (see negamax.h for additional details)
 */
#include "chess/negamax.h"

#include "chess/board.h"
#include "chess/score.h"

// Type definition for a container to hold negamax function parameters.
typedef struct
{
    board_t             board;
    move_t              move;
    u32                 ply;
    u32                 leaf_count;
    u32                 move_count;
    const attacks_t*    attacks;
}
negamax_t;

/**
 * @brief Quiescence search.
 * @param alpha Alpha negamax cutoff.
 * @param beta Beta negamax cutoff.
 * @param args Static function arguments.
 * @return Current negamax score.
 */
i32
negamax_quiescence
(   i32         alpha
,   i32         beta
,   negamax_t*  args
);

/**
 * @brief Negamax board evaluation function.
 * @param board A chess board state.
 * @return A negamax score corresponding to the board state.
 */
i32
negamax_score
(   const board_t* board
);

/**
 * @brief Primary implementation of negamax (see negamax).
 * @param alpha Alpha negamax cutoff.
 * @param beta Beta negamax cutoff.
 * @param depth Current recursion depth.
 * @param args Static function arguments.
 * @return Current negamax score.
 */
i32
_negamax
(   i32         alpha
,   i32         beta
,   u32         depth
,   negamax_t*  args
);

move_t
negamax
(   const board_t*      board
,   const i32           alpha
,   const i32           beta
,   const u32           depth
,   const attacks_t*    attacks
)
{
    negamax_t args;
    memory_copy ( &args.board , board , sizeof ( board_t ) );
    args.ply = 0;
    args.leaf_count = 0;
    args.move_count = 0;
    args.attacks = attacks;
    _negamax ( alpha , beta , depth , &args );
    return args.move;
}

i32
_negamax
(   i32         alpha
,   i32         beta
,   u32         depth
,   negamax_t*  args
)
{
    // Base case.
    if ( !depth )
    {
        return negamax_quiescence ( alpha , beta , args );
    }

    ( *args ).leaf_count += 1;

    // Check? Y/N
    const bool check = board_check ( &( *args ).board
                                   , ( *args ).attacks
                                   , ( *args ).board.side
                                   );

    // Generate move options.
    moves_t moves;
    moves_compute ( &moves , &( *args ).board , ( *args ).attacks );
    
    move_t best = moves.moves[ 0 ]; // Default.
    i32 alpha_ = alpha;
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
        const i32 score = -_negamax ( -beta , -alpha , depth - 1 , args );

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
            if ( !( ( *args ).ply ) )
            {
                best = moves.moves[ i ];
            }
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

    // Write new best move to output buffer.
    if ( alpha != alpha_ )
    {
        ( *args ).move = best;
    }

    return alpha;
}

i32
negamax_quiescence
(   i32         alpha
,   i32         beta
,   negamax_t*  args
)
{
    i32 score;
    
    score = negamax_score ( &( *args ).board );

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

    // Generate capture options.
    moves_t moves;
    moves_filter ( moves_compute ( &moves
                                 , &( *args ).board
                                 , ( *args ).attacks
                                 )
                 , MOVE_FILTER_ONLY_CAPTURE
                 , &moves
                 );
    
    for ( u8 i = 0; i < moves.count; ++i )
    {
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
        score = -negamax_quiescence ( -beta , -alpha , args );

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
negamax_score
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