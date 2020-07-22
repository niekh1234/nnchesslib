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

    ChessBoard test("rnbqkbnr/pppp1ppp/8/8/3Pp3/4P2P/PPP2PP1/RNBQKBNR b KQkq d3 0 3");

    test.whiteEnPassantTarget.printDebug();
    MoveList easy = genLegalMoves(test);

    test.print();

    return 0;
}
