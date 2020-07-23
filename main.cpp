#include <iostream>
#include <board.h>
#include <algorithm>
#include <rays.h>
#include <time.h>
#include <attacks.h>
#include <movegen.h>
#include <bitset>
#include <utils.h>
#include <string>

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

    ChessBoard test("1k6/8/8/8/8/8/8/R3K3 w - - 0 1");

    Move bruh = test.fromUci("e7e8q");
    std::string ucitest = toUci(bruh);
    std::cout<<ucitest<<std::endl;

    return 0;
}
