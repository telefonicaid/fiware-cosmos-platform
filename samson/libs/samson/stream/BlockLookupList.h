#ifndef _H_BLOCK_LOOKUPLIST
#define _H_BLOCK_LOOKUPLIST

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "au/containers/simple_map.h"                         // au::simple_map

#include "engine/MemoryManager.h"

#include "samson/common/KVHeader.h"
#include "samson/common/KVFile.h"
#include "samson/common/KVInfo.h"
#include "samson/common/FullKVInfo.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"

#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/Object.h"                          // engien::EngineListener

#include "engine/ProcessItem.h"

#include "samson/module/KVSetStruct.h"              // samson::KVSetStruct

#include "BlockInfo.h"                              // struct BlockInfo

#include <set>

namespace samson { namespace stream {
  
  class BlockList;
  
  typedef struct BlockLookupRecord
  {
    char* keyP;
    size_t keyS;
  } BlockLookupRecord;
  
  typedef struct BlockHashLookupRecord
  {
    size_t  startIx;
    size_t  endIx;
  } BlockHashLookupRecord;
  
  class Block;
  class BlockLookupList
  {
    
    BlockLookupRecord*      head;
    size_t                  size;
    BlockHashLookupRecord*  hashInfo;
    KVFormat                kvFormat;
    
  public:
    
    au::ErrorManager error;
    
    BlockLookupList( Block* _block );
    ~BlockLookupList();
    
    std::string lookup(const char* key, std::string outputFormat);
    
  };
  
} }

#endif