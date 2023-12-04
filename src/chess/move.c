/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file move.c
 * @brief Implementation of the move header.
 * (see move.h for additional details)
 */
#include "chess/move.h"

#include "chess/board.h"
#include "chess/string.h"

bool
move_parse
(   const move_t        move
,   const MOVE_FILTER   filter
,   board_t*            board
)
{
    if ( filter == MOVE_FILTER_ONLY_CAPTURES )
    {
        return move_decode_capture ( move ) ? move_parse ( move
                                                         , MOVE_FILTER_NONE
                                                         , board
                                                         )
                                            : false
                                            ;
    }

    //const SQUARE src = move_decode_source_square ( move );
    const SQUARE target = move_decode_source_square ( move );
    const PIECE piece = move_decode_piece ( move );
    //const PIECE promotion = move_decode_promotion ( move );
    const bool capture = move_decode_capture ( move );
    //const bool double_push = move_decode_double_push ( move );
    //const bool enpassant = move_decode_enpassant ( move );
    //const bool castle = move_decode_castle ( move );
    
    // Move the piece.
    BITCLR ( ( *board ).pieces[ piece ]
           , move_decode_source_square ( move )
           );
    BITSET ( ( *board ).pieces[ piece ]
           , move_decode_target_square ( move )
           );

    // Capture.
    if ( capture )
    {
        const PIECE from = ( ( *board ).side == WHITE ) ? p : P;
        const PIECE to = ( ( *board ).side == WHITE ) ? k : K;
        for ( PIECE i = from; i <= to; ++i )
        {
            if ( bit ( ( *board ).pieces[ i ] , target ) )
            {
                BITCLR ( ( *board ).pieces[ i ] , target );
                break;
            }
        }
    }

    return true;
}

void
moves_init
(   moves_t*            moves
,   board_t*            board
,   const attacks_t*    attacks
)
{
    SQUARE src;
    SQUARE target;

    bitboard_t bitboard;
    bitboard_t attack;

    ( *moves ).count = 0;

    for ( PIECE piece = P; piece <= k; ++piece )
    {
        bitboard = ( *board ).pieces[ piece ];

        // White pawn and castling moves.
        if ( ( *board ).side == WHITE )
        {
            // Pawn.
            if ( piece == P )
            {
                while ( bitboard )
                {
                    src = bitboard_lsb ( bitboard );
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

                    BITCLR ( bitboard , src );
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
                while ( bitboard )
                {
                    src = bitboard_lsb ( bitboard );
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

                    BITCLR ( bitboard , src );
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
            while ( bitboard )
            {
                src = bitboard_lsb ( bitboard );
               
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

                BITCLR ( bitboard , src );
            }
        }
        
        // Bishop.
        else if ( ( ( *board ).side == WHITE ) ? piece == B : piece == b )
        {
            while ( bitboard )
            {
                src = bitboard_lsb ( bitboard );
               
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

                BITCLR ( bitboard , src );
            }
        }
        
        // Rook.
        else if ( ( ( *board ).side == WHITE ) ? piece == R : piece == r )
        {
            while ( bitboard )
            {
                src = bitboard_lsb ( bitboard );
               
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

                BITCLR ( bitboard , src );
            }
        }

        // Queen.
        else if ( ( ( *board ).side == WHITE ) ? piece == Q : piece == q )
        {
            while ( bitboard )
            {
                src = bitboard_lsb ( bitboard );
               
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

                BITCLR ( bitboard , src );
            }
        }

        // King.
        else if ( ( ( *board ).side == WHITE ) ? piece == K : piece == k )
        {
            while ( bitboard )
            {
                src = bitboard_lsb ( bitboard );
               
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

                BITCLR ( bitboard , src );
            }
        }

    }// END for.
}

bool
moves_push
(   moves_t*        moves
,   const move_t    move
)
{
    if ( ( *moves ).count == MOVES_BUFFER_LENGTH )
    {
        return false;
    }

    ( *moves ).moves[ ( *moves ).count ] = move;
    ( *moves ).count += 1;

    return true;
}
