/**
 * @file negamax.c
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Implementation of the negamax header.
 * (see negamax.h for additional details)
 */
#include "chess/negamax.h"

#include "chess/board.h"
#include "chess/score.h"

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
 * @param board Current chess board state.
 * @param alpha Current alpha negamax cutoff.
 * @param beta Current beta negamax cutoff.
 * @param depth Current recursion depth.
 * @param attacks The pregenerated attack tables.
 * @param move Output buffer to hold best move.
 * @param leaf_count Output buffer to hold leaf node count.
 * @return Current negamax score.
 */
i32
_negamax
(   board_t*            board
,   i32                 alpha
,   i32                 beta
,   const u32           depth
,   const attacks_t*    attacks
,   move_t*             best
,   u32*                leaf_count
,   u32*                root
);

move_t
negamax
(   const board_t*      board_
,   const i32           alpha
,   const i32           beta
,   const u32           depth
,   const attacks_t*    attacks
)
{
    board_t board;
    move_t move;
    u32 leaf_count = 0;
    u32 root = 0;
    _negamax ( memory_copy ( &board , board_ , sizeof ( board_t ) )
             , alpha
             , beta
             , depth
             , attacks
             , &move
             , &leaf_count
             , &root
             );
    return move;
}

i32
_negamax
(   board_t*            board
,   i32                 alpha
,   i32                 beta
,   const u32           depth
,   const attacks_t*    attacks
,   move_t*             best
,   u32*                leaf_count
,   u32*                root
)
{
    // Base case.
    if ( !depth )
    {
        return negamax_score ( board );
    }

    *leaf_count += 1;

    // Generate move options.
    moves_t moves;
    moves_compute ( &moves , board , attacks );
    
    move_t best_ = moves.moves[ 0 ];
    i32 alpha_ = alpha;
    for ( u8 i = 0; i < moves.count; ++i )
    {
        // Preserve board state.
        board_t board_prev;
        memory_copy ( &board_prev , board , sizeof ( board_t ) );
        
        *root += 1;
        
        // Score next move, if it is valid.
        board_move ( board
                   , moves.moves[ i ]
                   , attacks
                   );
        if ( board_check ( board , attacks , ( *board ).side ) )
        {
            // Restore board state.
            memory_copy ( board , &board_prev , sizeof ( board_t ) );

            *root -= 1;

            continue;
        }

        const i32 score = -_negamax ( board
                                    , -beta
                                    , -alpha
                                    , depth - 1
                                    , attacks
                                    , best
                                    , leaf_count
                                    , root
                                    );

        // Restore the board state.
        memory_copy ( board , &board_prev , sizeof ( board_t ) );

        *root -= 1;

        // Beta cutoff - no move found.
        if ( score >= beta )
        {
            return beta;
        }

        // Alpha cutoff - new best move.
        if ( score > alpha )
        {
            alpha = score;
            if ( !( *root ) )
            {
                best_ = moves.moves[ i ];
            }
        }
    }

    // Write new best move to output buffer.
    if ( alpha != alpha_ )
    {
        *best = best_;
    }

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