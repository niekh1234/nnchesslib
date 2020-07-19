#ifndef MOVE_H
#define MOVE_H

#include <types.h>

namespace nnchesslib
{
    namespace Move
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

        int from_Square(Move m);
        int to_Square(Move m);
        PieceType movePromotionType(Move m);
        MoveType moveType(Move m);

        Move createMove(int from, int to);
        Move createMove(int from, int to, PieceType p);
        Move createMove(int from, int to, MoveType mt);

    };
}
#endif