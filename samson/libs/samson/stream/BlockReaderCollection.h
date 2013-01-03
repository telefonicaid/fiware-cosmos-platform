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


#include "au/containers/SharedPointer.h"
#include "au/ErrorManager.h"

#include "samson/module/Operation.h"
#include "samson/stream/BlockList.h"
#include "samson/common/KVInputVector.h"


namespace samson {
  namespace stream {
    
    
    /*
     
     class BlockReaderCollection
     Auxiliar class used in reduce operations to organize input data
     
     class BlockReader
     Single item inside BlockReaderCollection
     */
    
    class BlockReader {
    public:
      
      // Create a block reader based on a block ref. Error can be returned if not possible to create
      static au::SharedPointer<BlockReader> create(BlockRef *block_ref, int channel, au::ErrorManager& error);
      
      au::SharedPointer<KVFile> kv_file() {
        return kv_file_;
      }
      
      int channel() {
        return channel_;
      }
      
    private:
      
      // Private constructor since instances are create with create method
      BlockReader() {}
      
      au::SharedPointer<KVFile> kv_file_;       // Internal management of data
      int channel_;                             // Input channel associated to this block
    };
    
    
    
    
    class BlockReaderCollection {
    public:
      
      BlockReaderCollection(Operation *operation) : kvVector_(operation) {
        operation_ = operation;        // Keep operation for sorting
      }
      
      ~BlockReaderCollection() {
        // Not necessary to clear vector since they are shared pointers
      }

      // Mehtods to add new blocks to this manager
      void AddInputBlocks(BlockRef *block_ref, int channel);
      void AddStateBlocks(BlockRef *block_ref, int channel);

      // Prepare processing a particular hash-group
      size_t PrepareProcessingHashGroup(int hg);
      
      // Get next elements to be processed by the operation
      KVSetStruct *GetNext();
      
    private:
      
      // Collection of BlockReader's ( each for every block used as input for this operation )
      au::Vector<BlockReader> input_block_readers_;
      
      // Collection of BlockReader's ( each for every block used in the state )
      // Note: Only one state at the moment
      au::Vector< BlockReader> state_block_readers_;
      
      // Common structure to give data to 3rd party software
      KVInputVector kvVector_;
      
      size_t input_num_kvs_;
      size_t state_num_kvs_;
      
      // Operation used ( necessary for sorting )
      Operation *operation_;
    };
    
  }
}
