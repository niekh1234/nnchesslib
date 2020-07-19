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

    // BitBoard frick = Attacks::nonSlidingAttacks[WHITE][PAWN][31];
    // frick.printDebug();
    ChessBoard test("r3k2r/pppqp1bp/2n1pn2/3P2p1/7P/1PN1PN2/5PP1/R1BQ1K1R b kq - 0 12");
    MoveGen lmao = MoveGen(test);
    lmao.genLegalMoves();

    return 0;
}
