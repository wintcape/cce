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
 * @brief Generates move options using a pregenerated attack table.
 * @param attacks The pregenerated attack tables.
 * @param board A chess board state.
 */
INLINE
void
moves_init
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

        // White.
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
                            LOGINFO ("pawn promotion: %s%sq",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion: %s%sr",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion: %s%sb",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion: %s%sn",string_chess_board_square(src),string_chess_board_square(target));
                        }
                        else
                        {
                            // Push.
                            LOGINFO ("pawn push: %s%s",string_chess_board_square(src),string_chess_board_square(target));

                            // Double push.
                            if ( ( src >= A2 && src <= H2 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                                    , target - 8
                                                                    ))
                            {
                                LOGINFO ("double pawn push: %s%s",string_chess_board_square(src),string_chess_board_square(target-8));

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
                            LOGINFO ("pawn promotion capture: %s%sq",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sr",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sb",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sn",string_chess_board_square(src),string_chess_board_square(target));
                        }
                        // Push + capture.
                        else
                        {
                            LOGINFO ("pawn capture: %s%s",string_chess_board_square(src),string_chess_board_square(target));
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
                            LOGINFO ("pawn enpassant capture: %s%s",string_chess_board_square(src),string_chess_board_square(target_enpassant));
                            
                        }
                    }

                    BITCLR ( bitboard , src );
                }
            }

            else
            {}
        }

        // Black.
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
                            LOGINFO ("pawn promotion: %s%sq",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion: %s%sr",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion: %s%sb",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion: %s%sn",string_chess_board_square(src),string_chess_board_square(target));
                        }
                        else
                        {
                            // Push.
                            LOGINFO ("pawn push: %s%s",string_chess_board_square(src),string_chess_board_square(target));

                            // Double push.
                            if ( ( src >= A7 && src <= H7 ) && !bit ( ( *board ).occupancies[ 2 ]
                                                                    , target + 8
                                                                    ))
                            {
                                LOGINFO ("double pawn push: %s%s",string_chess_board_square(src),string_chess_board_square(target+8));

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
                            LOGINFO ("pawn promotion capture: %s%sq",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sr",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sb",string_chess_board_square(src),string_chess_board_square(target));
                            LOGINFO ("pawn promotion capture: %s%sn",string_chess_board_square(src),string_chess_board_square(target));
                        }
                        // Push + capture.
                        else
                        {
                            LOGINFO ("pawn capture: %s%s",string_chess_board_square(src),string_chess_board_square(target));
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
                            LOGINFO ("pawn enpassant capture: %s%s",string_chess_board_square(src),string_chess_board_square(target_enpassant));
                            
                        }
                    }

                    BITCLR ( bitboard , src );
                }
            }

            else
            {}
        }
    }// END for.
}

#endif  // CHESS_MOVE_H
