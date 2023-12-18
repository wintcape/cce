/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file move.c
 * @brief Implementation of the move header.
 * (see move.h for additional details)
 */
#include "chess/move.h"

#include "chess/board.h"
#include "chess/castle.h"
#include "chess/string.h"

#include "core/logger.h"
#include "core/memory.h"

bool
move_parse
(   const move_t        move
,   const MOVE_FILTER   filter
,   const attacks_t*    attacks
,   board_t*            board_
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
    
    // Apply filter.
    switch ( filter )
    {
        case MOVE_FILTER_ONLY_PAWN:
            return ( piece == 'P' || piece == 'p' ) ? move_parse ( move
                                                                , MOVE_FILTER_NONE
                                                                , attacks
                                                                , board_
                                                                )
                                                    : false
                                                    ;
        case MOVE_FILTER_ONLY_KNIGHT:
            return ( piece == 'N' || piece == 'n' ) ? move_parse ( move
                                                                , MOVE_FILTER_NONE
                                                                , attacks
                                                                , board_
                                                                )
                                                    : false
                                                    ;
        case MOVE_FILTER_ONLY_BISHOP:
            return ( piece == 'B' || piece == 'b' ) ? move_parse ( move
                                                                , MOVE_FILTER_NONE
                                                                , attacks
                                                                , board_
                                                                )
                                                    : false
                                                    ;
        case MOVE_FILTER_ONLY_ROOK:
            return ( piece == 'R' || piece == 'r' ) ? move_parse ( move
                                                                , MOVE_FILTER_NONE
                                                                , attacks
                                                                , board_
                                                                )
                                                    : false
                                                    ;
        case MOVE_FILTER_ONLY_QUEEN:
            return ( piece == 'Q' || piece == 'q' ) ? move_parse ( move
                                                                , MOVE_FILTER_NONE
                                                                , attacks
                                                                , board_
                                                                )
                                                    : false
                                                    ;
        case MOVE_FILTER_ONLY_KING:
            return ( piece == 'K' || piece == 'k' ) ? move_parse ( move
                                                                , MOVE_FILTER_NONE
                                                                , attacks
                                                                , board_
                                                                )
                                                    : false
                                                    ;
        case MOVE_FILTER_ONLY_CAPTURE:
            return ( capture ) ? move_parse ( move
                                            , MOVE_FILTER_NONE
                                            , attacks
                                            , board_
                                            )
                               : false
                               ;
        case MOVE_FILTER_ONLY_PROMOTION:
            return ( promotion ) ? move_parse ( move
                                              , MOVE_FILTER_NONE
                                              , attacks
                                              , board_
                                              )
                               : false
                               ;
        case MOVE_FILTER_ONLY_DOUBLE_PUSH:
            return ( double_push ) ? move_parse ( move
                                                , MOVE_FILTER_NONE
                                                , attacks
                                                , board_
                                                )
                                   : false
                                   ;
        case MOVE_FILTER_ONLY_ENPASSANT:
            return ( enpassant ) ? move_parse ( move
                                              , MOVE_FILTER_NONE
                                              , attacks
                                              , board_
                                              )
                                 : false
                                 ;
        case MOVE_FILTER_ONLY_CASTLE:
            return ( castle ) ? move_parse ( move
                                           , MOVE_FILTER_NONE
                                           , attacks                                           
                                           , board_
                                           )
                              : false
                              ;
        default:
            break;
    }

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
        ( white ) ? ( board.enpassant = dst + 8 )
                  : ( board.enpassant = dst - 8 )
                  ;
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
    board.side ^= 1;

    // If new side's king is in check, discard the working board.
    if ( board_square_attackable ( &board
                                 , attacks
                                 , bitboard_lsb ( ( !white ) ? board.pieces[ k ]
                                                             : board.pieces[ K ]
                                                             )
                                 , board.side
                                 ))
    {
        return false;
    }
    
    // Update history.
    board.history += 1;

    // Overwrite the output buffer with the updated board.
    memory_copy ( board_ , &board , sizeof ( board_t ) );

    return true;
}

INLINE
void
moves_push
(   moves_t*        moves
,   const move_t    move
)
{
    ( *moves ).moves[ ( *moves ).count ] = move;
    ( *moves ).count += 1;
}

void
moves_compute
(   moves_t*            moves
,   const board_t*      board
,   const attacks_t*    attacks
)
{
    SQUARE src;
    SQUARE dst;

    bitboard_t pieces;
    bitboard_t attack;

    ( *moves ).count = 0;

    for ( PIECE piece = P; piece <= k; ++piece )
    {
        pieces = ( *board ).pieces[ piece ];

        // White pawn and castling moves.
        if ( ( *board ).side == WHITE )
        {
            // Pawn.
            if ( piece == P )
            {
                while ( pieces )
                {
                    src = bitboard_lsb ( pieces );
                    dst = src - 8;

                    // Quiet moves.
                    if ( !( dst < A8 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                  , dst
                                                  ))
                    {
                        // Promotion.
                        if ( src >= A7 && src <= H7 )
                        {
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , Q , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , R , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , B , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , N , 0 , 0 , 0 , 0 )
                                       );
                        }
                        else
                        {
                            // Push.
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , 0 , 0 , 0 , 0 , 0 )
                                       );

                            // Double push.
                            if ( ( src >= A2 && src <= H2 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                                    , dst - 8
                                                                    ))
                            {
                                moves_push ( moves
                                           , move_encode ( src , dst - 8 , piece , 0 , 0 , 1 , 0 , 0 )
                                           );
                            }
                        }
                    }

                    // Capture moves.
                    attack = bitboard_pawn_attack ( attacks
                                                  , src
                                                  , ( *board ).side
                                                  )
                           & ( *board ).occupancies[ BLACK ]
                           ;
                    while ( attack )
                    {
                        dst = bitboard_lsb ( attack );

                        // Promotion + capture.
                        if ( src >= A7 && src <= H7 )
                        {
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , Q , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , R , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , B , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , N , 1 , 0 , 0 , 0 )
                                       );
                        }
                        // Push + capture.
                        else
                        {
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , 0 , 1 , 0 , 0 , 0 )
                                       );
                        }

                        BITCLR ( attack , dst );
                    }

                    // En passant captures.
                    if ( ( *board ).enpassant != NO_SQ )
                    {
                        bitboard_t enpassant = bitboard_pawn_attack ( attacks
                                                                    , src
                                                                    , ( *board ).side
                                                                    )
                                             & bitset ( 0 , ( *board ).enpassant )
                                             ;
                        if ( enpassant )
                        {
                            const SQUARE dst_enpassant = bitboard_lsb ( enpassant );
                            moves_push ( moves
                                       , move_encode ( src , dst_enpassant , piece , 0 , 1 , 0 , 1 , 0 )
                                       );
                            
                        }
                    }

                    BITCLR ( pieces , src );
                }
            }
            
            // Castling moves.
            else if ( piece == K )
            {
                if ( ( *board ).castle & CASTLE_WK )
                {
                    if (   !bit ( ( *board ).occupancies[ 2 ] , F1 )
                        && !bit ( ( *board ).occupancies[ 2 ] , G1 )
                       )
                    {
                        if (   !board_square_attackable ( board , attacks , E1 , BLACK )
                            && !board_square_attackable ( board , attacks , F1 , BLACK )
                           )
                        {
                            moves_push ( moves
                                       , move_encode ( E1 , G1 , piece , 0 , 0 , 0 , 0 , 1 )
                                       );
                        }
                    }
                }
                if ( ( *board ).castle & CASTLE_WQ )
                {
                    if (   !bit ( ( *board ).occupancies[ 2 ] , D1 )
                        && !bit ( ( *board ).occupancies[ 2 ] , C1 )
                        && !bit ( ( *board ).occupancies[ 2 ] , B1 )
                       )
                    {
                        if (   !board_square_attackable ( board , attacks , E1 , BLACK )
                            && !board_square_attackable ( board , attacks , D1 , BLACK )
                           )
                        {
                            moves_push ( moves
                                       , move_encode ( E1 , C1 , piece , 0 , 0 , 0 , 0 , 1 )
                                       );
                        }
                    }
                }
            }
        }
        
        // Black pawn and castling moves.
        else
        {
            // Pawn.
            if ( piece == p )
            {
                while ( pieces )
                {
                    src = bitboard_lsb ( pieces );
                    dst = src + 8;

                    // Quiet moves.
                    if ( !( dst > H1 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                  , dst
                                                  ))
                    {
                        // Promotion.
                        if ( src >= A2 && src <= H2 )
                        {
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , q , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , r , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , b , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , n , 0 , 0 , 0 , 0 )
                                       );
                        }
                        else
                        {
                            // Push.
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , 0 , 0 , 0 , 0 , 0 )
                                       );

                            // Double push.
                            if ( ( src >= A7 && src <= H7 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                                    , dst + 8
                                                                    ))
                            {
                                moves_push ( moves
                                           , move_encode ( src , dst + 8 , piece , 0 , 0 , 1 , 0 , 0 )
                                           );
                            }
                        }
                    }

                    // Capture moves.
                    attack = bitboard_pawn_attack ( attacks
                                                  , src
                                                  , ( *board ).side
                                                  )
                           & ( *board ).occupancies[ WHITE ]
                           ;
                    while ( attack )
                    {
                        dst = bitboard_lsb ( attack );

                        // Promotion + capture.
                        if ( src >= A2 && src <= H2 )
                        {
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , q , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , r , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , b , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , n , 1 , 0 , 0 , 0 )
                                       );
                        }
                        // Push + capture.
                        else
                        {
                            moves_push ( moves
                                       , move_encode ( src , dst , piece , 0 , 1 , 0 , 0 , 0 )
                                       );
                        }

                        BITCLR ( attack , dst );
                    }

                    // En passant captures.
                    if ( ( *board ).enpassant != NO_SQ )
                    {
                        bitboard_t enpassant = bitboard_pawn_attack ( attacks
                                                                    , src
                                                                    , ( *board ).side
                                                                    )
                                             & bitset ( 0 , ( *board ).enpassant )
                                             ;
                        if ( enpassant )
                        {
                            const SQUARE dst_enpassant = bitboard_lsb ( enpassant );
                            moves_push ( moves
                                       , move_encode ( src , dst_enpassant , piece , 0 , 1 , 0 , 1 , 0 )
                                       );
                            
                        }
                    }

                    BITCLR ( pieces , src );
                }
            }

            // Castling moves.
            else if ( piece == k )
            {
                if ( ( *board ).castle & CASTLE_BK )
                {
                    if (   !bit ( ( *board ).occupancies[ 2 ] , F8 )
                        && !bit ( ( *board ).occupancies[ 2 ] , G8 )
                       )
                    {
                        if (   !board_square_attackable ( board , attacks , E8 , WHITE )
                            && !board_square_attackable ( board , attacks , F8 , WHITE )
                           )
                        {
                            moves_push ( moves
                                       , move_encode ( E8 , G8 , piece , 0 , 0 , 0 , 0 , 1 )
                                       );
                        }
                    }
                }
                if ( ( *board ).castle & CASTLE_BQ )
                {
                    if (   !bit ( ( *board ).occupancies[ 2 ] , D8 )
                        && !bit ( ( *board ).occupancies[ 2 ] , C8 )
                        && !bit ( ( *board ).occupancies[ 2 ] , B8 )
                       )
                    {
                        if (   !board_square_attackable ( board , attacks , E8 , WHITE )
                            && !board_square_attackable ( board , attacks , D8 , WHITE )
                           )
                        {
                            moves_push ( moves
                                       , move_encode ( E8 , C8 , piece , 0 , 0 , 0 , 0 , 1 )
                                       );
                        }
                    }
                }
            }
        }

        // Knight.
        if ( ( ( *board ).side == WHITE ) ? piece == N : piece == n )
        {
            while ( pieces )
            {
                src = bitboard_lsb ( pieces );
               
                attack = bitboard_knight_attack ( attacks , src )
                       & ( ( ( *board ).side == WHITE ) ? ~( *board ).occupancies[ WHITE ]
                                                        : ~( *board ).occupancies[ BLACK ]
                                                        );
                while ( attack )
                {
                    dst = bitboard_lsb ( attack );
 
                    // Quiet move.
                    if ( !bit ( ( ( *board ).side == WHITE ) ? ( *board ).occupancies[ BLACK ]
                                                             : ( *board ).occupancies[ WHITE ]
                              , dst
                              ))
                    {
                        moves_push ( moves
                                   , move_encode ( src , dst , piece , 0 , 0 , 0 , 0 , 0 )
                                   );
                    }

                    // Capture move.
                    else
                    { 
                        moves_push ( moves
                                   , move_encode ( src , dst , piece , 0 , 1 , 0 , 0 , 0 )
                                   );
                    }

                    BITCLR ( attack , dst );
                }

                BITCLR ( pieces , src );
            }
        }
        
        // Bishop.
        else if ( ( ( *board ).side == WHITE ) ? piece == B : piece == b )
        {
            while ( pieces )
            {
                src = bitboard_lsb ( pieces );
               
                attack = bitboard_bishop_attack ( attacks
                                                , src
                                                , ( *board ).occupancies[ 2 ]
                                                )
                       & ( ( ( *board ).side == WHITE ) ? ~( *board ).occupancies[ WHITE ]
                                                        : ~( *board ).occupancies[ BLACK ]
                                                        );
                while ( attack )
                {
                    dst = bitboard_lsb ( attack );
 
                    // Quiet move.
                    if ( !bit ( ( ( *board ).side == WHITE ) ? ( *board ).occupancies[ BLACK ]
                                                             : ( *board ).occupancies[ WHITE ]
                              , dst
                              ))
                    {
                        moves_push ( moves
                                   , move_encode ( src , dst , piece , 0 , 0 , 0 , 0 , 0 )
                                   );
                    }

                    // Capture move.
                    else
                    { 
                        moves_push ( moves
                                   , move_encode ( src , dst , piece , 0 , 1 , 0 , 0 , 0 )
                                   );
                    }

                    BITCLR ( attack , dst );
                }

                BITCLR ( pieces , src );
            }
        }
        
        // Rook.
        else if ( ( ( *board ).side == WHITE ) ? piece == R : piece == r )
        {
            while ( pieces )
            {
                src = bitboard_lsb ( pieces );
               
                attack = bitboard_rook_attack ( attacks
                                              , src
                                              , ( *board ).occupancies[ 2 ]
                                              )
                       & ( ( ( *board ).side == WHITE ) ? ~( *board ).occupancies[ WHITE ]
                                                        : ~( *board ).occupancies[ BLACK ]
                                                        );
                while ( attack )
                {
                    dst = bitboard_lsb ( attack );
 
                    // Quiet move.
                    if ( !bit ( ( ( *board ).side == WHITE ) ? ( *board ).occupancies[ BLACK ]
                                                             : ( *board ).occupancies[ WHITE ]
                              , dst
                              ))
                    {
                        moves_push ( moves
                                   , move_encode ( src , dst , piece , 0 , 0 , 0 , 0 , 0 )
                                   );
                    }

                    // Capture move.
                    else
                    { 
                        moves_push ( moves
                                   , move_encode ( src , dst , piece , 0 , 1 , 0 , 0 , 0 )
                                   );
                    }

                    BITCLR ( attack , dst );
                }

                BITCLR ( pieces , src );
            }
        }

        // Queen.
        else if ( ( ( *board ).side == WHITE ) ? piece == Q : piece == q )
        {
            while ( pieces )
            {
                src = bitboard_lsb ( pieces );
               
                attack = bitboard_queen_attack ( attacks
                                               , src
                                               , ( *board ).occupancies[ 2 ]
                                               )
                       & ( ( ( *board ).side == WHITE ) ? ~( *board ).occupancies[ WHITE ]
                                                        : ~( *board ).occupancies[ BLACK ]
                                                        );
                while ( attack )
                {
                    dst = bitboard_lsb ( attack );
 
                    // Quiet move.
                    if ( !bit ( ( ( *board ).side == WHITE ) ? ( *board ).occupancies[ BLACK ]
                                                             : ( *board ).occupancies[ WHITE ]
                              , dst
                              ))
                    {
                        moves_push ( moves
                                   , move_encode ( src , dst , piece , 0 , 0 , 0 , 0 , 0 )
                                   );
                    }

                    // Capture move.
                    else
                    { 
                        moves_push ( moves
                                   , move_encode ( src , dst , piece , 0 , 1 , 0 , 0 , 0 )
                                   );
                    }

                    BITCLR ( attack , dst );
                }

                BITCLR ( pieces , src );
            }
        }

        // King.
        else if ( ( ( *board ).side == WHITE ) ? piece == K : piece == k )
        {
            while ( pieces )
            {
                src = bitboard_lsb ( pieces );
               
                attack = bitboard_king_attack ( attacks , src )
                       & ( ( ( *board ).side == WHITE ) ? ~( *board ).occupancies[ WHITE ]
                                                        : ~( *board ).occupancies[ BLACK ]
                                                        );
                while ( attack )
                {
                    dst = bitboard_lsb ( attack );
 
                    // Quiet move.
                    if ( !bit ( ( ( *board ).side == WHITE ) ? ( *board ).occupancies[ BLACK ]
                                                             : ( *board ).occupancies[ WHITE ]
                              , dst
                              ))
                    {
                        moves_push ( moves
                                   , move_encode ( src , dst , piece , 0 , 0 , 0 , 0 , 0 )
                                   );
                    }

                    // Capture move.
                    else
                    { 
                        moves_push ( moves
                                   , move_encode ( src , dst , piece , 0 , 1 , 0 , 0 , 0 )
                                   );
                    }

                    BITCLR ( attack , dst );
                }

                BITCLR ( pieces , src );
            }
        }

    }// END for.
}
