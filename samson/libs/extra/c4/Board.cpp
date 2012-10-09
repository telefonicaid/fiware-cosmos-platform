
#include "extra/c4/Board.h" // Own interface

namespace au {
namespace c4 {

Board::Board(size_t white, size_t black) :
  white_(white), black_(black) {
}

Board::Board() :
  white_(0), black_(0) {
}

}
}
