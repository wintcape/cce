/**
 * @file move.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defiens operations for pregenerating move options.
 */
#ifndef CHESS_MOVE_H
#define CHESS_MOVE_H

#include "chess/common.h"

#include "chess/board.h"
#include "chess/string.h"

// Type definition for a move.
typedef u32 move_t;

/**
 * @brief Encodes a single move via bitpacking.
 * @param src The source square.
 * @param target The target square.
 * @param piece The piece on the source square.
 * @param promotion Promoted piece, if any.
 * @param capture Is move capture? Y/N
 * @param enpassant Is move en passant? Y/N
 * @param castle Is move castle? Y/N
 * @return A single move, encoded as a bitpacked 32-bit unsigned integer.
 */
INLINE
move_t
board_move_encode
(   const SQUARE    src
,   const SQUARE    target
,   const PIECE     piece
,   const PIECE     promotion
,   const bool      capture
,   const bool      double_push
,   const bool      enpassant
,   const bool      castle
)
{
    return src
         | ( target << 6 )
         | ( piece << 12 )
         | ( promotion << 16 )
         | ( capture << 20 )
         | ( double_push << 21 )
         | ( enpassant << 22 )
         | ( castle << 23 )
         ;
}

/**
 * @brief Retrieves source square from a bitpacked move.
 * @param move A move.
 * @return The source square of move.
 */
INLINE
SQUARE
board_move_decode_source_square
(   const move_t move
)
{
    return move & 0x3F;
}

/**
 * @brief Retrieves target square from a bitpacked move.
 * @param move A move.
 * @return The target square of move.
 */
INLINE
SQUARE
board_move_decode_target_square
(   const move_t move
)
{
    return ( move & 0xFC ) >> 6;
}

/**
 * @brief Retrieves piece from a bitpacked move.
 * @param move A move.
 * @return The piece of move.
 */
INLINE
SQUARE
board_move_decode_piece
(   const move_t move
)
{
    return ( move & 0xF000 ) >> 12;
}

/**
 * @brief Retrieves source square from a bitpacked move.
 * @param move A move.
 * @return The source square of move.
 */
INLINE
SQUARE
board_move_decode_promotion
(   const move_t move
)
{
    return ( move & 0xF0000 ) >> 16;
}

/**
 * @brief Retrieves source square from a bitpacked move.
 * @param move A move.
 * @return The source square of move.
 */
INLINE
SQUARE
board_move_decode_capture
(   const move_t move
)
{
    return ( move & 0x100000 ) >> 20;
}

/**
 * @brief Retrieves source square from a bitpacked move.
 * @param move A move.
 * @return The source square of move.
 */
INLINE
SQUARE
board_move_decode_double_push
(   const move_t move
)
{
    return ( move & 0x200000 ) >> 21;
}

/**
 * @brief Retrieves source square from a bitpacked move.
 * @param move A move.
 * @return The source square of move.
 */
INLINE
SQUARE
board_move_decode_enpassant
(   const move_t move
)
{
    return ( move & 0x400000 ) >> 22;
}

/**
 * @brief Retrieves source square from a bitpacked move.
 * @param move A move.
 * @return The source square of move.
 */
INLINE
SQUARE
board_move_decode_castling
(   const move_t move
)
{
    return ( move & 0x800000 ) >> 23;
}

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
                        LOGINFO ("knight move: %s%s",string_board_square(src),string_board_square(target));
                    }

                    // Capture move.
                    else
                    {
                        LOGINFO ("knight capture: %s%s",string_board_square(src),string_board_square(target));
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
                        LOGINFO ("bishop move: %s%s",string_board_square(src),string_board_square(target));
                    }

                    // Capture move.
                    else
                    {
                        LOGINFO ("bishop capture: %s%s",string_board_square(src),string_board_square(target));
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
                        LOGINFO ("rook move: %s%s",string_board_square(src),string_board_square(target));
                    }

                    // Capture move.
                    else
                    {
                        LOGINFO ("rook capture: %s%s",string_board_square(src),string_board_square(target));
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
                        LOGINFO ("queen move: %s%s",string_board_square(src),string_board_square(target));
                    }

                    // Capture move.
                    else
                    {
                        LOGINFO ("queen capture: %s%s",string_board_square(src),string_board_square(target));
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
                        LOGINFO ("king move: %s%s",string_board_square(src),string_board_square(target));
                    }

                    // Capture move.
                    else
                    {
                        LOGINFO ("king capture: %s%s",string_board_square(src),string_board_square(target));
                    }

                    BITCLR ( attack , target );
                }

                BITCLR ( bitboard , src );
            }
        }

    }// END for.
}

#endif  // CHESS_MOVE_H
