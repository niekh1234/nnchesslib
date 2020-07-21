#include <bitboard.h>
#include <board.h>
#include <string>
#include <sstream>
#include <iostream>
#include <bits/stdc++.h>
#include <cassert>
#include <algorithm>
#include <types.h>
#include <move.h>
#include <attacks.h>

using namespace nnchesslib;

// Constructors
ChessBoard::ChessBoard()
{
    generateBitBoards(STARTING_FEN);
}

ChessBoard::ChessBoard(std::string fen)
{
    std::string valid_fen;

    if (!isValidFen(fen))
        valid_fen = STARTING_FEN;
    else
        valid_fen = fen;

    generateBitBoards(valid_fen);
}

bool ChessBoard::isValidFen(std::string fen)
{
    // Setup variables
    std::istringstream iss(fen);
    std::string board, sideToMove, castleRights, enpassant;

    // check if fen is provided
    if(!iss) return false;

    iss >> board;

    if(!iss) return false;

    iss >> sideToMove;

    if (!iss) 
    {
        castleRights = "-";
        enpassant = "-";
    } 
    else 
    {
        iss >> castleRights;
        if (iss)
            iss >> enpassant;
        else
            enpassant = "-";
    }

    // Let's check that all components of the supposed FEN are OK.
    // Check side to move
    if (sideToMove != "w" && sideToMove != "b") return false;
    
    // Check castling rights
    if (castleRights != "-" && castleRights != "K" && castleRights != "Kk"
        && castleRights != "Kkq" && castleRights != "Kq" && castleRights !="KQ"
        && castleRights != "KQk" && castleRights != "KQq" && castleRights != "KQkq"
        && castleRights != "k" && castleRights != "q" && castleRights != "kq"
        && castleRights != "Q" && castleRights != "Qk" && castleRights != "Qq"
        && castleRights != "Qkq")
        return false;

    // Check valid en passant
    if (enpassant != "-") {
        if (enpassant.length() != 2) return false;
        if (!(enpassant[0] >= 'a' && enpassant[0] <= 'h')) return false;
        if (!((sideToMove == "w" && enpassant[1] == '6') || (sideToMove == "b" && enpassant[1] == '3')))
            return false;
    }

    // Check valid board
    // Seven board dividers
    if (std::count(board.begin(), board.end(), '/') != 7) return false;

    // Board contains only legal characters
    for (int i = 0; i < board.length(); i++)
        if (!(board[i] == '/' || (board[i] >= '1' && board[i] <= '8')
                || board[i] == 'p' || board[i] == 'P' || board[i] == 'r' || board[i] == 'R' || 
                board[i] == 'k' || board[i] == 'K' || board[i] == 'n' || board[i] == 'N' || board[i] == 'b' || board[i] == 'B'
                || board[i] == 'q' || board[i] == 'Q'))
            return false;
    
    // Check king amount
    if (std::count(board.begin(), board.end(), 'K') != 1) return false;
    if (std::count(board.begin(), board.end(), 'k') != 1) return false;

    // Count other piece amounts
    size_t wp = std::count(board.begin(), board.end(), 'P'),
        bp = std::count(board.begin(), board.end(), 'p'),
        wn = std::count(board.begin(), board.end(), 'N'),
        bn = std::count(board.begin(), board.end(), 'n'),
        wb = std::count(board.begin(), board.end(), 'B'),
        bb = std::count(board.begin(), board.end(), 'b'),
        wr = std::count(board.begin(), board.end(), 'R'),
        br = std::count(board.begin(), board.end(), 'r'),
        wq = std::count(board.begin(), board.end(), 'Q'),
        bq = std::count(board.begin(), board.end(), 'q');

    // Check if piece count is valid
    if (wp > 8 || bp > 8 || wn > 10 || bn > 10 || wb > 10 || bb > 10
        || wr > 10 || br > 10 || wq > 9 || bq > 9
        || wp + wn + wb + wr + wq > 15 || bp + bn + bb + br + bq > 15)
        return false;

    // setting some variables now that all the checks have been passed:
    // put this in dedicated function in the future.

    // changing who's turn it is.
    if(sideToMove == "w") whiteToMove = true;
    else whiteToMove = false;

    // setting the castling rights bools
    for (auto c : castleRights)
    {
        switch(c)
        {
            case 'K':
                whiteCastleShort = true;
                break;
            case 'Q':
                whiteCastleLong = true;
                break;
            case 'k':
                blackCastleShort = true;
                break;
            case 'q':
                blackCastleLong = true;
                break;
        }
    }
    // All checks passed
    return true;
}


void ChessBoard::generateBitBoards(std::string fen)
{
    assert(isValidFen(fen));

    std::istringstream iss(fen);
    std::string board;
    // getting the board (first) part of the fen string.
    iss >> board;

    // removing all the '/' from the board string.
    board.erase(std::remove(board.begin(), board.end(), '/'), board.end());

    // the idea is to increment bitIndex by 1 if a letter is occuring in the board string.
    // and to increment it by however much the number says when it is not a letter.
    int bitIndex = 0;
    for(int i = 0; i < board.size(); i++)
    {
        if(isdigit(board[i]))
            bitIndex += (int(board[i]) - 48);

        else if (isalpha(board[i]))
        {
            // Generate bitboard for white and black
            // this is reversed atm.
            if (board[i] >= 'A' && board[i] <= 'Z') whitePieces.set(bitIndex, true);
            if (board[i] >= 'a' && board[i] <= 'z') blackPieces.set(bitIndex, true);

            // Generate bitboard for pieces
            if (board[i] == 'N' || board[i] == 'n') knights.set(bitIndex, true);
            if (board[i] == 'B' || board[i] == 'b') bishops.set(bitIndex, true);
            if (board[i] == 'R' || board[i] == 'r') rooks.set(bitIndex, true);
            if (board[i] == 'Q' || board[i] == 'q') queens.set(bitIndex, true);
            if (board[i] == 'K' || board[i] == 'k') kings.set(bitIndex, true);
            if (board[i] == 'P' || board[i] == 'p') pawns.set(bitIndex, true);

            bitIndex += 1;
        }
    }
    // cannot think of another way to do this yet.
    // The problem is that the code above generates the board but mirrorred vertically.
    knights = knights.flipVertical();
    bishops = bishops.flipVertical();
    rooks = rooks.flipVertical();
    queens = queens.flipVertical();
    kings = kings.flipVertical();
    pawns = pawns.flipVertical();
    whitePieces = whitePieces.flipVertical();
    blackPieces = blackPieces.flipVertical();
}

//function for printing / combining all the bitboards to form a readable board. 
void ChessBoard::print()
{
    std::string output;
    std::string finalOutput;

    BitBoard occupied = whitePieces.board | blackPieces.board;
    for(int i = 0; i <= 63; i++)
    {
        if(occupied.get(i))
        {
            if(BitBoard(pawns.board & whitePieces.board).get(i)) output+=" P ";
            if(BitBoard(knights.board & whitePieces.board).get(i)) output+=" N ";
            if(BitBoard(bishops.board & whitePieces.board).get(i)) output+=" B ";
            if(BitBoard(rooks.board & whitePieces.board).get(i)) output+=" R ";
            if(BitBoard(queens.board & whitePieces.board).get(i)) output+=" Q ";
            if(BitBoard(kings.board & whitePieces.board).get(i)) output+=" K ";
            if(BitBoard(pawns.board & blackPieces.board).get(i)) output+=" p ";
            if(BitBoard(knights.board & blackPieces.board).get(i)) output+=" n ";
            if(BitBoard(bishops.board & blackPieces.board).get(i)) output+=" b ";
            if(BitBoard(rooks.board & blackPieces.board).get(i)) output+=" r ";
            if(BitBoard(queens.board & blackPieces.board).get(i)) output+=" q ";
            if(BitBoard(kings.board & blackPieces.board).get(i)) output+=" k ";
        } 
        else
        {
            output+=" . ";
        }
        if((i + 1) % 8 == 0){
            finalOutput.insert(0, output + "\n");
            output = "";
        }
    }

    finalOutput.insert(0, " -  -  -  -  -  -  -  - \n");
    finalOutput+=" -  -  -  -  -  -  -  - ";
    std::cout<<finalOutput<<std::endl;
}

BitBoard ChessBoard::getBoard(Color color, PieceType piece)
{
    assert(color == WHITE || color == BLACK);

    U64 boardColor = whitePieces.board;
    if(color == BLACK) boardColor = blackPieces.board;

    switch(piece){
        case PAWN:
            return (boardColor & pawns.board);
            break;
        case KNIGHT:
            return (boardColor & knights.board);
            break;
        case BISHOP:
            return (boardColor & bishops.board);
            break;
        case ROOK:
            return (boardColor & rooks.board);
            break;
        case QUEEN:
            return (boardColor & queens.board);
            break;
        case KING:
            return (boardColor & kings.board);
            break;
        default:
            return (whitePieces.board & pawns.board);
            break;
    }
}

BitBoard ChessBoard::getBoard(Color color)
{
    assert(color == WHITE || color == BLACK);

    if(color == WHITE) return (whitePieces.board);
    return (blackPieces.board);
}

BitBoard ChessBoard::getBlockers()
{
    return (whitePieces.board | blackPieces.board);
}

bool ChessBoard::getWhiteToMove()
{
    return whiteToMove;
}

BitBoard * ChessBoard::getPieceOnSquare(int index)
{
    if(pawns.get(index)) return(&pawns);
    if(knights.get(index)) return(&knights);
    if(bishops.get(index)) return(&bishops);
    if(rooks.get(index)) return(&rooks);
    if(queens.get(index)) return(&queens);
    if(kings.get(index)) return(&kings);
    return 0;
}

BitBoard * ChessBoard::getColorOnSquare(int index)
{
    if(whitePieces.get(index)) return(&whitePieces);
    if(blackPieces.get(index)) return(&blackPieces);
    return 0;
}

bool ChessBoard::kingInCheck(Color color)
{
    BitBoard kingBoard = getBoard(color, KING);
    int kingSquare = __builtin_ffsll(kingBoard.board) - 1;

    return squareAttacked(kingSquare, color);
}

// idea is to generate a bitboard where all opponent attacks are a 1.
bool ChessBoard::squareAttacked(int square, Color color)
{
    BitBoard pawnAttacks = Attacks::getNonSlidingAttacks(square, color, PAWN);
    if(pawnAttacks.board & getBoard(getOppositeColor(color), PAWN).board) return true;

    BitBoard knightAttacks = Attacks::getNonSlidingAttacks(square, color, KNIGHT);
    if(knightAttacks.board & getBoard(getOppositeColor(color), KNIGHT).board) return true;

    BitBoard kingAttacks = Attacks::getNonSlidingAttacks(square, color, KING);
    if(kingAttacks.board & getBoard(getOppositeColor(color), KING).board) return true;

    BitBoard bishopAttacks = Attacks::getSlidingAttacks(square, BISHOP, getBlockers().board);
    if(bishopAttacks.board & getBoard(getOppositeColor(color), BISHOP).board) return true;

    BitBoard rookAttacks = Attacks::getSlidingAttacks(square, ROOK, getBlockers().board);
    if(rookAttacks.board & getBoard(getOppositeColor(color), ROOK).board) return true;

    BitBoard queenAttacks = Attacks::getSlidingAttacks(square, QUEEN, getBlockers().board);
    if(queenAttacks.board & getBoard(getOppositeColor(color), QUEEN).board) return true;

    return false;
}

void ChessBoard::updateCastlingRights()
{
    // if the rooks have moved:
    bool whiteQSRook = BitBoard(whitePieces.board & rooks.board).get(A1);
    bool whiteKSRook = BitBoard(whitePieces.board & rooks.board).get(H1);
    bool blackQSRook = BitBoard(blackPieces.board & rooks.board).get(A8);
    bool blackKSRook = BitBoard(blackPieces.board & rooks.board).get(H8);
    // castling rights can only go from true to false. If you move a rook back into proper position you cannot castle anymore.
    if(!whiteQSRook && whiteCastleLong) whiteCastleLong = false;
    if(!whiteKSRook && whiteCastleShort) whiteCastleShort = false;
    if(!blackQSRook && blackCastleLong) blackCastleLong = false;
    if(!blackKSRook && blackCastleShort) blackCastleShort = false;

    // if the kings have moved:
    bool whiteKing = BitBoard(whitePieces.board & kings.board).get(E1);
    bool blackKing = BitBoard(blackPieces.board & kings.board).get(E8);
    if(!whiteKing && (whiteCastleLong || whiteCastleShort))
    {
        whiteCastleLong = false;
        whiteCastleShort = false;
    } else if (!blackKing && (blackCastleLong || blackCastleShort))
    {
        blackCastleLong = false;
        blackCastleShort = false;
    }

}

void ChessBoard::pushCastlingMove(Move move)
{
    int from = from_Square(move);
    int to = to_Square(move);
    // gets the bitboard corresponding to our piece color.
    BitBoard * ourPieces = getColorOnSquare(from);

    // I already thought of a more efficient way of writing this but cannot be asked at the moment. + this is probably quite fast.
    // white ks castle
    if(to == 6)
    {
        kings.set(E1, false);
        rooks.set(H1, false);
        kings.set(G1, true);
        rooks.set(F1, true);
        ourPieces->set(E1, false);
        ourPieces->set(H1, false);
        ourPieces->set(G1, true);
        ourPieces->set(F1, true);
    } 
    // white qs castle
    else if (to == 0)
    {
        kings.set(E1, false);
        rooks.set(A1, false);
        kings.set(C1, true);
        rooks.set(D1, true);
        ourPieces->set(E1, false);
        ourPieces->set(A1, false);
        ourPieces->set(C1, true);
        ourPieces->set(D1, true);
    }
    else if (to == 56)
    {
        kings.set(E8, false);
        rooks.set(H8, false);
        kings.set(G8, true);
        rooks.set(F8, true);
        ourPieces->set(E8, false);
        ourPieces->set(H8, false);
        ourPieces->set(G8, true);
        ourPieces->set(F8, true);
    } 
    // black qs castle
    else if (to == 63)
    {
        kings.set(E8, false);
        rooks.set(A8, false);
        kings.set(C8, true);
        rooks.set(D8, true);
        ourPieces->set(E8, false);
        ourPieces->set(A8, false);
        ourPieces->set(C8, true);
        ourPieces->set(D8, true);
    }

    std::cout<<from<<" to "<<to<<std::endl;
}

void ChessBoard::pushPromotionMove(Move move)
{
    int from = from_Square(move);
    int to = to_Square(move);

    std::cout<<(from, to)<<std::endl;
}

void ChessBoard::pushMove(Move move)
{
    // getting the movetype
    MoveType type = moveType(move);

    if(type == CASTLING)
    {
        pushCastlingMove(move);
        return;
    } 
    else if(type == PROMOTION)
    {
        pushPromotionMove(move);
        return;
    }

    int from = from_Square(move);
    int to = to_Square(move);

    // gets the bitboard corresponding to our piecetype, e.g. knights, pawns etc.
    BitBoard * ourPieceType = getPieceOnSquare(from);
    // gets the bitboard corresponding to the opponent piecetype (at least if it is a capture).
    BitBoard * theirPieceType = getPieceOnSquare(to);
    // if board is empty it returns 0.
    bool isCapture = theirPieceType;
    // gets the bitboard corresponding to our piece color.
    BitBoard * ourPieces = getColorOnSquare(from);

    if(isCapture)
    {
        theirPieceType->set(to, false);

        BitBoard * theirPieces = getColorOnSquare(to);
        theirPieces->set(to, false);
    }
    // changing to the position of a piece on one of the piece boards: pawn, knight, rook king etc.
    ourPieceType->set(from, false);
    ourPieceType->set(to, true);
    // changing the position of the piece on the white_pieces or black_pieces board.
    ourPieces->set(from, false);
    ourPieces->set(to, true);

    updateCastlingRights();
}

// removes one move from the list.
void ChessBoard::popMove()
{

}

Color ChessBoard::getOppositeColor(Color color)
{
    if(color == WHITE) return BLACK;
    return WHITE;
}

