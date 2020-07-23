#ifndef BOARD_H
#define BOARD_H

#include <bitboard.h>
#include <move.h>
#include <iostream>

namespace nnchesslib
{
    struct BoardInfo
    {
        BitBoard whitePieces;
        BitBoard blackPieces;
        BitBoard pawns;
        BitBoard knights;
        BitBoard bishops;
        BitBoard rooks;
        BitBoard queens;
        BitBoard kings;

        bool whiteToMove = true;

        bool whiteCastleShort = false;
        bool whiteCastleLong = false;
        bool blackCastleShort = false;
        bool blackCastleLong = false;   
        int fiftyMoveRule;
        int plyCount;
        int repetition;

        BitBoard whiteEnPassantTarget;
        BitBoard blackEnPassantTarget;
    };

    class ChessBoard
    {
        private:
            // Generate bitboards corresponding to a given fen.
            void generateBitBoards(std::string fen);
        public:
            BoardInfo boardinfo;
            BoardInfo previousBoard;

            ChessBoard();
            ChessBoard(std::string fenRepresentation);

            // Determine whether a fen is valid.
            bool isValidFen(std::string fen);
            // Cout current instance of board. 
            void print();
            // Return the bitboard of a specified PieceType and color.
            BitBoard getBoard(Color color, PieceType piece);
            // Return the bitboard of a specified color.
            BitBoard getBoard(Color color);
            // Return all occupied squares in the board.
            BitBoard getBlockers();

            // Returns true if it is white to move and false if black is to move.
            bool getWhiteToMove();

            // Returns the piece bitboard by looking at which piece is on a specific index.
            BitBoard * getPieceOnSquare(int index);
            // Returns the color bitboard by looking at which piece is on a specific index.
            BitBoard * getColorOnSquare(int index);

            // Determines whether a black or white king is in check. Usage: kingInCheck(BLACK) returns true if black king in check.
            bool kingInCheck(Color color);
            // Determines whether a square is attacked by an opponent piece.
            bool squareAttacked(int square, Color color);

            void setEnPassantPossibility(BitBoard ourPieces, int from, int to);
            // Updates the boards castling rights.
            void updateCastlingRights();

            // Pushes a castling move to the board.
            void pushCastlingMove(Move move);
            // Pushes a promotion move to the board.
            void pushPromotionMove(Move move);
            // Pushes an en passant move to the board.
            void pushEnPassantMove(Move move);
            // Pushes a regular move to the board.
            void pushRegularMove(Move move);
            // Pushes a move to the board.
            void pushMove(Move move);
            // Undo's a move.
            void popMove();

            // Returns the opposite color: BLACK -> WHITE
            Color getOppositeColor(Color color);

            // Get char representation of a piece at an index.
            std::string getPieceChar(int i);
            // Board to fen conversion.
            std::string convertToFen();

            // Creating moves by parsing uci strings.
            Move createUci(std::string uci);
    };
}

#endif