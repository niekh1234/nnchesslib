// -----------TODO--------------
// gen all bitboards
// ---white, black, pawns, knights, bishops, rooks, queens, kings

// setup board from start function

// TRANSLATE FEN
// ---From and to fen

// manipulate bitboards (maybe for different file)
// ---make move, (push move, pop move)
#include <bitboard.h>

#pragma once

using namespace nnchesslib;

struct BoardInfo
{
    //Copied from move

    int fiftyMoveRule;
    int plyCount;

    //Uncopied from move
    int repetition;
}

class ChessBoard
{
    private:
        BitBoard white_pieces;
        BitBoard black_pieces;
        BitBoard pawns;
        BitBoard knights;
        BitBoard bishops;
        BitBoard rooks;
        BitBoard queens;
        BitBoard kings;

        bool whiteToMove;
    public:
        void generateFromFen();
};
