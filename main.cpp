#include <iostream>
#include <board.h>
#include <algorithm>
#include <rays.h>
#include <time.h>
#include <attacks.h>

using namespace nnchesslib;

void initAll()
{
    time_t begin, end;
    begin = clock();
    std::cout << "Initializing..." << std::endl;

    Rays::initRays();
    Attacks::initAllAttacks();

    end = clock();

    std::cout << "Finished initializing." << std::endl;
    std::cout << "Time taken: " << (float)(end-begin)/CLOCKS_PER_SEC << " seconds." << std::endl;
}

int main(int argc, char *argv[])
{
    initAll();

    const std::string testFen = "r1b1k1nr/1p1n1ppp/2pQp3/p3q3/2P5/B7/PP2NPPP/2KR1B1R b kq - 3 11";

    BitBoard frick = Attacks::nonSlidingAttacks[WHITE][PAWN][31];
    frick.printDebug();

    BitBoard bishopTest = BitBoard(Attacks::bishopMasks[35]);
    bishopTest.printDebug();

    BitBoard queenTest = BitBoard(Attacks::bishopMasks[35] | Attacks::rookMasks[35]);
    queenTest.printDebug();

    //ChessBoard board(STARTING_FEN);
    return 0;
}
