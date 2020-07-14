#include <bitboard.h>

namespace nnchesslib
{
    namespace Rays
    {
        extern BitBoard rays[8][64];

        void initRays();

        BitBoard getRay(Direction d, int index);

        U64 northEast(U64 bRay, int n);
        U64 northWest(U64 bRay, int n);
    }
}