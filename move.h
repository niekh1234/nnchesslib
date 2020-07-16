#ifndef MOVE_H
#define MOVE_H

#include <types.h>

namespace nnchesslib
{

    enum MoveType
    {
        NORMAL, PROMOTION, ENPASSANT, CASTLING
    };

    typedef unsigned int Move;
    //0-5 -> to
    //6-11 -> from
    //12-13 -> promotionpiecetype (PieceType-1)
    //14-15 -> (0) no special (1) promotion (2) en passant (3) castle

    int from_square(Move m);
    int to_square(Move m);
    PieceType move_promotion_type(Move m);
    MoveType move_type(Move m);

    Move create_move(int from, int to);
    Move create_move(int from, int to, PieceType p);
    Move create_move(int from, int to, MoveType mt);
}
#endif