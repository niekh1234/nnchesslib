#include <bitboard.h>
#include <iostream>

using namespace nnchesslib;

int main()
{
    BitBoard pawn;
    pawn.set(0,0, true);
    pawn.setRank(2);
    pawn.setFile(2);
    int testNum = pawn.get(7,2);

    std::string boardRepr = pawn.getBoardString();

    std::cout<<boardRepr<<"\n";
    for(int i = 0; i < boardRepr.size(); i++){
        if(i % 8 == 0){
            std::cout<<boardRepr.substr(i, 8)<<"\n";
        }
    }
    return 0;
}
