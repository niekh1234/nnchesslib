// -----------TODO--------------
// gen all bitboards
// ---white, black, pawns, knights, bishops, rooks, queens, kings

// setup board from start function

// TRANSLATE FEN
// ---From and to fen

// manipulate bitboards (maybe for different file)
// ---make move, (push move, pop move)
#ifndef BOARD_H
#define BOARD_H

#include <bitboard.h>
#include <move.h>
#include <iostream>

namespace nnchesslib
{
    struct BoardInfo
    {
        // Copied from move
        bool whiteCastleShort;
        bool whiteCastleLong;
        bool blackCastleShort;
        bool blackCastleLong;
        
        int fiftyMoveRule;
        int plyCount;

        // Uncopied from move
        // Piece capturedPiece;
        BoardInfo* previousBoard;
        int repetition;
    };

    class ChessBoard
    {
        private:
            BitBoard whitePieces;
            BitBoard blackPieces;
            BitBoard pawns;
            BitBoard knights;
            BitBoard bishops;
            BitBoard rooks;
            BitBoard queens;
            BitBoard kings;

            bool whiteToMove = true;

            // Generate bitboards corresponding to a given fen.
            void generateBitBoards(std::string fen);
        public:
            bool whiteCastleShort;
            bool whiteCastleLong;
            bool blackCastleShort;
            bool blackCastleLong;   
            int fiftyMoveRule;
            int plyCount;
            int repetition;

            BoardInfo* previousBoard;

            ChessBoard();
            ChessBoard(std::string fenRepresentation);
            // Determine whether a fen is valid.
            bool isValidFen(std::string fen);
            // Cout current instance of board. 
            void print();

            BitBoard getBoard(Color color, PieceType piece);
            BitBoard getBoard(Color color);
            BitBoard getBlockers();

            bool getWhiteToMove();

            BitBoard * getPieceOnSquare(int index);
            BitBoard * getColorOnSquare(int index);

            void updateCastlingRights();

            void pushMove(Move move);
            void popMove();
    };
}

#endif