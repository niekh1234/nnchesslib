#ifndef NNCHESSLIB_H
#define NNCHESSLIB_H

#include <string>
#include <cassert>
#include <vector>

#pragma once

namespace nnchesslib{
    void init();

    typedef unsigned long long U64;

    const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    enum Color
    {
        BLACK, WHITE
    };

    enum PieceType
    {
        PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
        TYPE_UD = 8
    };

    enum Piece
    {
        PIECE_NONE,
        W_PAWN = 1, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
        B_PAWN = 8, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
        PIECE_UD = 16
    };

    enum Square
    {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8 
    };

    constexpr U64 file_bb[8] = {  0x0101010101010101ULL, 0x0101010101010101ULL << 1,
                        0x0101010101010101ULL << 2, 0x0101010101010101ULL << 3,
                        0x0101010101010101ULL << 4, 0x0101010101010101ULL << 5,
                        0x0101010101010101ULL << 6, 0x0101010101010101ULL << 7};

    enum File
    {
        FILE_A, FILE_B, FILE_C, FILE_D, 
        FILE_E, FILE_F, FILE_G, FILE_H,
        FILE_UD
    };

    constexpr U64 rank_bb[8] = {  (U64)0xFF, (U64)0xFF << (8 * 1), 
                        (U64)0xFF << (8 * 2), (U64)0xFF << (8 * 3), 
                        (U64)0xFF << (8 * 4), (U64)0xFF << (8 * 5), 
                        (U64)0xFF << (8 * 6), (U64)0xFF << (8 * 7)};

    enum Rank
    {
        RANK_1, RANK_2, RANK_3, RANK_4, 
        RANK_5, RANK_6, RANK_7, RANK_8,
        RANK_UD
    };

    enum Direction
    {
        NORTH = 0,
        EAST,
        SOUTH,
        WEST,
        NORTH_EAST,
        NORTH_WEST,
        SOUTH_EAST,
        SOUTH_WEST
    };

    int countBits(U64 n);

    int bitScanForward(U64 board);

    int bitScanReverse(U64 board);

    int getSquareInt(std::string square);
    std::string getSquareString(int square);

    class BitBoard
    {
        public:
            //Attributes
            U64 board;
            //Constructors
            BitBoard();
            BitBoard(U64 value);

            //IO
            std::string getBoardString();

            //Interact with individual points
            void set(int square, bool set);
            int get(int square);

            //Interact with lines
            void setFile(int y);
            void setRank(int x);

            //Interact with entire board
            void setAll(bool val);
            void reset();

            // Manipulate the board
            U64 flipVertical();
            
            // Debug
            void printDebug();
    };
    namespace Attacks
    {
        extern U64 rookMasks[64];
        extern U64 bishopMasks[64];

        extern U64 rookTable[64][4096];
        extern U64 bishopTable[64][1024];

        extern U64 nonSlidingAttacks[2][6][64];

        const U64 rookMagics[64] = {
            0x0080001020400080, 0x0040001000200040, 0x0080081000200080, 0x0080040800100080,
            0x0080020400080080, 0x0080010200040080, 0x0080008001000200, 0x0080002040800100,
            0x0000800020400080, 0x0000400020005000, 0x0000801000200080, 0x0000800800100080,
            0x0000800400080080, 0x0000800200040080, 0x0000800100020080, 0x0000800040800100,
            0x0000208000400080, 0x0000404000201000, 0x0000808010002000, 0x0000808008001000,
            0x0000808004000800, 0x0000808002000400, 0x0000010100020004, 0x0000020000408104,
            0x0000208080004000, 0x0000200040005000, 0x0000100080200080, 0x0000080080100080,
            0x0000040080080080, 0x0000020080040080, 0x0000010080800200, 0x0000800080004100,
            0x0000204000800080, 0x0000200040401000, 0x0000100080802000, 0x0000080080801000,
            0x0000040080800800, 0x0000020080800400, 0x0000020001010004, 0x0000800040800100,
            0x0000204000808000, 0x0000200040008080, 0x0000100020008080, 0x0000080010008080,
            0x0000040008008080, 0x0000020004008080, 0x0000010002008080, 0x0000004081020004,
            0x0000204000800080, 0x0000200040008080, 0x0000100020008080, 0x0000080010008080,
            0x0000040008008080, 0x0000020004008080, 0x0000800100020080, 0x0000800041000080,
            0x00FFFCDDFCED714A, 0x007FFCDDFCED714A, 0x003FFFCDFFD88096, 0x0000040810002101,
            0x0001000204080011, 0x0001000204000801, 0x0001000082000401, 0x0001FFFAABFAD1A2
        };

        const U64 bishopMagics[64] = {
            0x0002020202020200, 0x0002020202020000, 0x0004010202000000, 0x0004040080000000,
            0x0001104000000000, 0x0000821040000000, 0x0000410410400000, 0x0000104104104000,
            0x0000040404040400, 0x0000020202020200, 0x0000040102020000, 0x0000040400800000,
            0x0000011040000000, 0x0000008210400000, 0x0000004104104000, 0x0000002082082000,
            0x0004000808080800, 0x0002000404040400, 0x0001000202020200, 0x0000800802004000,
            0x0000800400A00000, 0x0000200100884000, 0x0000400082082000, 0x0000200041041000,
            0x0002080010101000, 0x0001040008080800, 0x0000208004010400, 0x0000404004010200,
            0x0000840000802000, 0x0000404002011000, 0x0000808001041000, 0x0000404000820800,
            0x0001041000202000, 0x0000820800101000, 0x0000104400080800, 0x0000020080080080,
            0x0000404040040100, 0x0000808100020100, 0x0001010100020800, 0x0000808080010400,
            0x0000820820004000, 0x0000410410002000, 0x0000082088001000, 0x0000002011000800,
            0x0000080100400400, 0x0001010101000200, 0x0002020202000400, 0x0001010101000200,
            0x0000410410400000, 0x0000208208200000, 0x0000002084100000, 0x0000000020880000,
            0x0000001002020000, 0x0000040408020000, 0x0004040404040000, 0x0002020202020000,
            0x0000104104104000, 0x0000002082082000, 0x0000000020841000, 0x0000000000208800,
            0x0000000010020200, 0x0000000404080200, 0x0000040404040400, 0x0002020202020200   
        };

        void initAllAttacks();

        U64 genBlockers(int index, U64 mask);

        void initRookMasks();
        void initBishopMasks();

        void initRookMagics();
        void initBishopMagics();

        U64 getRookAttacksRays(int sq, U64 blockers);
        U64 getBishopAttacksRays(int sq, U64 blockers);

        U64 getRookAttacks(int sq, U64 blockers);
        U64 getBishopAttacks(int sq, U64 blockers);

        void initPawnAttacks();
        void initKnightAttacks();
        void initKingAttacks();

        U64 getNonSlidingAttacks(int sq, Color c, PieceType p);
        U64 getSlidingAttacks(int sq, PieceType p, U64 blockers);
    }

    enum MoveType
    {
        NORMAL, PROMOTION, ENPASSANT, CASTLING
    };

    typedef unsigned int Move;
    
    //0-5 -> to
    //6-11 -> from
    //12-13 -> promotionpiecetype (PieceType-1)
    //14-15 -> (0) no special (1) promotion (2) en passant (3) castle

    int from_Square(Move m);
    int to_Square(Move m);
    PieceType movePromotionType(Move m);
    MoveType moveType(Move m);

    Move createMove(int from, int to);
    Move createMove(int from, int to, PieceType p);
    Move createMove(int from, int to, MoveType mt);

    std::string toUci(Move m);

    void printMove(Move m);

    namespace Rays
    {
        extern U64 rays[8][64];

        void initRays();

        int file(int sq);
        int rank(int sq);

        U64 getRay(Direction d, int index);

        U64 northEast(U64 bRay, int n);
        U64 northWest(U64 bRay, int n);
    }

    struct BoardInfo
    {
        BitBoard whitePieces;
        BitBoard blackPieces;
        BitBoard pawns;
        BitBoard knights;
        BitBoard bishops;
        BitBoard rooks;
        BitBoard queens;
        BitBoard kings;

        bool whiteToMove = true;

        bool whiteCastleShort = false;
        bool whiteCastleLong = false;
        bool blackCastleShort = false;
        bool blackCastleLong = false;
        int fiftyMoveRule = 0;
        int plyCount = 0;
        int repetition;

        BitBoard whiteEnPassantTarget;
        BitBoard blackEnPassantTarget;
    };

    class ChessBoard
    {
        private:
            // Generate bitboards corresponding to a given fen.
            void generateBitBoards(std::string fen);
        public:
            BoardInfo boardinfo;
            BoardInfo previousBoard;

            ChessBoard();
            ChessBoard(std::string fenRepresentation);

            // Determine whether a fen is valid.
            bool isValidFen(std::string fen);
            // Get a string of the current board. 
            std::string getString();
            // cout's the current board in the terminal.
            void print();
            // Return the bitboard of a specified PieceType and color.
            BitBoard getBoard(Color color, PieceType piece);
            // Return the bitboard of a specified color.
            BitBoard getBoard(Color color);
            // Return all occupied squares in the board.
            BitBoard getBlockers();

            // Returns true if it is white to move and false if black is to move.
            bool getWhiteToMove();

            // Returns the piece bitboard by looking at which piece is on a specific index.
            BitBoard * getPieceOnSquare(int index);
            // Returns the color bitboard by looking at which piece is on a specific index.
            BitBoard * getColorOnSquare(int index);

            // Determines whether a black or white king is in check. Usage: kingInCheck(BLACK) returns true if black king in check.
            bool kingInCheck(Color color);
            // Determines whether a square is attacked by an opponent piece.
            bool squareAttacked(int square, Color color);

            void setEnPassantPossibility(BitBoard ourPieces, int from, int to);
            // Updates the boards castling rights.
            void updateCastlingRights();

            // Pushes a castling move to the board.
            void pushCastlingMove(Move move);
            // Pushes a promotion move to the board.
            void pushPromotionMove(Move move);
            // Pushes an en passant move to the board.
            void pushEnPassantMove(Move move);
            // Pushes a regular move to the board.
            void pushRegularMove(Move move);
            // Pushes a move to the board.
            void pushMove(Move move);
            // Undo's a move.
            void popMove();

            // Returns the opposite color: BLACK -> WHITE
            Color getOppositeColor(Color color);

            // Get char representation of a piece at an index.
            std::string getPieceChar(int i);
            // Board to fen conversion.
            std::string convertToFen();

            // Creating moves by parsing uci strings.
            Move fromUci(std::string uci);

            void pushFromUci(std::string uci);

            // Returns true if checkmate.
            bool isCheckMate();

            std::vector<std::string> genLegal();
    };

    typedef std::vector<Move> MoveList;
    // Function that generates pseudo-legal moves and looks at whether it put the king in check to generate legal moves.
    MoveList genLegalMoves(ChessBoard cboard);
    // function for calling pseudo-legal move generating functions.
    void genPseudoLegalMoves(ChessBoard cboard, MoveList& moveList);

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