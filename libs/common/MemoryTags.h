#ifndef _H_MEMORY_TAGS
#define _H_MEMORY_TAGS

namespace ss
{

    enum MemoryTag
      {
        MemoryInput,                // Memory used for input in operations                                                     
        MemoryOutputNetwork,        // Memory to be writted on the network                                                     
        MemoryOutputDisk,           // Memory to be writted on disk                                                            
        MemoryAccumulated,          // Memory accumulated to be consolidated in a file                                         
          
        MemoryBlocks,               // Memory used by the blocks of the stream processing
      };

}

#endif
