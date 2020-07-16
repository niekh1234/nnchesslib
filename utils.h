#ifndef UTILS_H
#define UTILS_H

#include <types.h>

using namespace nnchesslib;

// counts 1's in a U64
int countBits(U64 n)
{
    int output = 0;
    while (n != 0)
    {
        n &= (n - 1);
        output++;
    }
    return output;
}

int bitScanForward(U64 board)
{
    return __builtin_ffsll(board) - 1;
}

int bitScanReverse(U64 board)
{
    return 63 - __builtin_clzll(board);
}

#endif