#include <movegen.h>
#include <move.h>

using namespace nnchesslib;

MoveGen::MoveGen(ChessBoard board)
{
    MoveGen::board = board;
}

MoveList MoveGen::genLegalMoves()
{
    MoveGen::genPseudoLegalMoves(MoveGen::board);
    // do something XD
    return legalMoves;
}

MoveList MoveGen::genPseudoLegalMoves(ChessBoard board)
{
    MoveGen::board = board;
    BitBoard blockers = board.getBlockers();
    
    if(board.getWhiteToMove())
        MoveGen::genWhiteMoves(blockers);
    else
        MoveGen::genBlackMoves(blockers);

    return pseudoLegalMoves;
}

void MoveGen::genWhiteMoves(BitBoard blockers)
{
    genWhiteSinglePawnMoves(blockers);
}

void MoveGen::genBlackMoves(BitBoard blockers)
{
    NULL;
}

void MoveGen::genWhiteSinglePawnMoves(BitBoard blockers)
{
    BitBoard pawns = board.getBoard(WHITE, PAWN);
    // checking if pawns have been blocked
    BitBoard pawnsMoved = (pawns.board << 8) & ~blockers.board;

    BitBoard promotedPawns = pawnsMoved.board & rank_bb[RANK_8];
    pawnsMoved.board &= ~rank_bb[RANK_8];

    int validPawns = __builtin_popcountll(pawnsMoved.board);

    for(int _ = 0; _ < validPawns; _++)
    {
        // finding current index of pawn.
        int index = popLsb(pawnsMoved.board);
        // adding to pseudo legal movelist. 
        Move::Move move = Move::createMove(index - 8, index);
        pseudoLegalMoves.push_back(move);
    }
    // 
    // todo: pawn promotions.
    // 
}

void MoveGen::genWhiteDoublePawnMoves(BitBoard blockers)
{
    BitBoard pawns = board.getBoard(WHITE, PAWN);

}



int MoveGen::popLsb(U64 &board)
{
    int lsbIndex = __builtin_ffsll(board) - 1;
    board &= board - 1;
    return lsbIndex;
}