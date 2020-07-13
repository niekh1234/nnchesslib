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
    bool whiteCastleShort;
    bool whiteCastleLong;
    bool blackCastleShort;
    bool blackCastleLong;
    
    int fiftyMoveRule;
    int plyCount;

    //Uncopied from move
    // Piece capturedPiece;
    BoardInfo* previousBoard;
    int repetition;
};

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
        std::string fen;
        ChessBoard(std::string fenRepresentation){
            fen = fenRepresentation;
            // next step -> fen to board representation
            // -> determine if valid fen.
        }
};
