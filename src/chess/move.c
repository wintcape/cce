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

bool
move_parse
(   const move_t        move
,   const MOVE_FILTER   filter
,   const attacks_t*    attacks
,   board_t*            board_
)
{
    const SQUARE src = move_decode_source_square ( move );
    const SQUARE target = move_decode_target_square ( move );
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
    BITSET ( board.pieces[ piece ] , target );

    // Parse capture.
    if ( capture )
    {
        const PIECE from = ( white ) ? p : P;
        const PIECE to = ( white ) ? k : K;
        for ( PIECE i = from; i <= to; ++i )
        {
            if ( bit ( board.pieces[ i ] , target ) )
            {
                // Clear piece on target square.
                BITCLR ( board.pieces[ i ] , target );
                break;
            }
        }
    }

    // Parse promotion.
    if ( promotion )
    {
        // Clear pawn.
        BITCLR ( board.pieces[ ( white ) ? P : p ] , target );

        // Set promotion.
        BITSET ( board.pieces[ promotion ] , target );
    }

    // Parse en passant capture.
    if ( enpassant )
    {
        ( white ) ? BITCLR ( board.pieces[ p ] , target + 8 )
                  : BITCLR ( board.pieces[ P ] , target - 8 )
                  ;
    }

    // Reset en passant square.
    board.enpassant = NO_SQ;

    // Parse double push.
    if ( double_push )
    {
        ( white ) ? ( board.enpassant = target + 8 )
                  : ( board.enpassant = target - 8 )
                  ;
    }

    // Parse castling.
    if ( castle )
    {
        switch ( target )
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
                BITCLR ( board.pieces[ R ] , A8 );
                BITSET ( board.pieces[ R ] , D8 );
            }
            break;

            case G8:
            {
                BITCLR ( board.pieces[ R ] , H8 );
                BITSET ( board.pieces[ R ] , F8 );
            }
            break;

            default:
            {}
            break;
        }
    }

    // Update castling rights.
    board.castle &= castling_rights[ src ];
    board.castle &= castling_rights[ target ];

    // Write the new occupancy maps to the board.
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

    // If king is in check, do not perform the move.
    if ( board_square_attackable ( &board
                                 , attacks
                                 , bitboard_lsb ( ( white ) ? board.pieces[ K ]
                                                            : board.pieces[ k ]
                                                            )
                                 , board.side
                                 ))
    {
        return false;
    }

    // Overwrite the output buffer with the updated board.
    memory_copy ( board_ , &board , sizeof ( board_t ) );

    return true;
}

void
moves_get
(   moves_t*            moves
,   board_t*            board
,   const attacks_t*    attacks
)
{
    SQUARE src;
    SQUARE target;

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
                    target = src - 8;

                    // Quiet moves.
                    if ( !( target < A8 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                  , target
                                                  ))
                    {
                        // Promotion.
                        if ( src >= A7 && src <= H7 )
                        {
                            moves_push ( moves
                                       , move_encode ( src , target , piece , Q , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , R , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , B , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , N , 0 , 0 , 0 , 0 )
                                       );
                        }
                        else
                        {
                            // Push.
                            moves_push ( moves
                                       , move_encode ( src , target , piece , 0 , 0 , 0 , 0 , 0 )
                                       );

                            // Double push.
                            if ( ( src >= A2 && src <= H2 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                                    , target - 8
                                                                    ))
                            {
                                moves_push ( moves
                                           , move_encode ( src , target - 8 , piece , 0 , 0 , 1 , 0 , 0 )
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
                        target = bitboard_lsb ( attack );

                        // Promotion + capture.
                        if ( src >= A7 && src <= H7 )
                        {
                            moves_push ( moves
                                       , move_encode ( src , target , piece , Q , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , R , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , B , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , N , 1 , 0 , 0 , 0 )
                                       );
                        }
                        // Push + capture.
                        else
                        {
                            moves_push ( moves
                                       , move_encode ( src , target , piece , 0 , 1 , 0 , 0 , 0 )
                                       );
                        }

                        BITCLR ( attack , target );
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
                            const SQUARE target_enpassant = bitboard_lsb ( enpassant );
                            moves_push ( moves
                                       , move_encode ( src , target_enpassant , piece , 0 , 1 , 0 , 1 , 0 )
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
                            && !board_square_attackable ( board , attacks , C1 , BLACK )
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
                    target = src + 8;

                    // Quiet moves.
                    if ( !( target > H1 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                  , target
                                                  ))
                    {
                        // Promotion.
                        if ( src >= A2 && src <= H2 )
                        {
                            moves_push ( moves
                                       , move_encode ( src , target , piece , q , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , r , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , b , 0 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , n , 0 , 0 , 0 , 0 )
                                       );
                        }
                        else
                        {
                            // Push.
                            moves_push ( moves
                                       , move_encode ( src , target , piece , 0 , 0 , 0 , 0 , 0 )
                                       );

                            // Double push.
                            if ( ( src >= A7 && src <= H7 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                                    , target + 8
                                                                    ))
                            {
                                moves_push ( moves
                                           , move_encode ( src , target + 8 , piece , 0 , 0 , 1 , 0 , 0 )
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
                        target = bitboard_lsb ( attack );

                        // Promotion + capture.
                        if ( src >= A7 && src <= H7 )
                        {
                            moves_push ( moves
                                       , move_encode ( src , target , piece , q , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , r , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , b , 1 , 0 , 0 , 0 )
                                       );
                            moves_push ( moves
                                       , move_encode ( src , target , piece , n , 1 , 0 , 0 , 0 )
                                       );
                        }
                        // Push + capture.
                        else
                        {
                            moves_push ( moves
                                       , move_encode ( src , target , piece , 0 , 1 , 0 , 0 , 0 )
                                       );
                        }

                        BITCLR ( attack , target );
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
                            const SQUARE target_enpassant = bitboard_lsb ( enpassant );
                            moves_push ( moves
                                       , move_encode ( src , target_enpassant , piece , 0 , 1 , 0 , 1 , 0 )
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
                if ( ( *board ).castle & CASTLE_WQ )
                {
                    if (   !bit ( ( *board ).occupancies[ 2 ] , D8 )
                        && !bit ( ( *board ).occupancies[ 2 ] , C8 )
                        && !bit ( ( *board ).occupancies[ 2 ] , B8 )
                       )
                    {
                        if (   !board_square_attackable ( board , attacks , E8 , WHITE )
                            && !board_square_attackable ( board , attacks , C8 , WHITE )
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
                    target = bitboard_lsb ( attack );
 
                    // Quiet move.
                    if ( !bit ( ( ( *board ).side == WHITE ) ? ( *board ).occupancies[ BLACK ]
                                                             : ( *board ).occupancies[ WHITE ]
                              , target
                              ))
                    {
                        moves_push ( moves
                                   , move_encode ( src , target , piece , 0 , 0 , 0 , 0 , 0 )
                                   );
                    }

                    // Capture move.
                    else
                    { 
                        moves_push ( moves
                                   , move_encode ( src , target , piece , 0 , 1 , 0 , 0 , 0 )
                                   );
                    }

                    BITCLR ( attack , target );
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
                    target = bitboard_lsb ( attack );
 
                    // Quiet move.
                    if ( !bit ( ( ( *board ).side == WHITE ) ? ( *board ).occupancies[ BLACK ]
                                                             : ( *board ).occupancies[ WHITE ]
                              , target
                              ))
                    {
                        moves_push ( moves
                                   , move_encode ( src , target , piece , 0 , 0 , 0 , 0 , 0 )
                                   );
                    }

                    // Capture move.
                    else
                    { 
                        moves_push ( moves
                                   , move_encode ( src , target , piece , 0 , 1 , 0 , 0 , 0 )
                                   );
                    }

                    BITCLR ( attack , target );
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
                    target = bitboard_lsb ( attack );
 
                    // Quiet move.
                    if ( !bit ( ( ( *board ).side == WHITE ) ? ( *board ).occupancies[ BLACK ]
                                                             : ( *board ).occupancies[ WHITE ]
                              , target
                              ))
                    {
                        moves_push ( moves
                                   , move_encode ( src , target , piece , 0 , 0 , 0 , 0 , 0 )
                                   );
                    }

                    // Capture move.
                    else
                    { 
                        moves_push ( moves
                                   , move_encode ( src , target , piece , 0 , 1 , 0 , 0 , 0 )
                                   );
                    }

                    BITCLR ( attack , target );
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
                    target = bitboard_lsb ( attack );
 
                    // Quiet move.
                    if ( !bit ( ( ( *board ).side == WHITE ) ? ( *board ).occupancies[ BLACK ]
                                                             : ( *board ).occupancies[ WHITE ]
                              , target
                              ))
                    {
                        moves_push ( moves
                                   , move_encode ( src , target , piece , 0 , 0 , 0 , 0 , 0 )
                                   );
                    }

                    // Capture move.
                    else
                    { 
                        moves_push ( moves
                                   , move_encode ( src , target , piece , 0 , 1 , 0 , 0 , 0 )
                                   );
                    }

                    BITCLR ( attack , target );
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
                    target = bitboard_lsb ( attack );
 
                    // Quiet move.
                    if ( !bit ( ( ( *board ).side == WHITE ) ? ( *board ).occupancies[ BLACK ]
                                                             : ( *board ).occupancies[ WHITE ]
                              , target
                              ))
                    {
                        moves_push ( moves
                                   , move_encode ( src , target , piece , 0 , 0 , 0 , 0 , 0 )
                                   );
                    }

                    // Capture move.
                    else
                    { 
                        moves_push ( moves
                                   , move_encode ( src , target , piece , 0 , 1 , 0 , 0 , 0 )
                                   );
                    }

                    BITCLR ( attack , target );
                }

                BITCLR ( pieces , src );
            }
        }

    }// END for.
}
