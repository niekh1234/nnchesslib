#include <move.h>
#include <string>
#include <iostream>
#include <utils.h>

using namespace nnchesslib;

int nnchesslib::from_Square(Move m)
{
    return (m >> 6) & 0x3f;
}

int nnchesslib::to_Square(Move m)
{
    return (m & 0x3f);
}

PieceType nnchesslib::movePromotionType(Move m)
{
    return PieceType(((m >> 12) & 3) + KNIGHT);
}

MoveType nnchesslib::moveType(Move m)
{
    return MoveType(((m >> 14) & 3) + (int)NORMAL);
}

Move nnchesslib::createMove(int from, int to)
{
    return (from << 6) + to;
}

Move nnchesslib::createMove(int from, int to, PieceType p)
{
    return (PROMOTION << 14) + ((p-KNIGHT) << 12) + (from << 6) + to;
}

Move nnchesslib::createMove(int from, int to, MoveType mt)
{
    return (mt << 14) + (from << 6) + to;
}

std::string nnchesslib::toUci(Move m)
{
    std::string from = getSquareString(from_Square(m));
    std::string to = getSquareString(to_Square(m));

    std::string output = from + to;

    if(moveType(m) == PROMOTION)
    {
        switch(movePromotionType(m))
        {
            case QUEEN:
                output+="q";
                break;
            case KNIGHT:
                output+="n";
                break;
            case BISHOP:
                output+="b";
                break;
            case ROOK:
                output+="r";
                break;
        }
    }
    return output;
}

void nnchesslib::printMove(Move m)
{
    std::cout << "From " << from_Square(m) << " to " << to_Square(m) << std::endl;
}