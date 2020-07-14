#include <cassert>
#include <types.h>
#include <bitboard.h>
#include <rays.h>

using namespace nnchesslib;

BitBoard Rays::rays[8][64];

void Rays::initRays()
{
    for(int sq = 0; sq < 64; sq++)
    {
        rays[NORTH][sq] = 0x0101010101010100ULL << sq;

        rays[EAST][sq] = ((U64)1 << sq) - ((U64)1 << (sq & 56));

        rays[SOUTH][sq] = 0x0080808080808080ULL >> (63 - sq);

        rays[WEST][sq] =  2 * (((U64)1 << (sq | 7)) - ((U64)1 << sq));

        rays[NORTH_EAST][sq] = northEast(0x102040810204000ULL, 7 - (sq % 8)) << ((sq / 8) * 8);

        rays[NORTH_WEST][sq] = northWest(0x8040201008040200ULL, (sq % 8 ) << ((sq / 8) * 8));

        rays[SOUTH_EAST][sq] = 0;

        rays[SOUTH_WEST][sq] = 0;
    }
}

U64 Rays::northEast(U64 bRay, int n)
{
    U64 ray = bRay;
    for(int i = 0; i < n; i++)
    {
        bRay = ((ray << 1) & (~0x101010101010101ull));
    }
    return ray;
}

U64 Rays::northWest(U64 bRay, int n)
{
    U64 ray = bRay;
    for(int i = 0; i < n; i++)
    {
        bRay = ((ray >> 1) & (~0x8080808080808080ull));
    }
    return ray;
}


BitBoard Rays::getRay(Direction d, int index)
{
    assert(0 <= d && d <= 7);
    assert(0 <= index && index <= 63);
    return rays[d][index];
}