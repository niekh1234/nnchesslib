#include <bitboard.h>
#include <iostream>
#include <bitset>
#include <cassert>

using namespace std;

// representation:
// a8: 1, b8: 2, 
// a7: 9, b7, 10
// etc

class BitBoard {
 private:
    std::bitset<64> board;

 public:
    string getBoardString(){
        return board.to_string();
    }

    void set(int x, int y){
        assert(0<=x && x<=7);
        assert(0<=y && y<=7);
        board.set(64 - 8*x-y);
    }

    int get(int x, int y){
        assert(0<=x && x<=7);
        assert(0<=y && y<=7);
        return board[64 - 8*x-y];
    }

    void setRow(int x){
        assert(0<=x && x<=7);
        for(int i = 0; i < 8; i++){
            board.set(i + x*8);
        }
    }

    void setAll(bool val){
        if(val)
            board.set();   //All bits on
        else
            board.reset(); //All bits off
        }
    };

int test(){
  BitBoard pawn;
  pawn.set(7,2);
  pawn.setRow(2);
  int testNum = pawn.get(7,2);

  std::string boardRepr = pawn.getBoardString();

  std::cout<<boardRepr<<"\n";
  for(int i = 0; i < boardRepr.size(); i++){
      if(i % 8 == 0){
          std::cout<<boardRepr.substr(i, 8)<<"\n";
      }
  }
  return 0;
}