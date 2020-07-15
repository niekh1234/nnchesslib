#include <bitboard.h>

namespace nnchesslib
{
    namespace Rays
    {
        extern U64 rays[8][64];

        void initRays();

        int file(int sq);
        int rank(int sq);

        U64 getRay(Direction d, int index);

        U64 northEast(U64 bRay, int n);
        U64 northWest(U64 bRay, int n);
    }
}