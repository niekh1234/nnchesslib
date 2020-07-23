#ifndef UTILS_H
#define UTILS_H

#include <types.h>
#include <string>

namespace nnchesslib{

    int countBits(U64 n);

    int bitScanForward(U64 board);

    int bitScanReverse(U64 board);

    extern int getSquare(std::string square);
}

#endif