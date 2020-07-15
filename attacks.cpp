#include <bitboard.h>
#include <attacks.h>
#include <rays.h>
#include <types.h>

using namespace nnchesslib;

U64 Attacks::rookMasks[64];
U64 Attacks::bishopMasks[64];

U64 Attacks::nonSlidingAttacks[2][3][64];

void Attacks::initAllAttacks()
{
    //Init masks (all attacks from a sliding piece)
    initRookMasks();
    initBishopMasks();

    //Init non sliding piece attacks
    initPawnAttacks();
    initKnightAttacks();
    initKingAttacks();
}

void Attacks::initRookMasks()
{
    for (int i = 0; i <= 63; i++)
    {
        rookMasks[i] =  Rays::getRay(NORTH, i) & ~rank_bb[RANK_8] |
                        Rays::getRay(SOUTH, i) & ~rank_bb[RANK_1] |
                        Rays::getRay(EAST, i) & ~file_bb[FILE_H] |
                        Rays::getRay(WEST, i) & ~file_bb[FILE_A];
    }
}

void Attacks::initBishopMasks()
{
    for (int i = 0; i <= 63; i++)
    {
        bishopMasks[i] =    (Rays::getRay(NORTH_EAST, i) | Rays::getRay(NORTH_WEST, i) |
                            Rays::getRay(SOUTH_EAST, i) | Rays::getRay(SOUTH_WEST, i)) &
                            ~(file_bb[FILE_A] | file_bb[FILE_H] | rank_bb[RANK_1] | rank_bb[RANK_8]);
    }
}

void Attacks::initPawnAttacks()
{
    for (int i = 0; i <= 63; i++)
    {
        U64 pos = (U64)1 << i;

        U64 wpAttacks = ((pos << 9) & ~file_bb[FILE_H]) | ((pos << 7) & ~file_bb[FILE_A]);  //White pawn attacks, checking for no wraparound
        U64 bpAttacks = ((pos >> 7) & ~file_bb[FILE_H]) | ((pos >> 9) & ~file_bb[FILE_A]);  //Black pawn attacks, checking for no wraparound

        nonSlidingAttacks[WHITE][PAWN][i] = wpAttacks;
        nonSlidingAttacks[BLACK][PAWN][i] = bpAttacks;
    }
}

void Attacks::initKnightAttacks()
{
    for (int i = 0; i <= 63; i++)
    {
        U64 pos = (U64)1 << i;

        U64 kAttacks =  (((pos >> 6) | (pos << 10)) & ~(file_bb[FILE_G] | file_bb[FILE_H])) |   //Knight moves two to the left
                        (((pos << 6) | (pos >> 10)) & ~(file_bb[FILE_A] | file_bb[FILE_B])) |   //Knight moves two to the right
                        (((pos >> 15) | (pos << 17)) & ~(file_bb[FILE_H])) |                    //Knight moves one to the left
                        (((pos << 15) | (pos >> 17)) & ~(file_bb[FILE_A]));                     //Knight moves one to the right

        nonSlidingAttacks[WHITE][KNIGHT][i] = kAttacks;
        nonSlidingAttacks[BLACK][KNIGHT][i] = kAttacks;
    }
}

void Attacks::initKingAttacks()
{
    for (int i = 0; i <= 63; i++)
    {
        U64 pos = (U64)1 << i;

        U64 kAttacks =  ((pos << 7) | (pos >> 1) | (pos >> 9)) & ~file_bb[FILE_A] | //King moves to the right
                        ((pos >> 7) | (pos << 1) | (pos << 9)) & ~file_bb[FILE_H] | //King moves to the left
                        ((pos >> 8) | (pos << 8));                                  //King moves up and down | easy, no bounds checking :^)

        nonSlidingAttacks[WHITE][KING][i] = kAttacks;
        nonSlidingAttacks[BLACK][KING][i] = kAttacks;
    }
}

