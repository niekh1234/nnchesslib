#include <bitboard.h>
#include <board.h>
#include <string>
#include <sstream>
#include <iostream>
#include <bits/stdc++.h>
#include <cassert>
#include <algorithm>
#include <types.h>

using namespace nnchesslib;

// Constructors
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
            if (board[i] >= 'A' && board[i] <= 'Z') white_pieces.set(bitIndex, true);
            if (board[i] >= 'a' && board[i] <= 'z') black_pieces.set(bitIndex, true);

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
    // BAD BAD BAD BAD BAD BAD BAD BAD BAD BAD BAD BAD BAD
    // cannot think of another way to do this yet.
    // The problem is that the code above generates the board but mirrorred vertically.
    knights = knights.flipVertical();
    bishops = bishops.flipVertical();
    rooks = rooks.flipVertical();
    queens = queens.flipVertical();
    kings = kings.flipVertical();
    pawns = pawns.flipVertical();
}

//function for printing / combining all the bitboards to form a readable board. 
void ChessBoard::print(){

}

BitBoard ChessBoard::getBlockers()
{
    return (rooks.board | knights.board | bishops.board | kings.board | queens.board | pawns.board);
}
