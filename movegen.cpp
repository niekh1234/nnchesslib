#include <movegen.h>
#include <move.h>
#include <attacks.h>
#include <vector>
#include <board.h>

using namespace nnchesslib;

void nnchesslib::genLegalMoves(ChessBoard board, MoveList& moveList)
{
    // genPseudoLegalMoves(cboard, moveList);
    testXDD();
    // do something XD
}

void nnchesslib::testXDD()
{
    std::cout<<"bruh"<<std::endl;
}

void nnchesslib::genPseudoLegalMoves(ChessBoard board, MoveList& moveList)
{
    BitBoard blockers = board.getBlockers();
    
    if(board.getWhiteToMove())
        genWhiteMoves(board, moveList, blockers);
    else
        genBlackMoves(board, moveList, blockers);
}

void nnchesslib::genWhiteMoves(ChessBoard board, MoveList& moveList, BitBoard blockers)
{
    genWhiteSinglePawnMoves(board,moveList, blockers);
    genWhiteDoublePawnMoves(board,moveList, blockers);
    genWhitePawnCaptures(board,moveList, blockers);
    genNonSlidingMoves(board,moveList, WHITE, KNIGHT);
    genNonSlidingMoves(board,moveList, WHITE, KING);
    genSlidingMoves(board,moveList, WHITE, ROOK, blockers);
    genSlidingMoves(board,moveList, WHITE, BISHOP, blockers);
    genSlidingMoves(board,moveList, WHITE, QUEEN, blockers);
    genCastlingMoves(board,moveList,WHITE, blockers);
}

void nnchesslib::genBlackMoves(ChessBoard board, MoveList& moveList, BitBoard blockers)
{
    //NULL;
}

void nnchesslib::genWhiteSinglePawnMoves(ChessBoard board, MoveList& moveList, BitBoard blockers)
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

        if (index == -1)
            continue;
        // adding to pseudo legal movelist. 
        Move move = createMove(index - 8, index);
        moveList.push_back(move);
    }
    // 
    // todo: pawn promotions.
    // 
}

void nnchesslib::genWhiteDoublePawnMoves(ChessBoard board, MoveList& moveList, BitBoard blockers)
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

        if (index == -1)
            continue;
        // adding to pseudo legal movelist. 
        Move move = createMove(index - 16, index);
        moveList.push_back(move);
    }
}

void nnchesslib::genWhitePawnCaptures(ChessBoard board, MoveList& moveList, BitBoard blockers)
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

        if (index == -1)
            continue;
        
        // getting attacks and comparing to black pieces.
        BitBoard pawnAttacks = Attacks::getNonSlidingAttacks(index, WHITE, PAWN) & black.board;
        if(pawnAttacks.board){
            // getting the amount of pieces a pawn is attacking.
            int attackCount = __builtin_popcountll(pawnAttacks.board);
            
            for(int num = 0; num < attackCount; num++)
            {
                // getting the index of the attacked pawn/piece from black.
                int attackIndex = popLsb(pawnAttacks.board);

                if (attackIndex == -1)
                    continue;
                // generating a move with our pawn index and the index of blacks attacked pawn/piece.
                Move move = createMove(index, attackIndex);
                moveList.push_back(move);
            }
            // TODO
            // promotions with capture.
        }
    }
}

void nnchesslib::genNonSlidingMoves(ChessBoard board, MoveList& moveList, Color color, PieceType piece)
{
    // getting target pieces on the board for a specific color.
    BitBoard targets = board.getBoard(color, piece);

    int count = __builtin_popcountll(targets.board);

    for(int _ = 0; _ < count; _++)
    {
        int index = popLsb(targets.board);

        if (index == -1)
            continue;

        BitBoard targetMoves = Attacks::getNonSlidingAttacks(index, color, piece);
        BitBoard targetAttacks = targetMoves.board & ~board.getBoard(color).board;

        int attackCount = __builtin_popcountll(targetMoves.board);

        if(targetAttacks.board)
        {
            for(int num = 0; num < attackCount; num++)
            {
                // getting the index of a square that our piece can move to/capture.
                int attackIndex = popLsb(targetAttacks.board);

                if (attackIndex == -1)
                    continue;
                // generating a move with our piece index and the potential square it can move to. 
                Move move = createMove(index, attackIndex);       
                moveList.push_back(move);
            }
        }
    }    
}

void nnchesslib::genSlidingMoves(ChessBoard board, MoveList& moveList, Color color, PieceType piece, BitBoard blockers)
{
    // getting target pieces on the board for a specific color.
    BitBoard targets = board.getBoard(color, piece);

    int count = __builtin_popcountll(targets.board);

    for(int _ = 0; _ < count; _++)
    {
        int index = popLsb(targets.board);

        if (index == -1)
            continue;

        BitBoard targetMoves = Attacks::getSlidingAttacks(index, piece, blockers.board);
        BitBoard targetAttacks = targetMoves.board & ~board.getBoard(color).board;

        int attackCount = __builtin_popcountll(targetMoves.board);

        if(targetAttacks.board)
        {
            for(int num = 0; num < attackCount; num++)
            {
                // getting the index of a square that our piece can move to/capture.
                int attackIndex = popLsb(targetAttacks.board);

                if (attackIndex == -1)
                    continue;
                
                // generating a move with our piece index and the potential square it can move to. 
                Move move = createMove(index, attackIndex);
                std::cout<<from_Square(move)<<std::endl;
                moveList.push_back(move);
            }
        }
    }
    // todo!
    // if rook check for castle!
    // 
}

void nnchesslib::genCastlingMoves(ChessBoard board, MoveList& moveList, Color color, BitBoard blockers)
{
    if(color == WHITE)
    {
        // white queenside:
        if(board.whiteCastleLong && !blockers.get(B1) && !blockers.get(C1) && !blockers.get(D1))
        {
            Move move = createMove(E1, C1);
            moveList.push_back(move);
            std::cout<<"white qs\n";
        } 
        // white kingside:
        if(board.whiteCastleLong && !blockers.get(F1) && !blockers.get(G1))
        {
            Move move = createMove(E1, G1);
            moveList.push_back(move);
            std::cout<<"white ks\n";
        }
    }
    if(color == BLACK)
    {
        // black queenside:
        if(board.blackCastleLong && !blockers.get(B8) && !blockers.get(C8) && !blockers.get(D8))
        {
            Move move = createMove(E8, C8);
            moveList.push_back(move);
            std::cout<<"black qs\n";
        } 
        // black kingside:
        if(board.blackCastleLong && !blockers.get(F8) && !blockers.get(G8))
        {
            Move move = createMove(E8, G8);
            moveList.push_back(move);
            std::cout<<"black ks\n";
        }
    }
}

int nnchesslib::popLsb(U64 &board)
{
    int lsbIndex = __builtin_ffsll(board) - 1;
    board &= board - 1;
    return lsbIndex;
}