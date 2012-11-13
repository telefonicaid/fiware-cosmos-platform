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
#ifndef _H_BLOCK_REF
#define _H_BLOCK_REF

/* ****************************************************************************
 *
 * FILE                      BlockRef.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 */


#include <ostream>                   // std::ostream
#include <string>                    // std::string

#include "au/statistics/Cronometer.h"      // au::cronometer
#include "au/containers/SharedPointer.h"
#include "au/containers/list.h"      // au::list
#include "au/containers/map.h"       // au::map

#include "engine/Buffer.h"      // engine::Buffer
#include "engine/NotificationListener.h"           // engine::NotificationListener

#include "samson/common/KVFile.h"
#include "samson/common/coding.h"    // FullKVInfo
#include "samson/common/samson.pb.h"  // network::
#include "samson/stream/Block.h"
#include "samson/stream/BlockInfo.h"  // BlockInfo


namespace samson {
  class Info;
  
  namespace stream {
    class Block;
    class Queue;
    class StreamManager;
    class QueueItem;
    class BlockMatrix;
    
    
    /*
     *
     * BlockRef
     *
     *
     * Reference of a block and a particular range of key-values inside this block
     * Information about number of key-values and size is also included
     *
     */
    
    class BlockRef {
    public:
      
      BlockRef(BlockPointer block, KVRange range, KVInfo info);
      ~BlockRef();
      
      BlockPointer block();
      size_t block_id();
      size_t block_size();
      KVInfo info();
      KVRange range();
      au::SharedPointer<KVFile> file();
      
      // Accumulate content
      void append(BlockInfo&);
      
      // Review BlockReference
      void review(au::ErrorManager& error);
      
    private:
      
      BlockPointer block_;  // Pointer to the block
      KVRange range_;       // Range associated with this block
      KVInfo info_;         // Size and # of kvs to be processed
      
      // Extra information anout hg organitzation
      au::SharedPointer<KVFile> file_;
    };
  }
}

#endif
