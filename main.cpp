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

    ChessBoard test("8/1PPPPP2/8/8/1k6/8/8/R3K3 w Q - 0 1");
    test.print();
    MoveList testing;
    genPseudoLegalMoves(test, testing);

    for (Move m : testing)
        printMove(m);
    return 0;
}
