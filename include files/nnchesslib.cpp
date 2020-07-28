#include <nnchesslib.h>
#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include <bitset>
#include <sstream>
#include <algorithm>

using namespace nnchesslib;

void nnchesslib::init()
{
    time_t begin, end;
    begin = clock();
    std::cout << "Initializing..." << std::endl;

    Rays::initRays();
    Attacks::initAllAttacks();

    end = clock();
}
// counts 1's in a U64
int nnchesslib::countBits(U64 n)
{
    int output = 0;
    while (n != 0)
    {
        n &= (n - 1);
        output++;
    }
    return output;
}

extern int nnchesslib::bitScanForward(U64 board)
{
    return __builtin_ffsll(board) - 1;
}

extern int nnchesslib::bitScanReverse(U64 board)
{
    return 63 - __builtin_clzll(board);
}

// converts e.g. 'e2' to 12
int nnchesslib::getSquareInt(std::string square)
{
    int file = square.at(0) - 97;
    int rank = square.at(1) - '0';
    return file + (rank - 1) * 8;
}

std::string nnchesslib::getSquareString(int square)
{
    assert(square >= 0 && square <= 63);
    int file = square % 8;
    int rank = square / 8 + 1;

    const char* files = "abcdefgh";

    return files[file] + std::to_string(rank);
}

BitBoard::BitBoard()
{
    board = 0;
}

BitBoard::BitBoard(U64 value)
{
    board = value;
}

std::string BitBoard::getBoardString()
{
    std::string binary = std::bitset<64>(board).to_string();
    return binary;
}

void BitBoard::set(int square, bool set)
{
    assert(0 <= square && square <= 63);
    if(set)
        board |= ((U64)1 << square);
    else
        board &= ~((U64)1 << square);
}

int BitBoard::get(int square)
{
    assert(0 <= square && square <= 63);
    
    int val = (board >> square) & 1;

    return val;
}

void BitBoard::setRank(int y)
{
    assert(0 <= y && y <= 7);
    
    board |= rank_bb[y]; 
}

void BitBoard::setFile(int x)
{
    assert(0 <= x && x <= 7);

    board |= file_bb[x];
}

void BitBoard::setAll(bool val)
{

}

void BitBoard::reset()
{

}

U64 BitBoard::flipVertical()
{
    return __bswap_64(BitBoard::board);
}

void BitBoard::printDebug()
{
    std::string boardString = getBoardString();
    std::cout<<"---------------"<<std::endl;
    for (int y = 0; y <= 7; y++)
    {
        for (int x = 0; x <= 7; x++)
        {
            // if (boardString[63 - (8*y+x)] == '1')
            if (boardString[8*y+7-x] == '1')
                std::cout << "1 ";
            else
                std::cout << ". ";
        }
        std::cout << std::endl;
    }
    std::cout<<"---------------"<<std::endl;
}

// All Rays stored in memory.
U64 Rays::rays[8][64];

// Converts a square into a file.
int Rays::file(int sq)
{
    return(sq % 8);
}

// Converts a square into a rank.
int Rays::rank(int sq)
{
    return(sq / 8);
}

// Function for calculating all Rays to be stored in memory.
void Rays::initRays()
{
    for(int sq = 0; sq < 64; sq++)
    {
        rays[NORTH][sq] = 0x0101010101010100ULL << sq;

        rays[EAST][sq] = 2 * (((U64)1 << (sq | 7)) - ((U64)1 << sq));

        rays[SOUTH][sq] = 0x0080808080808080ULL >> (63 - sq);

        rays[WEST][sq] = ((U64)1 << sq) - ((U64)1 << (sq & 56)) ;

        rays[NORTH_WEST][sq] = northWest(0x102040810204000ULL, 7 - file(sq)) << (rank(sq) * 8);

        rays[NORTH_EAST][sq] = northEast(0x8040201008040200ULL, file(sq)) << (rank(sq) * 8);

        rays[SOUTH_WEST][sq] = northWest(0x40201008040201ULL, 7 - file(sq)) >> ((7 - rank(sq)) * 8);

        rays[SOUTH_EAST][sq] = northEast(0x2040810204080ULL, file(sq)) >> ((7 - rank(sq)) * 8);
    }
}

// Shifts a diagonal north-east ray without having bits wrap around.
inline U64 Rays::northEast(U64 bRay, int n)
{
    U64 ray = bRay;
    for(int i = 0; i < n; i++)
    {
        ray = (ray << ((U64)1) & ~0x0101010101010101ULL); 
    }
    return ray;
}

// Shifts a diagonal north-west ray without having bits wrap around.
inline U64 Rays::northWest(U64 bRay, int n)
{
    U64 ray = bRay;
    for(int i = 0; i < n; i++)
    {
        ray = ((ray >> (U64)1) & ~0x8080808080808080ull);
    }
    return ray;
}

// Returns Rays from memory from a given direction and index.
U64 Rays::getRay(Direction d, int index)
{
    assert(0 <= d && d <= 7);
    assert(-1 <= index && index <= 63);
    
    return rays[d][index];
}

U64 Attacks::rookMasks[64];
U64 Attacks::bishopMasks[64];

U64 Attacks::rookTable[64][4096];
U64 Attacks::bishopTable[64][1024];

U64 Attacks::nonSlidingAttacks[2][6][64];

void Attacks::initAllAttacks()
{
    //Init masks (all attacks from a sliding piece)
    initRookMasks();
    initBishopMasks();

    initRookMagics();
    initBishopMagics();

    //Init non sliding piece attacks
    initPawnAttacks();
    initKnightAttacks();
    initKingAttacks();
}

U64 Attacks::genBlockers(int index, U64 mask)
{
    U64 blockerBoard = (U64)0;

    int bitIndex = 0;
    for(int i = 0; i <= 63; i++)
    {
        if(mask & (U64)1 << i)
        {
            if(index & (1 << bitIndex))
            {
                blockerBoard |= ((U64)1 << i);
            }

            bitIndex++;
        }
    }

    return blockerBoard;
}

void Attacks::initRookMasks()
{
    for (int i = 0; i <= 63; i++)
    {
        rookMasks[i] =  Rays::getRay(NORTH, i) & ~rank_bb[RANK_8] |
                        Rays::getRay(SOUTH, i) & ~rank_bb[RANK_1] |
                        Rays::getRay(WEST, i) & ~file_bb[FILE_A]  |
                        Rays::getRay(EAST, i) & ~file_bb[FILE_H];
    }
}

void Attacks::initBishopMasks()
{
    for (int i = 0; i <= 63; i++)
    {
        bishopMasks[i] =   (Rays::getRay(NORTH_EAST, i) | Rays::getRay(NORTH_WEST, i)  |
                            Rays::getRay(SOUTH_EAST, i) | Rays::getRay(SOUTH_WEST, i)) &
                            ~(file_bb[FILE_H] | file_bb[FILE_A] | rank_bb[RANK_1] | rank_bb[RANK_8]);
    }
}

void Attacks::initRookMagics()
{
    for(int sq = 0; sq <= 63; sq++)
    {
        for(int i = 0; i < (1 << countBits(Attacks::rookMasks[sq])); i++)
        {
            U64 blockers = Attacks::genBlockers(i, Attacks::rookMasks[sq]);

            Attacks::rookTable[sq][(blockers * Attacks::rookMagics[sq]) >> (64 - countBits(Attacks::rookMasks[sq]))] = Attacks::getRookAttacksRays(sq, blockers);
        }
    }
}

void Attacks::initBishopMagics()
{
    for(int sq = 0; sq <= 63; sq++)
    {
        for(int i = 0; i < (1 << countBits(Attacks::bishopMasks[sq])); i++)
        {
            U64 blockers = Attacks::genBlockers(i, Attacks::bishopMasks[sq]);

            Attacks::bishopTable[sq][(blockers * Attacks::bishopMagics[sq]) >> (64 - countBits(Attacks::bishopMasks[sq]))] = Attacks::getBishopAttacksRays(sq, blockers);
        }
    }
}

U64 Attacks::getRookAttacksRays(int sq, U64 blockers)
{
    U64 combinedAttacks = (U64)0;

    // north attacks
    U64 rookAttacksNorth = Rays::getRay(NORTH, sq);
    U64 northBlocker = rookAttacksNorth & blockers;
    if(blockers)
    {
        int index = bitScanForward(northBlocker);
        rookAttacksNorth ^= Rays::getRay(NORTH, index);
    }
    combinedAttacks |= rookAttacksNorth;

    // south attacks
    U64 rookAttacksSouth = Rays::getRay(SOUTH, sq);
    U64 southBlocker = rookAttacksSouth & blockers;
    if(blockers)
    {
        int index = bitScanReverse(southBlocker);
        rookAttacksSouth ^= Rays::getRay(SOUTH, index);
    }
    combinedAttacks |= rookAttacksSouth;

    // east attacks
    U64 rookAttacksEast = Rays::getRay(EAST, sq);
    U64 eastBlocker = rookAttacksEast & blockers;
    if(blockers)
    {
        int index = bitScanForward(eastBlocker);
        rookAttacksEast ^= Rays::getRay(EAST, index);
    }
    combinedAttacks |= rookAttacksEast;

    // west attacks
    U64 rookAttacksWest = Rays::getRay(WEST, sq);
    U64 westBlocker = rookAttacksWest & blockers;
    if(blockers)
    {
        int index = bitScanReverse(westBlocker);
        rookAttacksWest ^= Rays::getRay(WEST, index);
    }
    combinedAttacks |= rookAttacksWest;

    return combinedAttacks;
}

U64 Attacks::getBishopAttacksRays(int sq, U64 blockers)
{
    U64 combinedAttacks = (U64)0;

    // north west attacks
    U64 bishopAttacksNorthWest = Rays::getRay(NORTH_WEST, sq);
    U64 northWestBlocker = bishopAttacksNorthWest & blockers;
    if(blockers)
    {
        int index = bitScanForward(northWestBlocker);
        bishopAttacksNorthWest ^= Rays::getRay(NORTH_WEST, index);
    }
    combinedAttacks |= bishopAttacksNorthWest;

    // north east attacks
    // this is different because above method did not work properly.
    combinedAttacks |= Rays::getRay(NORTH_EAST, sq);
    if (Rays::getRay(NORTH_EAST, sq) & blockers) {
        combinedAttacks &= ~(Rays::getRay(NORTH_EAST, bitScanForward(Rays::getRay(NORTH_EAST, sq) & blockers)));
    }

    // south east attacks
    U64 bishopAttacksSouthEast = Rays::getRay(SOUTH_EAST, sq);
    U64 southEastBlocker = bishopAttacksSouthEast & blockers;
    if(blockers)
    {
        int index = bitScanReverse(southEastBlocker);
        bishopAttacksSouthEast ^= Rays::getRay(SOUTH_EAST, index);
    }
    combinedAttacks |= bishopAttacksSouthEast;

    // south west attacks
    U64 bishopAttacksSouthWest = Rays::getRay(SOUTH_WEST, sq);
    U64 southWestBlocker = bishopAttacksSouthWest & blockers;
    if(blockers)
    {
        int index = bitScanReverse(southWestBlocker);
        bishopAttacksSouthWest ^= Rays::getRay(SOUTH_WEST, index);
    }
    combinedAttacks |= bishopAttacksSouthWest;

    return combinedAttacks;
}

U64 Attacks::getRookAttacks(int sq, U64 blockers)
{
    //blockers will be the entire board representation, so getting blockers in rook mask:
    blockers &= Attacks::rookMasks[sq];
    // retrieving the index in which this specific blocker/square position is stored in the rookTable.
    U64 index = (blockers * Attacks::rookMagics[sq]) >> (64 - countBits(Attacks::rookMasks[sq]));
    return Attacks::rookTable[sq][index];
}

U64 Attacks::getBishopAttacks(int sq, U64 blockers)
{
    //blockers will be the entire board representation, so getting blockers in bishop mask:
    blockers &= Attacks::bishopMasks[sq];
    //retrieving the index in which this specific blocker/square position is stored in the bishopTable.
    U64 index = (blockers * Attacks::bishopMagics[sq]) >> (64 - countBits(Attacks::bishopMasks[sq]));
    return Attacks::bishopTable[sq][index];
}

void Attacks::initPawnAttacks()
{
    for (int i = 0; i <= 63; i++)
    {
        U64 pos = (U64)1 << i;

        U64 wpAttacks = ((pos << 9) & ~file_bb[FILE_A]) | ((pos << 7) & ~file_bb[FILE_H]);  //White pawn attacks, checking for no wraparound
        U64 bpAttacks = ((pos >> 7) & ~file_bb[FILE_A]) | ((pos >> 9) & ~file_bb[FILE_H]);  //Black pawn attacks, checking for no wraparound

        nonSlidingAttacks[WHITE][PAWN][i] = wpAttacks;
        nonSlidingAttacks[BLACK][PAWN][i] = bpAttacks;
    }
}

void Attacks::initKnightAttacks()
{
    for (int i = 0; i <= 63; i++)
    {
        U64 pos = (U64)1 << i;

        U64 kAttacks =  (((pos >> 6) | (pos << 10)) & ~(file_bb[FILE_B] | file_bb[FILE_A])) |   //Knight moves two to the left
                        (((pos << 6) | (pos >> 10)) & ~(file_bb[FILE_H] | file_bb[FILE_G])) |   //Knight moves two to the right
                        (((pos >> 15) | (pos << 17)) & ~(file_bb[FILE_A])) |                    //Knight moves one to the left
                        (((pos << 15) | (pos >> 17)) & ~(file_bb[FILE_H]));                     //Knight moves one to the right

        nonSlidingAttacks[WHITE][KNIGHT][i] = kAttacks;
        nonSlidingAttacks[BLACK][KNIGHT][i] = kAttacks;
    }
}

void Attacks::initKingAttacks()
{
    for (int i = 0; i <= 63; i++)
    {
        U64 pos = (U64)1 << i;

        U64 kAttacks =  ((pos << 7) | (pos >> 1) | (pos >> 9)) & ~file_bb[FILE_H] | //King moves to the right
                        ((pos >> 7) | (pos << 1) | (pos << 9)) & ~file_bb[FILE_A] | //King moves to the left
                        ((pos >> 8) | (pos << 8));                                  //King moves up and down | easy, no bounds checking :^)

        nonSlidingAttacks[WHITE][KING][i] = kAttacks;
        nonSlidingAttacks[BLACK][KING][i] = kAttacks;
    }
}

U64 Attacks::getNonSlidingAttacks(int sq, Color c, PieceType p)
{
    assert(p == 0 || p == 1 || p == 5);
    return nonSlidingAttacks[c][p][sq];
}

U64 Attacks::getSlidingAttacks(int sq, PieceType p, U64 blockers)
{
    assert(p == 2 || p == 3 || p == 4);
    switch(p)
    {
        case BISHOP:
            return getBishopAttacks(sq, blockers);
            break;
        case ROOK:
            return getRookAttacks(sq, blockers);
            break;
        case QUEEN:
            return (getRookAttacks(sq, blockers) | getBishopAttacks(sq, blockers));
            break;
        default:
            return (U64)0;
            break;
    }
}
int nnchesslib::from_Square(Move m)
{
    return (m >> 6) & 0x3f;
}

int nnchesslib::to_Square(Move m)
{
    return (m & 0x3f);
}

PieceType nnchesslib::movePromotionType(Move m)
{
    return PieceType(((m >> 12) & 3) + KNIGHT);
}

MoveType nnchesslib::moveType(Move m)
{
    return MoveType(((m >> 14) & 3) + (int)NORMAL);
}

Move nnchesslib::createMove(int from, int to)
{
    return (from << 6) + to;
}

Move nnchesslib::createMove(int from, int to, PieceType p)
{
    return (PROMOTION << 14) + ((p-KNIGHT) << 12) + (from << 6) + to;
}

Move nnchesslib::createMove(int from, int to, MoveType mt)
{
    return (mt << 14) + (from << 6) + to;
}

std::string nnchesslib::toUci(Move m)
{
    std::string from = getSquareString(from_Square(m));
    std::string to = getSquareString(to_Square(m));

    std::string output = from + to;

    if(moveType(m) == PROMOTION)
    {
        switch(movePromotionType(m))
        {
            case QUEEN:
                output+="q";
                break;
            case KNIGHT:
                output+="n";
                break;
            case BISHOP:
                output+="b";
                break;
            case ROOK:
                output+="r";
                break;
        }
    }
    return output;
}

void nnchesslib::printMove(Move m)
{
    std::cout << "From " << from_Square(m) << " to " << to_Square(m) << std::endl;
}

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
    std::string board, sideToMove, castleRights, enpassant, plycount, movecount;

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

    iss >> movecount;

    iss >> plycount;

    boardinfo.plyCount = stoi(plycount);
    boardinfo.fiftyMoveRule = stoi(movecount);

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
std::string ChessBoard::getString()
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
    return finalOutput;
}

void ChessBoard::print()
{
    std::cout<<getString()<<std::endl;
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

void ChessBoard::pushFromUci(std::string uci)
{
    Move move = fromUci(uci);

    pushMove(move);
}

bool ChessBoard::isCheckMate()
{
    MoveList legalMoves = genLegalMoves(*this);

    if(legalMoves.size() == 0) return true;
    else return false;
}

std::vector<std::string> ChessBoard::genLegal()
{
    MoveList legalMoves = genLegalMoves(*this);
    std::vector<std::string> uciList;

    for(auto move : legalMoves)
    {
        uciList.push_back(toUci(move));
    }

    return uciList;
}

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
