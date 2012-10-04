#ifndef _H_BLOCK_LOOKUPLIST
#define _H_BLOCK_LOOKUPLIST

#include <set>

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/containers/SharedPointer.h"
#include "au/network/RESTServiceCommand.h"
#include "au/containers/simple_map.h"                         // au::simple_map

#include "engine/MemoryManager.h"
#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/NotificationListener.h"            // engien::EngineListener
#include "engine/ProcessItem.h"

#include "samson/common/FullKVInfo.h"
#include "samson/common/KVFile.h"
#include "samson/common/KVHeader.h"
#include "samson/common/KVInfo.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"

#include "samson/module/KVSetStruct.h"              // samson::KVSetStruct
#include "samson/Stream/BlockInfo.h"                              // struct BlockInfo


namespace samson {
  namespace stream {
    class BlockList;
    class Block;
    
    typedef struct BlockLookupRecord {
      char *keyP;
      size_t keyS;
    } BlockLookupRecord;
    
    typedef struct BlockHashLookupRecord {
      size_t startIx;
      size_t endIx;
    } BlockHashLookupRecord;
    
    /*
     
     BlockLookupList
     
     Lookup facility to locate "keys" in a block
     
     */
    
    class BlockLookupList {
      
    public:
      
      au::ErrorManager error;
      
      BlockLookupList(Block *_block);
      ~BlockLookupList();
      
      void lookup(const char *key, au::SharedPointer< au::network::RESTServiceCommand> command );
      
    private:
      
      BlockLookupRecord *head;
      size_t size;
      BlockHashLookupRecord *hashInfo;
      KVFormat kvFormat;
      size_t block_id_;
      
    };
  }
}

#endif  // ifndef _H_BLOCK_LOOKUPLIST