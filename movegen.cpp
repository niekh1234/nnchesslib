#include <movegen.h>
#include <move.h>
#include <attacks.h>
#include <vector>

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
    genWhiteDoublePawnMoves(blockers);
    genWhitePawnCaptures(blockers);
    genNonSlidingMoves(WHITE, KNIGHT);
    genNonSlidingMoves(WHITE, KING);
    genSlidingMoves(WHITE, ROOK, blockers);
    genSlidingMoves(WHITE, BISHOP, blockers);
    genSlidingMoves(WHITE, QUEEN, blockers);
}

void MoveGen::genBlackMoves(BitBoard blockers)
{
    NULL;
}

void MoveGen::genWhiteSinglePawnMoves(BitBoard blockers)
{
    BitBoard pawns = board.getBoard(WHITE, PAWN);
    // making a move and checking if pawns have been blocked
    BitBoard pawnsMoved = (pawns.board << 8) & ~blockers.board;

    BitBoard promotedPawns = pawnsMoved.board & rank_bb[RANK_8];
    pawnsMoved.board &= ~rank_bb[RANK_8];

    int validPawnCount = __builtin_popcountll(pawnsMoved.board);

    for(int _ = 0; _ < validPawnCount; _++)
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
    // targetting pawns that are on the second rank. These are the only pawns that can move twice.
    BitBoard unmovedPawns = board.getBoard(WHITE, PAWN).board & rank_bb[RANK_2];
    // moving pawns once to check if they are blocked:
    BitBoard firstMove = (unmovedPawns.board << 8) & ~blockers.board;
    // moving again:
    BitBoard secondMove = (firstMove.board << 8) & ~blockers.board;

    int validPawnCount = __builtin_popcountll(secondMove.board);

    for(int _ = 0; _ < validPawnCount; _++)
    {
        // finding current index of pawn.
        int index = popLsb(secondMove.board);
        // adding to pseudo legal movelist. 
        Move::Move move = Move::createMove(index - 16, index);
        pseudoLegalMoves.push_back(move);
    }
}

void MoveGen::genWhitePawnCaptures(BitBoard blockers)
{
    // getting board for white pawns.
    BitBoard pawns = board.getBoard(WHITE, PAWN).board;
    BitBoard black = board.getBoard(BLACK);
    // counting the amount of white pawns.
    int pawnCount = __builtin_popcountll(pawns.board);

    for(int _ = 0; _ < pawnCount; _++)
    {
        // getting the index of a specific pawn and removing it from the board.
        int index = popLsb(pawns.board);
        // getting attacks and comparing to black pieces.
        BitBoard pawnAttacks = Attacks::getNonSlidingAttacks(index, WHITE, PAWN) & black.board;
        if(pawnAttacks.board){
            // getting the amount of pieces a pawn is attacking.
            int attackCount = __builtin_popcountll(pawnAttacks.board);
            
            for(int num = 0; num < attackCount; num++)
            {
                // getting the index of the attacked pawn/piece from black.
                int attackIndex = popLsb(pawnAttacks.board);
                // generating a move with our pawn index and the index of blacks attacked pawn/piece.
                Move::Move move = Move::createMove(index, attackIndex);
                pseudoLegalMoves.push_back(move);
            }
            // TODO
            // promotions with capture.
        }
    }
}

void MoveGen::genNonSlidingMoves(Color color, PieceType piece)
{
    // getting target pieces on the board for a specific color.
    BitBoard targets = board.getBoard(color, piece);

    int count = __builtin_popcountll(targets.board);

    for(int _ = 0; _ < count; _++)
    {
        int index = popLsb(targets.board);

        BitBoard targetMoves = Attacks::getNonSlidingAttacks(index, color, piece);
        BitBoard targetAttacks = targetMoves.board & ~board.getBoard(color).board;

        int attackCount = __builtin_popcountll(targetMoves.board);

        targetAttacks.printDebug();

        for(int num = 0; num < attackCount; num++)
        {
            // getting the index of a square that our piece can move to/capture.
            int attackIndex = popLsb(targetAttacks.board);
            // generating a move with our piece index and the potential square it can move to. 
            Move::Move move = Move::createMove(index, attackIndex);
            pseudoLegalMoves.push_back(move);
        }
    }    
}

void MoveGen::genSlidingMoves(Color color, PieceType piece, BitBoard blockers)
{
    // getting target pieces on the board for a specific color.
    BitBoard targets = board.getBoard(color, piece);

    int count = __builtin_popcountll(targets.board);

    for(int _ = 0; _ < count; _++)
    {
        int index = popLsb(targets.board);

        BitBoard targetMoves = Attacks::getSlidingAttacks(index, piece, blockers.board);
        BitBoard targetAttacks = targetMoves.board & ~board.getBoard(color).board;

        int attackCount = __builtin_popcountll(targetMoves.board);

        for(int num = 0; num < attackCount; num++)
        {
            // getting the index of a square that our piece can move to/capture.
            int attackIndex = popLsb(targetAttacks.board);
            // generating a move with our piece index and the potential square it can move to. 
            Move::Move move = Move::createMove(index, attackIndex);
            pseudoLegalMoves.push_back(move);
        }
    }
    // todo!
    // if rook check for castle!
    // 
}

int MoveGen::popLsb(U64 &board)
{
    int lsbIndex = __builtin_ffsll(board) - 1;
    board &= board - 1;
    return lsbIndex;
}