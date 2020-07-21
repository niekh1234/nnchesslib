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

    ChessBoard test("rnb1kbn1/ppp1pp2/6p1/3P3r/5BP1/1PqP3p/P1PN1P1P/R2QKB1R b KQq - 0 10");
    test.print();
    MoveList testing;
    genPseudoLegalMoves(test, testing);
    if(test.squareAttacked(E5, WHITE)) std::cout<<"Yep that is an attack"<<std::endl;

    // for (Move m : testing)
    //     printMove(m);
    return 0;
}
