/*TODO:
 Create
    -pawn attacks
    -knight attacks
    -king attacks

    -Naive (slow) rook attacks (for magic tables)
    -Naive (slow) bishop attacks (for magic tables)

    -Init rook masks
    -Init bishop masks
 
 
 
 */

#ifndef ATTACKS_H
#define ATTACKS_H

#include <bitboard.h>

namespace nnchesslib
{
   namespace Attacks
   {
      extern U64 rookMasks[64];
      extern U64 bishopMasks[64];

      extern U64 nonSlidingAttacks[2][3][64];

      void initAllAttacks();

      void initRookMasks();
      void initBishopMasks();

      void initPawnAttacks();
      void initKnightAttacks();
      void initKingAttacks();
   }
}
#endif