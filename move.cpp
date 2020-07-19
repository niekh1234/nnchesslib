#include <move.h>
#include <iostream>
#include <bitset>

using namespace nnchesslib;

int Move::from_Square(Move m)
{
    return (m >> 6) & 0xFFF;
}

int Move::to_Square(Move m)
{
    return (m & 0x3f);
}

PieceType Move::movePromotionType(Move m)
{
    return PieceType(((m >> 12) & 3) + KNIGHT);
}

Move::MoveType Move::moveType(Move m)
{
    return MoveType(((m >> 14) & 3) + (int)NORMAL);
}

Move::Move Move::createMove(int from, int to)
{
    return (from << 6) + to;
}

Move::Move Move::createMove(int from, int to, PieceType p)
{
    return (PROMOTION << 14) + ((p-KNIGHT) << 12) + (from << 6) + to;
}

Move::Move Move::createMove(int from, int to, MoveType mt)
{
    return (mt << 14) + (from << 6) + to;
}