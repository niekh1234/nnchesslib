#include <bitboard.h>
#include <iostream>
#include <bitset>
#include <cassert>
#include <string>
#include <types.h>

using namespace nnchesslib;

BitBoard::BitBoard()
{
    board = 0;
}

BitBoard::BitBoard(U64 value)
{
    board = value;
}

std::string BitBoard::getBoardString()
{
    std::string binary = std::bitset<64>(board).to_string();
    return binary;
}

void BitBoard::set(int square, bool set)
{
    assert(0 <= square && square <= 63);
    if(set)
        board |= ((U64)1 << square);
    else
        board &= ~((U64)1 << square);
}

int BitBoard::get(int x, int y)
{
    assert(0 <= x && x <= 7);
    assert(0 <= y && y <= 7);
    
    int val = (board >> (8*y+7-x)) & 1;

    return val;
}

void BitBoard::setRank(int y)
{
    assert(0 <= y && y <= 7);
    
    board |= rank_bb[y]; 
}

void BitBoard::setFile(int x)
{
    assert(0 <= x && x <= 7);

    board |= file_bb[x];
}

void BitBoard::setAll(bool val)
{

}

void BitBoard::reset()
{

}

U64 BitBoard::flipVertical()
{
    return __bswap_64(BitBoard::board);
}

void BitBoard::printDebug()
{
    std::string boardString = getBoardString();
    std::cout<<"---------------"<<std::endl;
    for (int y = 0; y <= 7; y++)
    {
        for (int x = 0; x <= 7; x++)
        {
            // if (boardString[63 - (8*y+x)] == '1')
            if (boardString[8*y+7-x] == '1')
                std::cout << "1 ";
            else
                std::cout << ". ";
        }
        std::cout << std::endl;
    }
    std::cout<<"---------------"<<std::endl;
}