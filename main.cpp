#include <iostream>
#include <board.h>

using namespace nnchesslib;

int main()
{
    const std::string testFen = "r1b1k1nr/1p1n1ppp/2pQp3/p3q3/2P5/B7/PP2NPPP/2KR1B1R b kq - 3 11";

    ChessBoard test(testFen);
    // BitBoard pawn;
    // pawn.setRank(1);
    // pawn.setRank(6);
    // pawn.set(1,1, false);

    // std::string boardRepr = pawn.getBoardString();

    // std::cout<<boardRepr<<"\n";
    // for(int i = 0; i < boardRepr.size(); i++){
    //     if(i % 8 == 0){
    //         std::cout<<boardRepr.substr(i, 8)<<"\n";
    //     }
    // }
    return 0;
}
