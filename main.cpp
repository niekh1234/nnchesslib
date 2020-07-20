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

    ChessBoard test("R3b1kr/6pp/1bN5/2B5/1P2p3/8/P1P2PPP/6K1 b - - 4 23");
    MoveList testing;
    genPseudoLegalMoves(test, testing);

    for (Move m : testing)
        printMove(m);

    test.print();
    Move testMove = createMove(B6, C5);
    std::cout<<"b6c5"<<std::endl;
    test.pushMove(testMove);
    test.print();

    return 0;
}
