

#include "common.h" // Own interface

#include "samson/module/samson.h"

namespace samson {

  std::string str_block_id(size_t block_id )
  {
    BlockId id;
    id.uint64 = block_id;
    return au::str("B_%d_%d" , id.uint32[0] , id.uint32[1] );
  }

}