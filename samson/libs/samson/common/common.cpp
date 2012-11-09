/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */


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