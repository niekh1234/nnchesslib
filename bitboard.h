#ifndef BITBOARD_H
#define BITBOARD_H

#include <string>
#include <types.h>

namespace nnchesslib
{
    class BitBoard
    {
        public:

            //Attributes
            U64 board;
            //Constructors
            BitBoard();
            BitBoard(U64 value);

            // BitBoard();

            //IO
            std::string getBoardString();

            //Interact with individual points
            void set(int square, bool set);
            int get(int x, int y);

            //Interact with lines
            void setFile(int y);
            void setRank(int x);

            //Interact with entire board
            void setAll(bool val);
            void reset();

            // Debug
            void printDebug();
    };
}

#endif