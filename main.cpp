#include <iostream>
#include <board.h>
#include <algorithm>
#include <rays.h>
#include <time.h>

using namespace nnchesslib;

void initAll()
{
    time_t begin, end;
    begin = clock();
    std::cout << "Initializing..." << std::endl;

    Rays::initRays();

    end = clock();

    std::cout << "Finished initializing." << std::endl;
    std::cout << "Time taken: " << end-begin << " seconds." << std::endl;
}

int main(int argc, char *argv[])
{

    if (argc == 1 && argv[0] == "init")
        initAll();

    const std::string testFen = "r1b1k1nr/1p1n1ppp/2pQp3/p3q3/2P5/B7/PP2NPPP/2KR1B1R b kq - 3 11";

    Rays::initRays();
    BitBoard frick = Rays::getRay(NORTH_WEST, 32);
    frick.printDebug();

    //ChessBoard board(STARTING_FEN);
    return 0;
}
