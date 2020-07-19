#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <board.h>
#include <move.h>
#include <vector>

namespace nnchesslib
{
    typedef std::vector<Move::Move> MoveList;

    class MoveGen
    {
        public:
            ChessBoard board;
            MoveGen(ChessBoard board);
            // move generation
            MoveList legalMoves;
            MoveList pseudoLegalMoves;

            MoveList genLegalMoves();
            MoveList genPseudoLegalMoves(ChessBoard board);

        private:
            void genWhiteMoves(BitBoard blockers);
            void genBlackMoves(BitBoard blockers);

            // pawns only move in one direction so seperate functions for white and black are needed.
            void genWhiteSinglePawnMoves(BitBoard blockers);
            void genWhiteDoublePawnMoves(BitBoard blockers);

            void genWhitePawnCaptures(BitBoard blockers);
            void genBlackPawnCaptures(BitBoard blockers);

            int popLsb(U64 &board);
    };
}

#endif
