#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <board.h>
#include <move.h>
#include <vector>

/* ideas
- movelist 
- gen pseudo legal moves for individual color and piece type
    - gen legal moves based on pseudo legal

**/

namespace nnchesslib
{
    typedef std::vector<Move> MoveList;
}

#endif
