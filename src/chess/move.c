/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file move.c
 * @brief Implementation of the move header.
 * (see move.h for additional details)
 */
#include "chess/move.h"

#include "chess/board.h"
#include "chess/string.h"

#include "core/logger.h"
#include "core/memory.h"

bool
move_parse
(   const char*         s
,   const moves_t*      moves
,   const attacks_t*    attacks
,   move_t*             move
)
{
    const u8 len = string_length_clamp ( s , MOVE_STRING_LENGTH + 1 );

    // Validate input string length.
    if ( len > MOVE_STRING_LENGTH )
    {
        return false;
    }

    // Copy input string into working buffer.
    char buf[ MOVE_STRING_LENGTH + 1 ];
    memory_copy ( buf , s , len );
    buf[ len ] = 0; // Append terminator.

    // Sanitize.
    string_trim ( buf );
    for ( u8 i = 0; i < len; ++ i )
    {
        buf[ i ] = to_uppercase ( buf[ i ] );
    }

    // Validate.
    if (   len < MOVE_STRING_LENGTH - 1
        || buf[ 0 ] < 'A' || buf[ 0 ] > 'H'
        || buf[ 1 ] < '1' || buf[ 1 ] > '8'
        || buf[ 2 ] < 'A' || buf[ 2 ] > 'H'
        || buf[ 3 ] < '1' || buf[ 3 ] > '8'
       )
    {
        return false;
    }
    if (   len == MOVE_STRING_LENGTH
        && buf[ 4 ] != 'N'
        && buf[ 4 ] != 'B'
        && buf[ 4 ] != 'R'
        && buf[ 4 ] != 'Q'
       )
    {
        return false;
    }
    
    // Parse the move string.
    const SQUARE src = SQUAREINDX ( 8 - to_digit ( buf[ 1 ] )
                                  , buf[ 0 ] - 'A'
                                  );
    const SQUARE dst = SQUAREINDX ( 8 - to_digit ( buf[ 3 ] )
                                  , buf[ 2 ] - 'A'
                                  );

    // Check move validity.
    for ( u8 i = 0; i < ( *moves ).count; ++i )
    {
        const move_t move_ = ( *moves ).moves[ i ];
        if (   src != move_decode_src ( move_ )
            || dst != move_decode_dst ( move_ )
           )
        {
            continue;
        }

        const PIECE promotion = move_decode_promotion ( move_ );
        if (   ( buf[ 4 ] == 'N' && promotion != N && promotion != n )
            || ( buf[ 4 ] == 'B' && promotion != B && promotion != b )
            || ( buf[ 4 ] == 'R' && promotion != R && promotion != r )
            || ( buf[ 4 ] == 'Q' && ( promotion == Q || promotion == q ) )
           )
        {
            continue;
        }
        
        // Write valid move to the output buffer.
        *move = move_;
        return true;
    }

    return false;
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

moves_t*
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

    return moves;
}

moves_t*
moves_filter
(   moves_t*    moves_
,   MOVE_FILTER filter_
)
{
    moves_t moves;
    for ( u8 i = 0; i < ( *moves_ ).count; ++i )
    {
        const move_t move = ( *moves_ ).moves[ i ];
        const PIECE piece = move_decode_piece ( move );
        bool filter;
        switch ( filter_ )
        {
            case MOVE_FILTER_NONE            : return moves_                             ;
            case MOVE_FILTER_ONLY_PAWN       : filter = ( piece == P || piece == p )     ;break;
            case MOVE_FILTER_ONLY_KNIGHT     : filter = ( piece == N || piece == n )     ;break;
            case MOVE_FILTER_ONLY_BISHOP     : filter = ( piece == B || piece == b )     ;break;
            case MOVE_FILTER_ONLY_ROOK       : filter = ( piece == R || piece == r )     ;break;
            case MOVE_FILTER_ONLY_QUEEN      : filter = ( piece == Q || piece == q )     ;break;
            case MOVE_FILTER_ONLY_KING       : filter = ( piece == K || piece == k )     ;break;
            case MOVE_FILTER_ONLY_CAPTURE    : filter = move_decode_capture ( move )     ;break;
            case MOVE_FILTER_ONLY_PROMOTION  : filter = move_decode_promotion ( move )   ;break;
            case MOVE_FILTER_ONLY_DOUBLE_PUSH: filter = move_decode_double_push ( move ) ;break;
            case MOVE_FILTER_ONLY_ENPASSANT  : filter = move_decode_enpassant ( move )   ;break;
            case MOVE_FILTER_ONLY_CASTLE     : filter = move_decode_castle ( move )      ;break;
            default                          : return moves_                             ;
        }
        if ( filter )
        {
            moves_push ( &moves , move );
        }
    }
    return memory_copy ( moves_ , &moves , sizeof ( moves_t ) );
}