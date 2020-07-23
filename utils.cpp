#include <iostream>
#include <string>
#include <utils.h>
#include <types.h>
#include <cassert>

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

// converts e.g. 'e2' to 12
int nnchesslib::getSquareInt(std::string square)
{
    int file = square.at(0) - 97;
    int rank = square.at(1) - '0';
    return file + (rank - 1) * 8;
}

std::string nnchesslib::getSquareString(int square)
{
    assert(square >= 0 && square <= 63);
    int file = square % 8;
    int rank = square / 8 + 1;

    const char* files = "abcdefgh";

    return files[file] + std::to_string(rank);
}