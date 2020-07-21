#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <board.h>
#include <move.h>
#include <vector>

namespace nnchesslib
{
    typedef std::vector<Move> MoveList;

    void genLegalMoves(ChessBoard cboard, MoveList& moveList);
    void genPseudoLegalMoves(ChessBoard cboard, MoveList& moveList);

    void testXDD();

    void genWhiteMoves(ChessBoard cboard, MoveList& moveList, BitBoard blockers);
    void genBlackMoves(ChessBoard cboard, MoveList& moveList, BitBoard blockers);

    // pawns only move in one direction so seperate functions for white and black are needed.
    void genWhiteSinglePawnMoves(ChessBoard cboard, MoveList& moveList, BitBoard blockers);
    void genWhiteDoublePawnMoves(ChessBoard cboard, MoveList& moveList, BitBoard blockers);
    void genWhitePawnCaptures(ChessBoard cboard, MoveList& moveList, BitBoard blockers);

    void genBlackSinglePawnMoves(ChessBoard cboard, MoveList& moveList, BitBoard blockers);
    void genBlackDoublePawnMoves(ChessBoard cboard, MoveList& moveList, BitBoard blockers);
    void genBlackPawnCaptures(ChessBoard cboard, MoveList& moveList, BitBoard blockers);

    void genNonSlidingMoves(ChessBoard cboard, MoveList& moveList, Color color, PieceType piece);
    void genSlidingMoves(ChessBoard cboard, MoveList& moveList, Color color, PieceType piece, BitBoard blockers);
    void genKingMoves(ChessBoard cboard, MoveList& moveList, Color color, BitBoard blockers);

    void genPromotions(int pawnIndex, MoveList& moveList, Color color, BitBoard pawns);

    void genCastlingMoves(ChessBoard board, MoveList& moveList, Color color, BitBoard blockers);

    int popLsb(U64 &board);
}

#endif
