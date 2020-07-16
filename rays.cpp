#include <cassert>
#include <types.h>
#include <bitboard.h>
#include <rays.h>
#include <iostream>

using namespace nnchesslib;

U64 Rays::rays[8][64];

int Rays::file(int sq)
{
    return(sq % 8);
}

int Rays::rank(int sq)
{
    return(sq / 8);
}

void Rays::initRays()
{
    for(int sq = 0; sq < 64; sq++)
    {
        rays[NORTH][sq] = 0x0101010101010100ULL << sq;

        rays[EAST][sq] = 2 * (((U64)1 << (sq | 7)) - ((U64)1 << sq));

        rays[SOUTH][sq] = 0x0080808080808080ULL >> (63 - sq);

        rays[WEST][sq] = ((U64)1 << sq) - ((U64)1 << (sq & 56)) ;

        rays[NORTH_WEST][sq] = northWest(0x102040810204000ULL, 7 - file(sq)) << (rank(sq) * 8);

        rays[NORTH_EAST][sq] = northEast(0x8040201008040200ULL, file(sq)) << (rank(sq) * 8);

        rays[SOUTH_WEST][sq] = northWest(0x40201008040201ULL, 7 - file(sq)) >> ((7 - rank(sq)) * 8);

        rays[SOUTH_EAST][sq] = northEast(0x2040810204080ULL, file(sq)) >> ((7 - rank(sq)) * 8);
    }
}

inline U64 Rays::northEast(U64 bRay, int n)
{
    U64 ray = bRay;
    for(int i = 0; i < n; i++)
    {
        ray = (ray << ((U64)1) & ~0x0101010101010101ULL); 
    }
    return ray;
}

inline U64 Rays::northWest(U64 bRay, int n)
{
    U64 ray = bRay;
    for(int i = 0; i < n; i++)
    {
        ray = ((ray >> (U64)1) & ~0x8080808080808080ull);
    }
    return ray;
}


U64 Rays::getRay(Direction d, int index)
{
    assert(0 <= d && d <= 7);
    // assert(0 <= index && index <= 63);
    return rays[d][index];
}