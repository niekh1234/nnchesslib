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

    const std::string testFen = "k7/8/8/3Q4/2K5/8/8/8 b - - 0 1";

    ChessBoard test("2k3r1/8/8/8/8/8/8/4K2R w K - 0 1");
    test.print();

    MoveList bruh = genLegalMoves(test);

    return 0;
}
