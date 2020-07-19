#include <attacks.h>
#include <rays.h>
#include <types.h>
#include <iostream>
#include <cassert>
#include <utils.h>

using namespace nnchesslib;

U64 Attacks::rookMasks[64];
U64 Attacks::bishopMasks[64];

U64 Attacks::rookTable[64][4096];
U64 Attacks::bishopTable[64][1024];

U64 Attacks::nonSlidingAttacks[2][6][64];

void Attacks::initAllAttacks()
{
    //Init masks (all attacks from a sliding piece)
    initRookMasks();
    initBishopMasks();

    initRookMagics();
    initBishopMagics();

    //Init non sliding piece attacks
    initPawnAttacks();
    initKnightAttacks();
    initKingAttacks();
}

U64 Attacks::genBlockers(int index, U64 mask)
{
    U64 blockerBoard = (U64)0;

    int bitIndex = 0;
    for(int i = 0; i <= 63; i++)
    {
        if(mask & (U64)1 << i)
        {
            if(index & (1 << bitIndex))
            {
                blockerBoard |= ((U64)1 << i);
            }

            bitIndex++;
        }
    }

    return blockerBoard;
}

void Attacks::initRookMasks()
{
    for (int i = 0; i <= 63; i++)
    {
        rookMasks[i] =  Rays::getRay(NORTH, i) & ~rank_bb[RANK_8] |
                        Rays::getRay(SOUTH, i) & ~rank_bb[RANK_1] |
                        Rays::getRay(WEST, i) & ~file_bb[FILE_A]  |
                        Rays::getRay(EAST, i) & ~file_bb[FILE_H];
    }
}

void Attacks::initBishopMasks()
{
    for (int i = 0; i <= 63; i++)
    {
        bishopMasks[i] =   (Rays::getRay(NORTH_EAST, i) | Rays::getRay(NORTH_WEST, i)  |
                            Rays::getRay(SOUTH_EAST, i) | Rays::getRay(SOUTH_WEST, i)) &
                            ~(file_bb[FILE_H] | file_bb[FILE_A] | rank_bb[RANK_1] | rank_bb[RANK_8]);
    }
}

void Attacks::initRookMagics()
{
    for(int sq = 0; sq <= 63; sq++)
    {
        for(int i = 0; i < (1 << countBits(Attacks::rookMasks[sq])); i++)
        {
            U64 blockers = Attacks::genBlockers(i, Attacks::rookMasks[sq]);

            Attacks::rookTable[sq][(blockers * Attacks::rookMagics[sq]) >> (64 - countBits(Attacks::rookMasks[sq]))] = Attacks::getRookAttacksRays(sq, blockers);
        }
    }
}

void Attacks::initBishopMagics()
{
    for(int sq = 0; sq <= 63; sq++)
    {
        for(int i = 0; i < (1 << countBits(Attacks::bishopMasks[sq])); i++)
        {
            U64 blockers = Attacks::genBlockers(i, Attacks::bishopMasks[sq]);

            Attacks::bishopTable[sq][(blockers * Attacks::bishopMagics[sq]) >> (64 - countBits(Attacks::bishopMasks[sq]))] = Attacks::getBishopAttacksRays(sq, blockers);
        }
    }
}

U64 Attacks::getRookAttacksRays(int sq, U64 blockers)
{
    U64 combinedAttacks = (U64)0;

    // north attacks
    U64 rookAttacksNorth = Rays::getRay(NORTH, sq);
    U64 northBlocker = rookAttacksNorth & blockers;
    if(blockers)
    {
        int index = bitScanForward(northBlocker);
        rookAttacksNorth ^= Rays::getRay(NORTH, index);
    }
    combinedAttacks |= rookAttacksNorth;

    // south attacks
    U64 rookAttacksSouth = Rays::getRay(SOUTH, sq);
    U64 southBlocker = rookAttacksSouth & blockers;
    if(blockers)
    {
        int index = bitScanReverse(southBlocker);
        rookAttacksSouth ^= Rays::getRay(SOUTH, index);
    }
    combinedAttacks |= rookAttacksSouth;

    // east attacks
    U64 rookAttacksEast = Rays::getRay(EAST, sq);
    U64 eastBlocker = rookAttacksEast & blockers;
    if(blockers)
    {
        int index = bitScanForward(eastBlocker);
        rookAttacksEast ^= Rays::getRay(EAST, index);
    }
    combinedAttacks |= rookAttacksEast;

    // west attacks
    U64 rookAttacksWest = Rays::getRay(WEST, sq);
    U64 westBlocker = rookAttacksWest & blockers;
    if(blockers)
    {
        int index = bitScanReverse(westBlocker);
        rookAttacksWest ^= Rays::getRay(WEST, index);
    }
    combinedAttacks |= rookAttacksWest;

    return combinedAttacks;
}

U64 Attacks::getBishopAttacksRays(int sq, U64 blockers)
{
    U64 combinedAttacks = (U64)0;

    // north west attacks
    U64 bishopAttacksNorthWest = Rays::getRay(NORTH_WEST, sq);
    U64 northWestBlocker = bishopAttacksNorthWest & blockers;
    if(blockers)
    {
        int index = bitScanForward(northWestBlocker);
        bishopAttacksNorthWest ^= Rays::getRay(NORTH_WEST, index);
    }
    combinedAttacks |= bishopAttacksNorthWest;

    // north east attacks
    // this is different because above method did not work properly.
    combinedAttacks |= Rays::getRay(NORTH_EAST, sq);
    if (Rays::getRay(NORTH_EAST, sq) & blockers) {
        combinedAttacks &= ~(Rays::getRay(NORTH_EAST, bitScanForward(Rays::getRay(NORTH_EAST, sq) & blockers)));
    }

    // south east attacks
    U64 bishopAttacksSouthEast = Rays::getRay(SOUTH_EAST, sq);
    U64 southEastBlocker = bishopAttacksSouthEast & blockers;
    if(blockers)
    {
        int index = bitScanReverse(southEastBlocker);
        bishopAttacksSouthEast ^= Rays::getRay(SOUTH_EAST, index);
    }
    combinedAttacks |= bishopAttacksSouthEast;

    // south west attacks
    U64 bishopAttacksSouthWest = Rays::getRay(SOUTH_WEST, sq);
    U64 southWestBlocker = bishopAttacksSouthWest & blockers;
    if(blockers)
    {
        int index = bitScanReverse(southWestBlocker);
        bishopAttacksSouthWest ^= Rays::getRay(SOUTH_WEST, index);
    }
    combinedAttacks |= bishopAttacksSouthWest;

    return combinedAttacks;
}

U64 Attacks::getRookAttacks(int sq, U64 blockers)
{
    //blockers will be the entire board representation, so getting blockers in rook mask:
    blockers &= Attacks::rookMasks[sq];
    // retrieving the index in which this specific blocker/square position is stored in the rookTable.
    U64 index = (blockers * Attacks::rookMagics[sq]) >> (64 - countBits(Attacks::rookMasks[sq]));
    return Attacks::rookTable[sq][index];
}

U64 Attacks::getBishopAttacks(int sq, U64 blockers)
{
    //blockers will be the entire board representation, so getting blockers in bishop mask:
    blockers &= Attacks::bishopMasks[sq];
    //retrieving the index in which this specific blocker/square position is stored in the bishopTable.
    U64 index = (blockers * Attacks::bishopMagics[sq]) >> (64 - countBits(Attacks::bishopMasks[sq]));
    return Attacks::bishopTable[sq][index];
}

void Attacks::initPawnAttacks()
{
    for (int i = 0; i <= 63; i++)
    {
        U64 pos = (U64)1 << i;

        U64 wpAttacks = ((pos << 9) & ~file_bb[FILE_A]) | ((pos << 7) & ~file_bb[FILE_H]);  //White pawn attacks, checking for no wraparound
        U64 bpAttacks = ((pos >> 7) & ~file_bb[FILE_A]) | ((pos >> 9) & ~file_bb[FILE_H]);  //Black pawn attacks, checking for no wraparound

        nonSlidingAttacks[WHITE][PAWN][i] = wpAttacks;
        nonSlidingAttacks[BLACK][PAWN][i] = bpAttacks;
    }
}

void Attacks::initKnightAttacks()
{
    for (int i = 0; i <= 63; i++)
    {
        U64 pos = (U64)1 << i;

        U64 kAttacks =  (((pos >> 6) | (pos << 10)) & ~(file_bb[FILE_B] | file_bb[FILE_A])) |   //Knight moves two to the left
                        (((pos << 6) | (pos >> 10)) & ~(file_bb[FILE_H] | file_bb[FILE_G])) |   //Knight moves two to the right
                        (((pos >> 15) | (pos << 17)) & ~(file_bb[FILE_A])) |                    //Knight moves one to the left
                        (((pos << 15) | (pos >> 17)) & ~(file_bb[FILE_H]));                     //Knight moves one to the right

        nonSlidingAttacks[WHITE][KNIGHT][i] = kAttacks;
        nonSlidingAttacks[BLACK][KNIGHT][i] = kAttacks;
    }
}

void Attacks::initKingAttacks()
{
    for (int i = 0; i <= 63; i++)
    {
        U64 pos = (U64)1 << i;

        U64 kAttacks =  ((pos << 7) | (pos >> 1) | (pos >> 9)) & ~file_bb[FILE_H] | //King moves to the right
                        ((pos >> 7) | (pos << 1) | (pos << 9)) & ~file_bb[FILE_A] | //King moves to the left
                        ((pos >> 8) | (pos << 8));                                  //King moves up and down | easy, no bounds checking :^)

        nonSlidingAttacks[WHITE][KING][i] = kAttacks;
        nonSlidingAttacks[BLACK][KING][i] = kAttacks;
    }
}

U64 Attacks::getNonSlidingAttacks(int sq, Color c, PieceType p)
{
    assert(p == 0 || p == 1 || p == 5);
    return nonSlidingAttacks[c][p][sq];
}

U64 Attacks::getSlidingAttacks(int sq, PieceType p, U64 blockers)
{
    assert(p == 2 || p == 3 || p == 4);
    switch(p)
    {
        case BISHOP:
            return getBishopAttacks(sq, blockers);
            break;
        case ROOK:
            return getRookAttacks(sq, blockers);
            break;
        case QUEEN:
            return (getRookAttacks(sq, blockers) | getBishopAttacks(sq, blockers));
            break;
        default:
            return (U64)0;
            break;
    }
}

