#include <bitboard.h>
#include <iostream>
#include <bitset>
#include <cassert>
#include <string>

using namespace nnchesslib;

std::string BitBoard::getBoardString()
{
    return board.to_string();
}

void BitBoard::set(int x, int y, bool set)
{
    assert(0 <= x && x <= 7);
    assert(0 <= y && y <= 7);
    
    if(set)
        board.set(8*y+7-x);
    else
        board.reset(8*y+7-x);
}

int BitBoard::get(int x, int y)
{
    assert(0 <= x && x <= 7);
    assert(0 <= y && y <= 7);
    
    return board[8*(7-y)+x];
}

void BitBoard::setRank(int x)
{
    assert(0 <= x && x <= 7);

    for(int i = 0; i < 8; i++)
        board.set(i + x*8);
}

void BitBoard::setFile(int y)
{
    assert(0 <= y && y <= 7);

    for(int i = 0; i < 8; i++)
    {
        board.set(i*8 + 7 - y);
    }
}

void BitBoard::reset()
{
    board.reset();
}