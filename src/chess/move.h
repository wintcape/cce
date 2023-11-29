/**
 * @file move.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defiens operations for pregenerating move options.
 */
#ifndef CHESS_MOVE_H
#define CHESS_MOVE_H

#include "chess/common.h"

#include "chess/string.h"

/**
 * @brief Generates the move options for a given board state using pregenerated
 * attack tables.
 * @param attacks The pregenerated attack tables.
 * @param board A chess board state.
 */
INLINE
void
board_moves
(   board_t*            board
,   const attacks_t*    attacks
)
{
    SQUARE src;
    SQUARE target;

    bitboard_t bitboard;
    bitboard_t attack;

    for ( PIECE piece = P; piece <= k; ++piece )
    {
        bitboard = ( *board ).pieces[ piece ];

        if ( ( *board ).side == WHITE )
        {
            // White pawn.
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
                            LOGINFO ("pawn promotion: %s%sq",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion: %s%sr",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion: %s%sb",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion: %s%sn",string_board_square(src),string_board_square(target));
                        }
                        else
                        {
                            // Push.
                            LOGINFO ("pawn push: %s%s",string_board_square(src),string_board_square(target));

                            // Double push.
                            if ( ( src >= A2 && src <= H2 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                                    , target - 8
                                                                    ))
                            {
                                LOGINFO ("double pawn push: %s%s",string_board_square(src),string_board_square(target-8));

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
                            LOGINFO ("pawn promotion capture: %s%sq",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sr",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sb",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sn",string_board_square(src),string_board_square(target));
                        }
                        // Push + capture.
                        else
                        {
                            LOGINFO ("pawn capture: %s%s",string_board_square(src),string_board_square(target));
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
                            LOGINFO ("pawn enpassant capture: %s%s",string_board_square(src),string_board_square(target_enpassant));
                            
                        }
                    }

                    BITCLR ( bitboard , src );
                }
            }
            
            // White king.
            else if ( piece == K )
            {
                // Castling moves.
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
                            LOGINFO ("king castling: E1G1");
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
                            LOGINFO ("king castling: E1C1");
                        }
                    }
                }
            }
        }
        else
        {
            // Black pawn.
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
                            LOGINFO ("pawn promotion: %s%sq",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion: %s%sr",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion: %s%sb",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion: %s%sn",string_board_square(src),string_board_square(target));
                        }
                        else
                        {
                            // Push.
                            LOGINFO ("pawn push: %s%s",string_board_square(src),string_board_square(target));

                            // Double push.
                            if ( ( src >= A7 && src <= H7 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                                    , target + 8
                                                                    ))
                            {
                                LOGINFO ("double pawn push: %s%s",string_board_square(src),string_board_square(target+8));

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
                        if ( src >= A2 && src <= H2 )
                        {
                            LOGINFO ("pawn promotion capture: %s%sq",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sr",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sb",string_board_square(src),string_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sn",string_board_square(src),string_board_square(target));
                        }
                        // Push + capture.
                        else
                        {
                            LOGINFO ("pawn capture: %s%s",string_board_square(src),string_board_square(target));
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
                            LOGINFO ("pawn enpassant capture: %s%s",string_board_square(src),string_board_square(target_enpassant));
                            
                        }
                    }

                    BITCLR ( bitboard , src );
                }
            }

            // Black king.
            else if ( piece == k )
            {
                // Castling moves.
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
                            LOGINFO ("king castling: E8G8");
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
                            LOGINFO ("king castling: E8C8");
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
                       & ( ( *board ).side == WHITE ) ? ~( *board ).occupancies[ WHITE ]
                                                      : ~( *board ).occupancies[ BLACK ]
                                                      ;
                while ( attack )
                {
                    target = bitboard_lsb ( attack );
                   
                    // Quiet move.
                    if ( !bit ( ( ( *board ).side == WHITE ) ? ( *board ).occupancies[ BLACK ]
                                                             : ( *board ).occupancies[ WHITE ]
                              , target
                              ))
                    {
                        LOGINFO ("knight piece move: %s%s",string_board_square(src),string_board_square(target));
                    }

                    // Capture move.
                    else
                    {
                        LOGINFO ("knight piece capture: %s%s",string_board_square(src),string_board_square(target));
                    }

                    BITCLR ( attack , target );
                }

                BITCLR ( bitboard , src );
            }
        }

    }// END for.
}

#endif  // CHESS_MOVE_H
