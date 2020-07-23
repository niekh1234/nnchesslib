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
#include <utils.h>

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
    if(sideToMove == "w") boardinfo.whiteToMove = true;
    else boardinfo.whiteToMove = false;

    // setting the castling rights bools
    for (auto c : castleRights)
    {
        switch(c)
        {
            case 'K':
                boardinfo.whiteCastleShort = true;
                break;
            case 'Q':
                boardinfo.whiteCastleLong = true;
                break;
            case 'k':
                boardinfo.blackCastleShort = true;
                break;
            case 'q':
                boardinfo.blackCastleLong = true;
                break;
        }
    }

    // setting en passant bitboard.
    if(enpassant != "-")
    {
        if(boardinfo.whiteToMove)  boardinfo.whiteEnPassantTarget.set(getSquareInt(enpassant), true);
        if(!boardinfo.whiteToMove) boardinfo.blackEnPassantTarget.set(getSquareInt(enpassant), true);
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
            if (board[i] >= 'A' && board[i] <= 'Z') boardinfo.whitePieces.set(bitIndex, true);
            if (board[i] >= 'a' && board[i] <= 'z') boardinfo.blackPieces.set(bitIndex, true);

            // Generate bitboard for pieces
            if (board[i] == 'N' || board[i] == 'n') boardinfo.knights.set(bitIndex, true);
            if (board[i] == 'B' || board[i] == 'b') boardinfo.bishops.set(bitIndex, true);
            if (board[i] == 'R' || board[i] == 'r') boardinfo.rooks.set(bitIndex, true);
            if (board[i] == 'Q' || board[i] == 'q') boardinfo.queens.set(bitIndex, true);
            if (board[i] == 'K' || board[i] == 'k') boardinfo.kings.set(bitIndex, true);
            if (board[i] == 'P' || board[i] == 'p') boardinfo.pawns.set(bitIndex, true);

            bitIndex += 1;
        }
    }
    // cannot think of another way to do this yet.
    // The problem is that the code above generates the board but mirrorred vertically.
    boardinfo.knights = boardinfo.knights.flipVertical();
    boardinfo.bishops = boardinfo.bishops.flipVertical();
    boardinfo.rooks = boardinfo.rooks.flipVertical();
    boardinfo.queens = boardinfo.queens.flipVertical();
    boardinfo.kings = boardinfo.kings.flipVertical();
    boardinfo.pawns = boardinfo.pawns.flipVertical();
    boardinfo.whitePieces = boardinfo.whitePieces.flipVertical();
    boardinfo.blackPieces = boardinfo.blackPieces.flipVertical();
}

//function for printing / combining all the bitboards to form a readable board. 
void ChessBoard::print()
{
    std::string output;
    std::string finalOutput;

    BitBoard occupied = boardinfo.whitePieces.board | boardinfo.blackPieces.board;
    for(int i = 0; i <= 63; i++)
    {
        if(occupied.get(i))
        {
            if(BitBoard(boardinfo.pawns.board & boardinfo.whitePieces.board).get(i)) output+=" P ";
            if(BitBoard(boardinfo.knights.board & boardinfo.whitePieces.board).get(i)) output+=" N ";
            if(BitBoard(boardinfo.bishops.board & boardinfo.whitePieces.board).get(i)) output+=" B ";
            if(BitBoard(boardinfo.rooks.board & boardinfo.whitePieces.board).get(i)) output+=" R ";
            if(BitBoard(boardinfo.queens.board & boardinfo.whitePieces.board).get(i)) output+=" Q ";
            if(BitBoard(boardinfo.kings.board & boardinfo.whitePieces.board).get(i)) output+=" K ";
            if(BitBoard(boardinfo.pawns.board & boardinfo.blackPieces.board).get(i)) output+=" p ";
            if(BitBoard(boardinfo.knights.board & boardinfo.blackPieces.board).get(i)) output+=" n ";
            if(BitBoard(boardinfo.bishops.board & boardinfo.blackPieces.board).get(i)) output+=" b ";
            if(BitBoard(boardinfo.rooks.board & boardinfo.blackPieces.board).get(i)) output+=" r ";
            if(BitBoard(boardinfo.queens.board & boardinfo.blackPieces.board).get(i)) output+=" q ";
            if(BitBoard(boardinfo.kings.board & boardinfo.blackPieces.board).get(i)) output+=" k ";
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

    U64 boardColor = boardinfo.whitePieces.board;
    if(color == BLACK) boardColor = boardinfo.blackPieces.board;

    switch(piece){
        case PAWN:
            return (boardColor & boardinfo.pawns.board);
            break;
        case KNIGHT:
            return (boardColor & boardinfo.knights.board);
            break;
        case BISHOP:
            return (boardColor & boardinfo.bishops.board);
            break;
        case ROOK:
            return (boardColor & boardinfo.rooks.board);
            break;
        case QUEEN:
            return (boardColor & boardinfo.queens.board);
            break;
        case KING:
            return (boardColor & boardinfo.kings.board);
            break;
        default:
            return (boardinfo.whitePieces.board & boardinfo.pawns.board);
            break;
    }
}

BitBoard ChessBoard::getBoard(Color color)
{
    assert(color == WHITE || color == BLACK);

    if(color == WHITE) return (boardinfo.whitePieces.board);
    return (boardinfo.blackPieces.board);
}

BitBoard ChessBoard::getBlockers()
{
    return (boardinfo.whitePieces.board | boardinfo.blackPieces.board);
}

bool ChessBoard::getWhiteToMove()
{
    return boardinfo.whiteToMove;
}

BitBoard * ChessBoard::getPieceOnSquare(int index)
{
    if(boardinfo.pawns.get(index)) return(&boardinfo.pawns);
    if(boardinfo.knights.get(index)) return(&boardinfo.knights);
    if(boardinfo.bishops.get(index)) return(&boardinfo.bishops);
    if(boardinfo.rooks.get(index)) return(&boardinfo.rooks);
    if(boardinfo.queens.get(index)) return(&boardinfo.queens);
    if(boardinfo.kings.get(index)) return(&boardinfo.kings);
    return 0;
}

BitBoard * ChessBoard::getColorOnSquare(int index)
{
    if(boardinfo.whitePieces.get(index)) return(&boardinfo.whitePieces);
    if(boardinfo.blackPieces.get(index)) return(&boardinfo.blackPieces);
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

void ChessBoard::setEnPassantPossibility(BitBoard ourPieces, int from, int to)
{
    // There can only be one target at the time, so clearing the boards every move.
    boardinfo.whiteEnPassantTarget.board = (U64)0;
    boardinfo.blackEnPassantTarget.board = (U64)0;
    // black en passant possibility (white has double moved)
    if((from / 8 == 1 && to / 8 == 3) && ourPieces.board & boardinfo.pawns.board)
    {
        // set the square under the pawn as a target for a black pawn (in a seperate bitboard)
        boardinfo.blackEnPassantTarget.set(to - 8, true);
    }
    // white en passant possibility (black has double moved)
    else if((from / 8 == 6 && to / 8 == 4) && ourPieces.board & boardinfo.pawns.board)
    {
        boardinfo.whiteEnPassantTarget.set(to + 8, true);
    }
}

void ChessBoard::updateCastlingRights()
{
    // if the rooks have moved:
    bool whiteQSRook = BitBoard(boardinfo.whitePieces.board & boardinfo.rooks.board).get(A1);
    bool whiteKSRook = BitBoard(boardinfo.whitePieces.board & boardinfo.rooks.board).get(H1);
    bool blackQSRook = BitBoard(boardinfo.blackPieces.board & boardinfo.rooks.board).get(A8);
    bool blackKSRook = BitBoard(boardinfo.blackPieces.board & boardinfo.rooks.board).get(H8);
    // castling rights can only go from true to false. If you move a rook back into proper position you cannot castle anymore.
    if(!whiteQSRook && boardinfo.whiteCastleLong) boardinfo.whiteCastleLong = false;
    if(!whiteKSRook && boardinfo.whiteCastleShort) boardinfo.whiteCastleShort = false;
    if(!blackQSRook && boardinfo.blackCastleLong) boardinfo.blackCastleLong = false;
    if(!blackKSRook && boardinfo.blackCastleShort) boardinfo.blackCastleShort = false;

    // if the kings have moved:
    bool whiteKing = BitBoard(boardinfo.whitePieces.board & boardinfo.kings.board).get(E1);
    bool blackKing = BitBoard(boardinfo.blackPieces.board & boardinfo.kings.board).get(E8);
    if(!whiteKing && (boardinfo.whiteCastleLong || boardinfo.whiteCastleShort))
    {
        boardinfo.whiteCastleLong = false;
        boardinfo.whiteCastleShort = false;
    } else if (!blackKing && (boardinfo.blackCastleLong || boardinfo.blackCastleShort))
    {
        boardinfo.blackCastleLong = false;
        boardinfo.blackCastleShort = false;
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
    if(to == 6 && boardinfo.whiteCastleShort)
    {
        boardinfo.kings.set(E1, false);
        boardinfo.rooks.set(H1, false);
        boardinfo.kings.set(G1, true);
        boardinfo.rooks.set(F1, true);
        ourPieces->set(E1, false);
        ourPieces->set(H1, false);
        ourPieces->set(G1, true);
        ourPieces->set(F1, true);
    } 
    // white qs castle
    else if (to == 2 && boardinfo.whiteCastleLong)
    {
        boardinfo.kings.set(E1, false);
        boardinfo.rooks.set(A1, false);
        boardinfo.kings.set(C1, true);
        boardinfo.rooks.set(D1, true);
        ourPieces->set(E1, false);
        ourPieces->set(A1, false);
        ourPieces->set(C1, true);
        ourPieces->set(D1, true);
    }
    // black qs castle
    else if (to == 58 && boardinfo.blackCastleLong)
    {
        boardinfo.kings.set(E8, false);
        boardinfo.rooks.set(H8, false);
        boardinfo.kings.set(G8, true);
        boardinfo.rooks.set(F8, true);
        ourPieces->set(E8, false);
        ourPieces->set(H8, false);
        ourPieces->set(G8, true);
        ourPieces->set(F8, true);
    } 
    // black ks castle
    else if (to == 62 && boardinfo.blackCastleShort)
    {
        boardinfo.kings.set(E8, false);
        boardinfo.rooks.set(A8, false);
        boardinfo.kings.set(C8, true);
        boardinfo.rooks.set(D8, true);
        ourPieces->set(E8, false);
        ourPieces->set(A8, false);
        ourPieces->set(C8, true);
        ourPieces->set(D8, true);
    } else {
        std::cout<<"No castling rights!"<<std::endl;
    }
}

void ChessBoard::pushPromotionMove(Move move)
{
    int from = from_Square(move);
    int to = to_Square(move);

    BitBoard * ourPieces = getColorOnSquare(from);

    PieceType piece = movePromotionType(move);

    boardinfo.pawns.set(from, false);
    ourPieces->set(from, false);
    ourPieces->set(to, true);

    if(piece == QUEEN) boardinfo.queens.set(to, true);
    if(piece == ROOK) boardinfo.rooks.set(to, true);
    if(piece == BISHOP) boardinfo.bishops.set(to, true);
    if(piece == KNIGHT) boardinfo.knights.set(to, true);
}

void ChessBoard::pushEnPassantMove(Move move)
{
    int from = from_Square(move);
    int to = to_Square(move);

    BitBoard * ourPieceType = getPieceOnSquare(from);

    // White double pawn move (this needs to be seperated because we need to remove the pawn above or under the targetted square)
    if(from >= A5)
    {
        BitBoard * theirPiece = getPieceOnSquare(to - 8);
        boardinfo.whitePieces.set(from, false);
        boardinfo.whitePieces.set(to, true);
        ourPieceType->set(from, false);
        ourPieceType->set(to, true);
        theirPiece->set(to - 8, false);
        boardinfo.blackPieces.set(to - 8, false);
    } 
    else if(from <= H4)
    {
        BitBoard * theirPiece = getPieceOnSquare(to + 8);
        boardinfo.blackPieces.set(from, false);
        boardinfo.blackPieces.set(to, true);
        ourPieceType->set(from, false);
        ourPieceType->set(to, true);
        theirPiece->set(to + 8, false);
        boardinfo.whitePieces.set(to + 8, false);
    }
}

void ChessBoard::pushRegularMove(Move move)
{
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

    boardinfo.fiftyMoveRule++;

    if (boardinfo.pawns.board & ourPieceType->board)    
        boardinfo.fiftyMoveRule = 0;

    if(isCapture)
    {
        theirPieceType->set(to, false);

        BitBoard * theirPieces = getColorOnSquare(to);
        theirPieces->set(to, false);
        boardinfo.fiftyMoveRule = 0;
    }
    // changing to the position of a piece on one of the piece boards: pawn, knight, rook king etc.
    ourPieceType->set(from, false);
    ourPieceType->set(to, true);
    // changing the position of the piece on the white_pieces or black_pieces board.
    ourPieces->set(from, false);
    ourPieces->set(to, true);

    setEnPassantPossibility(*ourPieces, from, to);
}

void ChessBoard::pushMove(Move move)
{
    // setting previous board information the the current position when there has not been made a move.
    previousBoard = boardinfo;
    // getting the movetype
    MoveType type = moveType(move);

    if(type == CASTLING) pushCastlingMove(move);
    else if(type == PROMOTION) pushPromotionMove(move);
    else if(type == ENPASSANT) pushEnPassantMove(move);
    else if(type == NORMAL) pushRegularMove(move);

    // Update board information
    updateCastlingRights();
    boardinfo.whiteToMove = !boardinfo.whiteToMove;

    if (boardinfo.whiteToMove)
        boardinfo.plyCount++;
}

// removes one move from the list.
void ChessBoard::popMove()
{
    boardinfo = previousBoard;
}

Color ChessBoard::getOppositeColor(Color color)
{
    if(color == WHITE) return BLACK;
    return WHITE;
}

std::string ChessBoard::getPieceChar(int i)
{
    if(BitBoard(boardinfo.pawns.board & boardinfo.whitePieces.board).get(i)) return "P";
    if(BitBoard(boardinfo.knights.board & boardinfo.whitePieces.board).get(i)) return "N";
    if(BitBoard(boardinfo.bishops.board & boardinfo.whitePieces.board).get(i)) return "B";
    if(BitBoard(boardinfo.rooks.board & boardinfo.whitePieces.board).get(i)) return "R";
    if(BitBoard(boardinfo.queens.board & boardinfo.whitePieces.board).get(i)) return "Q";
    if(BitBoard(boardinfo.kings.board & boardinfo.whitePieces.board).get(i)) return "K";
    if(BitBoard(boardinfo.pawns.board & boardinfo.blackPieces.board).get(i)) return "p";
    if(BitBoard(boardinfo.knights.board & boardinfo.blackPieces.board).get(i)) return "n";
    if(BitBoard(boardinfo.bishops.board & boardinfo.blackPieces.board).get(i)) return "b";
    if(BitBoard(boardinfo.rooks.board & boardinfo.blackPieces.board).get(i)) return "r";
    if(BitBoard(boardinfo.queens.board & boardinfo.blackPieces.board).get(i)) return "q";
    if(BitBoard(boardinfo.kings.board & boardinfo.blackPieces.board).get(i)) return "k";
    return "0";
}

std::string ChessBoard::convertToFen()
{
    std::string fen = "";
    for (int y = 0; y <= 7; y++)
    {
        int spaces = 0;
        for (int x = 0; x <= 7; x++)
        {
            std::string c = getPieceChar((7-y)*8 + x);
            if (c != "0")
            {
                if (spaces > 0)
                {
                    fen += std::to_string(spaces);
                    spaces = 0;
                }
                fen += c;
            }
            else
            {
                spaces++;
            }
        }
        if (spaces > 0)
        {
            fen += std::to_string(spaces);
            spaces = 0;
        }
        fen += "/";
    }
    fen.pop_back();
    fen += " ";

    // Add side to move
    boardinfo.whiteToMove ? fen += "w " : fen += "b ";
    
    // Add castling rights
    if (!(boardinfo.whiteCastleLong || boardinfo.whiteCastleShort || boardinfo.blackCastleLong || boardinfo.blackCastleShort))
        fen += "- ";
    else
    {
        if (boardinfo.whiteCastleShort) fen += "K";
        if (boardinfo.whiteCastleLong) fen += "Q";
        if (boardinfo.blackCastleShort) fen += "k";
        if (boardinfo.blackCastleLong) fen += "q";
        fen += " ";
    }

    // Add en passant
    fen += "- ";


    // Add halfmove clock and fullmove number
    fen += std::to_string(boardinfo.fiftyMoveRule) + " ";
    fen += std::to_string(boardinfo.plyCount);

    return fen;
}

// creates Move classes from uci string representation that are relevant for board (promotions, castling, en passant)
Move ChessBoard::fromUci(std::string move)
{
    assert(move.size() <= 5);

    int from = getSquareInt(move.substr(0,2));
    int to = getSquareInt(move.substr(2,2));

    // promotion ("e7e8q"), these moves are different because they have a size of 5
    if(move.size() == 5)
    {
        char promotedTo = move.back();

        switch(promotedTo)
        {
            case 'q':
                return createMove(from, to, QUEEN);
                break;
            case 'n':
                return createMove(from, to, KNIGHT);
                break;
            case 'b':
                return createMove(from, to, BISHOP);
                break;
            case 'r':
                return createMove(from, to, ROOK);
                break;
        }
    }
    // castling moves: check if the move is on the castling squares and if the king is moved.
    if(from == E1 && to == G1 && (getPieceOnSquare(from)->board & boardinfo.kings.board)) return createMove(from, to, CASTLING);
    else if(from == E1 && to == C1 && (getPieceOnSquare(from)->board & boardinfo.kings.board)) return createMove(from, to, CASTLING);
    else if(from == E8 && to == G8 && (getPieceOnSquare(from)->board & boardinfo.kings.board)) return createMove(from, to, CASTLING);
    else if(from == E8 && to == C8 && (getPieceOnSquare(from)->board & boardinfo.kings.board)) return createMove(from, to, CASTLING);

    // en passant moves
    if(boardinfo.blackEnPassantTarget.get(to) && getBoard(BLACK, PAWN).get(from)) return createMove(from, to, ENPASSANT);
    else if(boardinfo.whiteEnPassantTarget.get(to) && getBoard(WHITE, PAWN).get(from)) return createMove(from, to, ENPASSANT);

    // normal move
    return createMove(from, to, NORMAL);
}