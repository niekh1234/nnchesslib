#include <string>
#include <bitset>

#pragma once

namespace nnchesslib
{
    class BitBoard
    {
        public:

            //Attributes
            std::bitset<64> board;

            //Constructors
            BitBoard();

            //IO
            std::string getBoardString();

            //Interact with individual points
            void set(int x, int y, bool set);
            int get(int x, int y);

            //Interact with lines
            void setFile(int y);
            void setRank(int x);

            //Interact with entire board
            void setAll(bool val);
            void reset();
    };
}