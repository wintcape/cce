/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file board.c
 * @brief Implementation of the board header.
 * (see board.h for additional details)
 */
#include "chess/board.h"

#include "chess/castle.h"
#include "chess/negamax.h"

bool
board_checkmate
(   const board_t*      board_
,   const attacks_t*    attacks
,   const moves_t*      moves
)
{
    if ( !board_check ( board_ , attacks , ( *board_ ).side ) )
    {
        return false;
    }

    board_t board;
    memory_copy ( &board , board_ , sizeof ( board_t ) );
    for ( u32 i = 0; i < ( *moves ).count; ++i )
    {
        board_move ( &board , ( *moves ).moves[ i ] , attacks );
        if ( !board_check ( &board , attacks , ( *board_ ).side ) )
        {
            return false;
        }
    }
    return true;
}

void
board_move
(   board_t*            board_
,   const move_t        move
,   const attacks_t*    attacks
)
{
    const SQUARE src = move_decode_src ( move );
    const SQUARE dst = move_decode_dst ( move );
    const PIECE piece = move_decode_piece ( move );
    const PIECE promotion = move_decode_promotion ( move );
    const bool capture = move_decode_capture ( move );
    const bool double_push = move_decode_double_push ( move );
    const bool enpassant = move_decode_enpassant ( move );
    const bool castle = move_decode_castle ( move );

    // Initialize a working copy of the board.
    board_t board;
    memory_copy ( &board , board_ , sizeof ( board_t ) );
    
    const bool white = board.side == WHITE;
    
    // Move the piece.
    BITCLR ( board.pieces[ piece ] , src );
    BITSET ( board.pieces[ piece ] , dst );

    // Parse capture.
    if ( capture )
    {
        const PIECE from = ( white ) ? p : P;
        const PIECE to = ( white ) ? k : K;
        for ( PIECE i = from; i <= to; ++i )
        {
            if ( bit ( board.pieces[ i ] , dst ) )
            {
                board.capture = i;
                BITCLR ( board.pieces[ i ] , dst );
                break;
            }
        }
    }

    // Parse promotion.
    if ( promotion )
    {
        // Clear pawn.
        BITCLR ( board.pieces[ ( white ) ? P : p ] , dst );

        // Set promotion.
        BITSET ( board.pieces[ promotion ] , dst );
    }

    // Parse en passant capture.
    if ( enpassant )
    {
        if ( white )
        {
            board.capture = p;
            BITCLR ( board.pieces[ p ] , dst + 8 );
        }
        else
        {
            board.capture = P;
            BITCLR ( board.pieces[ P ] , dst - 8 );
        }
    }

    // Reset en passant square.
    board.enpassant = NO_SQ;

    // Parse double push.
    if ( double_push )
    {
        board.enpassant = ( white ) ? dst + 8 : dst - 8;
    }

    // Parse castling.
    if ( castle )
    {
        switch ( dst )
        {
            case C1:
            {
                BITCLR ( board.pieces[ R ] , A1 );
                BITSET ( board.pieces[ R ] , D1 );
            }
            break;

            case G1:
            {
                BITCLR ( board.pieces[ R ] , H1 );
                BITSET ( board.pieces[ R ] , F1 );
            }
            break;

            case C8:
            {
                BITCLR ( board.pieces[ r ] , A8 );
                BITSET ( board.pieces[ r ] , D8 );
            }
            break;

            case G8:
            {
                BITCLR ( board.pieces[ r ] , H8 );
                BITSET ( board.pieces[ r ] , F8 );
            }
            break;

            default:
            {}
            break;
        }
    }

    // Update castling rights.
    board.castle &= castling_rights[ src ];
    board.castle &= castling_rights[ dst ];

    // Update occupancy maps.
    memory_clear ( board.occupancies , sizeof ( board.occupancies ) );
    for ( PIECE piece = P; piece <= K; ++piece )
    {
        board.occupancies[ WHITE ] |= board.pieces[ piece ];
    }
    for ( PIECE piece = p; piece <= k; ++piece )
    {
        board.occupancies[ BLACK ] |= board.pieces[ piece ];
    }
    board.occupancies[ 2 ] = board.occupancies[ WHITE ]
                           | board.occupancies[ BLACK ]
                           ;

    // Toggle side.
    board.side = !board.side;

    // Overwrite the output buffer with the updated board.
    memory_copy ( board_ , &board , sizeof ( board_t ) );
}

move_t
board_best_move
(   const board_t*      board
,   const attacks_t*    attacks
,   const u32           depth
)
{
    return negamax ( board , -50000 , 50000 , depth , attacks );
}