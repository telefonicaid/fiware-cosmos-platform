
#include <sstream>

#include "logMsg/logMsg.h"				// LM_M

#include "au/file.h"                                // au::sizeOfFile

#include "engine/ProcessManager.h"                  // engine::ProcessManager

#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/common/coding.h"                   // KVInfo
#include "samson/common/SamsonSetup.h"

#include "samson/module/ModulesManager.h"

#include "samson/network/NetworkInterface.h"
#include "samson/worker/SamsonWorker.h"

#include "WorkerTask.h"
#include "Block.h"          // samson::stream::Block
#include "BlockManager.h"   // samson::stream::BlockManager

#include "BlockList.h"                              // Own interface


namespace samson {
  namespace stream
  {
    
    // ------------------------------------------------------------------
    //
    // BlockRef
    //
    // ------------------------------------------------------------------
    
    BlockRef::BlockRef( BlockPointer block , KVRange range , KVInfo info )
    {
      block_ = block;
      range_ = range;
      info_ = info;
      
    }
    
    BlockRef::~BlockRef()
    {
    }
    
    BlockPointer BlockRef::block()
    {
      return block_;
    }
    
    size_t BlockRef::block_id()
    {
      return block_->get_block_id();
    }
    
    KVInfo BlockRef::info()
    {
      return info_;
    }
    
    KVRange BlockRef::range()
    {
      return range_;
    }
    
    void BlockRef::append( BlockInfo& block_info )
    {
      // Information about number of blocks
      block_info.num_blocks++;
      
      // Information about sizes
      size_t size = block_->getSize();
      block_info.size += size;
      if( block_->is_content_in_memory() )
        block_info.size_on_memory += size;
      if( block_->is_content_in_disk() )
        block_info.size_on_disk += size;
      if( block_->is_content_locked_in_memory() )
        block_info.size_locked += size;
      
      // Key-Value information
      block_info.info.append( info_ ); // Only the key-values considered in this refernce
      
      block_info.push( block_->getKVFormat() );
      
      block_info.pushTime( block_->getTime() );
      
    }
    
    au::SharedPointer<KVFile> BlockRef::file()
    {
      return file_;
    }
    
    void BlockRef::review( au::ErrorManager& error )
    {
      if( !block_->is_content_in_memory() )
      {
        error.set( au::str("Block %lu is not in memory" , block_->get_block_id() ));
        return;
      }
      
      if( block_->getKVFormat().isTxt() )
      {
        // Just full update info
        info_ = block_->getKVInfo();
        return;
      }
      
      // Get complete information about how key-values are organized in this block
      file_ = block_->getKVFile( error );
      
      if( error.IsActivated() )
        return;
      
      if( file_ == NULL )
      {
        error.set(au::str("Not possible to parse block %lu" , block_id() ));
        return;
      }
      
      // Update info ( absolutelly necessary for a correct commit at the end )
      info_.set(0, 0);
      for ( int i = range_.hg_begin ; i < range_.hg_end ; i++ )
        info_.append( file_->info[i] );
      
      LM_W(("Review block %lu %s %s"
            , block_->get_block_id()
            , range_.str().c_str()
            , info_.str().c_str() ));
      
    }
    
    // ------------------------------------------------------------------
    //
    // BlockList
    //
    // ------------------------------------------------------------------
    
    BlockList::~BlockList()
    {
      // Make sure I am not in any list in the blocks I am retaining...
      au::list< BlockRef >::iterator it_blocks;            // List of blocks references
      for (it_blocks = blocks.begin() ; it_blocks != blocks.end() ; it_blocks++ )
      {
        BlockRef * block_ref = *it_blocks;
        BlockPointer block = block_ref->block();
        
        block->remove_block_list(this);
      }
      
      
      clearBlockList();
    }
    
    void BlockList::clearBlockList()
    {
      // Remove all reference contained in this list
      blocks.clearList();
    }
    
    void BlockList::add( BlockRef *block_ref  )
    {
      // Insert this block in my list
      blocks.push_back( block_ref );
      
      // Update information in the block ( for sorting )
    }
    
    void BlockList::remove( BlockRef* block_ref )
    {
      // Remove this block from my list of blocks
      blocks.remove( block_ref );
      
      // Update information in the block ( for sorting )
    }
    
    void BlockList::lock_content_in_memory()
    {
      au::list< BlockRef >::iterator it_blocks;            // List of blocks references
      for (it_blocks = blocks.begin() ; it_blocks != blocks.end() ; it_blocks++ )
      {
        BlockRef * block_ref = *it_blocks;
        BlockPointer block = block_ref->block();
        if( !block->is_content_in_memory() )
          LM_X(1, ("Internal error"));
        block->lock_content_in_memory( this );
      }
    }
    
    
    size_t BlockList::getNumBlocks()
    {
      return blocks.size();
    }
    
    size_t BlockList::task_id()
    {
      return task_id_;
    }
    
    int BlockList::priority()
    {
      return priority_;
    }
    
    BlockInfo BlockList::getBlockInfo()
    {
      BlockInfo block_info;
      
      au::list< BlockRef >::iterator it;
      for ( it = blocks.begin() ; it != blocks.end() ; it++ )
      {
        BlockRef* block_ref = *it;
        block_ref->append( block_info );
      }
      
      return block_info;
    }
    
    void BlockList::ReviewBlockReferences( au::ErrorManager& error )
    {
      au::list< BlockRef >::iterator it;
      for (it = blocks.begin() ; it != blocks.end() ; it++ )
      {
        (*it)->review( error );
        if( error.IsActivated() )
          return;
      }
    }
    
  }       
}

