#include <utils.h>
#include <types.h>

using namespace nnchesslib;

// counts 1's in a U64
int nnchesslib::countBits(U64 n)
{
    int output = 0;
    while (n != 0)
    {
        n &= (n - 1);
        output++;
    }
    return output;
}

extern int nnchesslib::bitScanForward(U64 board)
{
    return __builtin_ffsll(board) - 1;
}

extern int nnchesslib::bitScanReverse(U64 board)
{
    return 63 - __builtin_clzll(board);
}

extern int nnchesslib::getEPSquare(std::string square)
{
    if(square == "a3") return A3;
    if(square == "b3") return B3;
    if(square == "c3") return C3;
    if(square == "d3") return D3;
    if(square == "e3") return E3;
    if(square == "f3") return F3;
    if(square == "g3") return G3;
    if(square == "h3") return H3;
    if(square == "a6") return A6;
    if(square == "b6") return B6;
    if(square == "c6") return C6;
    if(square == "d6") return D6;
    if(square == "e6") return E6;
    if(square == "f6") return F6;
    if(square == "g6") return G6;
    if(square == "h6") return H6;
    return 0;
}