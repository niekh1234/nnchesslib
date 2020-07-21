#include <iostream>
#include <board.h>
#include <algorithm>
#include <rays.h>
#include <time.h>
#include <attacks.h>
#include <movegen.h>
#include <bitset>

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

    const std::string testFen = "1r2r1k1/pp6/3K4/4p1pp/4P3/5P2/6PP/1R6 b - - 1 34";

    ChessBoard test("r1b2rk1/pppp1pp1/2nb1q1p/1B2p3/4P3/2NP1N2/PPPQ1PPP/R3K2R b KQ - 1 8");
    MoveList testing;
    genPseudoLegalMoves(test, testing);

    for (Move m : testing)
        printMove(m);

    Move testMove = createMove(C2, C4);
    test.pushMove(testMove);
    test.print();
    return 0;
}
