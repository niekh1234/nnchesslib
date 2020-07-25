#include <movegen.h>
#include <move.h>
#include <attacks.h>
#include <vector>
#include <board.h>

using namespace nnchesslib;

MoveList nnchesslib::genLegalMoves(ChessBoard board)
{
    MoveList pseudoLegalMoves;
    MoveList legalMoves;

    genPseudoLegalMoves(board, pseudoLegalMoves);
    
    for(auto move : pseudoLegalMoves)
    {
        ChessBoard temp = board;
        temp.pushMove(move);

        Color boardTurn = BLACK;
        if(board.getWhiteToMove()) boardTurn = WHITE;

        if(!temp.kingInCheck(boardTurn)) 
        {
            legalMoves.push_back(move);
        }
    }
    return legalMoves;
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
    genCastlingMoves(board,moveList, WHITE, blockers);
}

void nnchesslib::genBlackMoves(ChessBoard board, MoveList& moveList, BitBoard blockers)
{
    genBlackSinglePawnMoves(board,moveList, blockers);
    genBlackDoublePawnMoves(board,moveList, blockers);
    genBlackPawnCaptures(board,moveList, blockers);
    genNonSlidingMoves(board,moveList, BLACK, KNIGHT);
    genNonSlidingMoves(board,moveList, BLACK, KING);
    genSlidingMoves(board,moveList, BLACK, ROOK, blockers);
    genSlidingMoves(board,moveList, BLACK, BISHOP, blockers);
    genSlidingMoves(board,moveList, BLACK, QUEEN, blockers);
    genCastlingMoves(board,moveList, BLACK, blockers);
}

void nnchesslib::genWhiteSinglePawnMoves(ChessBoard board, MoveList& moveList, BitBoard blockers)
{
    BitBoard pawns = board.getBoard(WHITE, PAWN);
    // making a move and checking if pawns have been blocked
    BitBoard pawnsMoved = (pawns.board << 8) & ~blockers.board;

    BitBoard promotedPawns = pawnsMoved.board & rank_bb[RANK_8];
    // make a copy because we still want to use promotedpawns bitboard after lsb has been popped.
    BitBoard promotedPawnsCopy = promotedPawns;
    int promotedPawnCount = __builtin_popcountll(promotedPawns.board);

    for(int _ =  0; _ < promotedPawnCount; _++)
    {
        int index = popLsb(promotedPawnsCopy.board);
        // genPromotions function only works for a single index in the bitboard.
        BitBoard singlePawnBoard;
        singlePawnBoard.set(index, true);

        genPromotions(index - 8, moveList, WHITE, singlePawnBoard);
    }

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

        // getting en passant move by looking at if a white pawn is attacking a predefined en passant square ('under' the pawn, actually it is above the pawn because it is a black pawn)
        BitBoard enPassant = Attacks::getNonSlidingAttacks(index, WHITE, PAWN) & board.boardinfo.whiteEnPassantTarget.board;
        if(enPassant.board)
        {
            int epIndex = popLsb(enPassant.board);

            Move enPassantMove = createMove(index, epIndex, ENPASSANT);
            moveList.push_back(enPassantMove);
        }

        BitBoard promotedPawns = pawnAttacks.board & rank_bb[RANK_8];
        // if there is an attack generate promotion moves.
        if(promotedPawns.board)
            genPromotions(index, moveList, WHITE, promotedPawns);

        // removing attacks on the eighth rank to prevent non promotions moves from being added.
        pawnAttacks.board &= ~rank_bb[RANK_8];
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
        }
    }
}

void nnchesslib::genBlackSinglePawnMoves(ChessBoard board, MoveList& moveList, BitBoard blockers)
{
    BitBoard pawns = board.getBoard(BLACK, PAWN);
    // making a move and checking if pawns have been blocked
    BitBoard pawnsMoved = (pawns.board >> 8) & ~blockers.board;

    BitBoard promotedPawns = pawnsMoved.board & rank_bb[RANK_1];
    // make a copy because we still want to use promotedpawns bitboard after lsb has been popped.
    BitBoard promotedPawnsCopy = promotedPawns;
    int promotedPawnCount = __builtin_popcountll(promotedPawns.board);

    for(int _ =  0; _ < promotedPawnCount; _++)
    {
        int index = popLsb(promotedPawnsCopy.board);
        // genPromotions function only works for a single index in the bitboard.
        BitBoard singlePawnBoard;
        singlePawnBoard.set(index, true);

        genPromotions(index + 8, moveList, BLACK, singlePawnBoard);
    }

    pawnsMoved.board &= ~rank_bb[RANK_1];

    int validPawnCount = __builtin_popcountll(pawnsMoved.board);

    for(int _ = 0; _ < validPawnCount; _++)
    {
        // finding current index of pawn.
        int index = popLsb(pawnsMoved.board);

        if (index == -1)
            continue;
        // adding to pseudo legal movelist. 
        Move move = createMove(index + 8, index);
        moveList.push_back(move);
    }
}

void nnchesslib::genBlackDoublePawnMoves(ChessBoard board, MoveList& moveList, BitBoard blockers)
{
    // targetting pawns that are on the second rank. These are the only pawns that can move twice.
    BitBoard unmovedPawns = board.getBoard(BLACK, PAWN).board & rank_bb[RANK_7];
    // moving pawns once to check if they are blocked:
    BitBoard firstMove = (unmovedPawns.board >> 8) & ~blockers.board;
    // moving again:
    BitBoard secondMove = (firstMove.board >> 8) & ~blockers.board;

    int validPawnCount = __builtin_popcountll(secondMove.board);

    for(int _ = 0; _ < validPawnCount; _++)
    {
        // finding current index of pawn.
        int index = popLsb(secondMove.board);

        if (index == -1)
            continue;
        // adding to pseudo legal movelist. 
        Move move = createMove(index + 16, index);
        moveList.push_back(move);
    }
}

void nnchesslib::genBlackPawnCaptures(ChessBoard board, MoveList& moveList, BitBoard blockers)
{
    // getting board for black pawns.
    BitBoard pawns = board.getBoard(BLACK, PAWN).board;
    BitBoard white = board.getBoard(WHITE);
    // counting the amount of white pawns.
    int pawnCount = __builtin_popcountll(pawns.board);

    for(int _ = 0; _ < pawnCount; _++)
    {
        // getting the index of a specific pawn and removing it from the board.
        int index = popLsb(pawns.board);

        if (index == -1)
            continue;
        
        // getting attacks and comparing to white pieces.
        BitBoard pawnAttacks = Attacks::getNonSlidingAttacks(index, BLACK, PAWN) & white.board;
        // checking whether there is an attack on the eighth rank.

        // getting en passant move by looking at if a white pawn is attacking a predefined en passant square ('above' the pawn, actually it is above the pawn because it is a white pawn)
        BitBoard enPassant = Attacks::getNonSlidingAttacks(index, BLACK, PAWN) & board.boardinfo.blackEnPassantTarget.board;
        if(enPassant.board)
        {
            int epIndex = popLsb(enPassant.board);

            Move enPassantMove = createMove(index, epIndex, ENPASSANT);
            moveList.push_back(enPassantMove);
        }

        BitBoard promotedPawns = pawnAttacks.board & rank_bb[RANK_1];
        // if there is an attack generate promotion moves.
        if(promotedPawns.board)
            genPromotions(index, moveList, BLACK, promotedPawns);

        // removing attacks on the eighth rank to prevent non promotions moves from being added.
        pawnAttacks.board &= ~rank_bb[RANK_1];
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
                moveList.push_back(move);
            }
        }
    }
}

void nnchesslib::genPromotions(int pawnIndex, MoveList& moveList, Color color, BitBoard pawns)
{
    // counting the amount of attacked pawns.
    int count = __builtin_popcountll(pawns.board);

    for(int _ = 0; _ < count; _++)
    {
        int index = popLsb(pawns.board);
        
        Move queen = createMove(pawnIndex, index, QUEEN);
        Move knight = createMove(pawnIndex, index, KNIGHT);
        Move bishop = createMove(pawnIndex, index, BISHOP);
        Move rook = createMove(pawnIndex, index, ROOK);
        moveList.push_back(queen);
        moveList.push_back(knight);
        moveList.push_back(bishop);
        moveList.push_back(rook);
    }
}

void nnchesslib::genCastlingMoves(ChessBoard board, MoveList& moveList, Color color, BitBoard blockers)
{
    if(color == WHITE)
    {
        // white queenside:
        bool attackedSquaresQS = board.squareAttacked(C1, color) || board.squareAttacked(D1, color) || board.squareAttacked(E1, color);
        if(board.boardinfo.whiteCastleLong && !blockers.get(B1) && !blockers.get(C1) && !blockers.get(D1) && !attackedSquaresQS)
        {
            Move move = createMove(E1, C1, CASTLING);
            moveList.push_back(move);
        } 
        // white kingside:
        bool attackedSquaresKS = board.squareAttacked(F1, color) || board.squareAttacked(G1, color) || board.squareAttacked(E1, color);
        if(board.boardinfo.whiteCastleShort && !blockers.get(F1) && !blockers.get(G1) && !attackedSquaresKS)
        {
            Move move = createMove(E1, G1, CASTLING);
            moveList.push_back(move);
        }
    }
    if(color == BLACK)
    {
        // black queenside:
        bool attackedSquaresQS = board.squareAttacked(C8, color) || board.squareAttacked(D8, color) || board.squareAttacked(E8, color);
        if(board.boardinfo.blackCastleLong && !blockers.get(B8) && !blockers.get(C8) && !blockers.get(D8) && !attackedSquaresQS)
        {
            Move move = createMove(E8, C8, CASTLING);
            moveList.push_back(move);
        } 
        // black kingside:
        bool attackedSquaresKS = board.squareAttacked(F8, color) || board.squareAttacked(G8, color) || board.squareAttacked(E8, color);
        if(board.boardinfo.blackCastleShort && !blockers.get(F8) && !blockers.get(G8) && !attackedSquaresKS)
        {
            Move move = createMove(E8, G8, CASTLING);
            moveList.push_back(move);
        }
    }
}

int nnchesslib::popLsb(U64 &board)
{
    int lsbIndex = __builtin_ffsll(board) - 1;
    board &= board - 1;
    return lsbIndex;
}