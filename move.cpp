#include <move.h>

using namespace nnchesslib;

int from_square(Move m)
{
    return (m >> 6) & 0xFFF;
}

int to_square(Move m)
{
    return (m & 0xFFF);
}

PieceType move_promotion_type(Move m)
{
    return PieceType(((m >> 12) & 3) + KNIGHT);
}

MoveType move_type(Move m)
{
    return MoveType(((m >> 14) & 3) + (int)NORMAL);
}

Move create_move(int from, int to)
{
    return (from << 6) + to;
}

Move create_move(int from, int to, PieceType p)
{
    return (PROMOTION << 14) + ((p-KNIGHT) << 12) + (from << 6) + to;
}

Move create_move(int from, int to, MoveType mt)
{
    return (mt << 14) + (from << 6) + to;
}