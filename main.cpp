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

    const std::string testFen = "r4rk1/ppp1npbp/6p1/q3p3/N3P3/PnP4Q/1P3PPP/R1BR2K1 w - - 0 17";

    // BitBoard frick = Attacks::nonSlidingAttacks[WHITE][PAWN][31];
    // frick.printDebug();
    ChessBoard test(testFen);
    BitBoard lmaoxDDDD = test.getBlockers();
    lmaoxDDDD.printDebug();
    BitBoard testBruh(Attacks::getRookAttacks(42, lmaoxDDDD.board));
    testBruh.printDebug();

    return 0;
}
