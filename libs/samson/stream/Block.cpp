
#include "logMsg/logMsg.h"                     // LM_W


#include "engine/DiskOperation.h"       // engine::DiskOperation
#include "engine/DiskManager.h"        // notification_disk_operation_request_response
#include "engine/Engine.h"              // engine::Engine
#include "engine/Notification.h"       // engine::Notification
#include "engine/MemoryManager.h"       // engine::MemoryManager

#include "samson/common/MemoryTags.h"                 // MemoryBlocks
#include "samson/common/SamsonSetup.h"                // samson::SamsonSetup

#include "PopQueueTask.h"               // samson::stream::PopQueueTask
#include "BlockManager.h"               // BlockManager
#include "BlockList.h"                  // BlockList

#include "Block.h"                      // Own interface

namespace samson {
    namespace stream
    {

        const char* Block::getState()
        {
            switch (state) {
                case Block::on_memory:
                    return " M ";
                    break;
                case Block::on_disk:
                    return "  D";
                    break;
                case Block::ready:
                    return " MD";
                    break;
                case Block::writing:
                    return " MW";
                    break;
                case Block::reading:
                    return " RD";
                    break;
            }
            
            return "Unknown";
        }
        
        
        Block::Block( engine::Buffer *_buffer )
        {
            // Get a new unique id from the block manager
            id = BlockManager::shared()->getNextBlockId();
            
            // Buffer of data
            buffer = _buffer;  
            buffer->tag = MemoryBlocks;     // Set the tag to MemoryBlock to controll the memory used by this system
            
            // Get the size of the packet
            size = buffer->getSize();

            // Default state is on_memory because the buffer has been given at memory
            state = on_memory;

            // Get a copy of the header
            header = (KVHeader*) malloc( sizeof( KVHeader ) );
            memcpy(header, buffer->getData(), sizeof(KVHeader));

            // Check range is coherent with the info vector
            if( !header->range.check( getKVInfo() ) )
                LM_X(1,("Internal error: incoherence between range in header"));

            
        }
        
        Block::Block( size_t _id , size_t _size , KVHeader* _header )
        {
            // Get a new unique id from the block manager
            id = _id;
            
            // Buffer of data
            buffer = NULL;  
            
            // Get the size of the packet
            size = _size;
            
            // Default state is on_memory because the buffer has been given at memory
            state = on_disk;
            
            // Get a copy of the header
            header = (KVHeader*) malloc( sizeof( KVHeader ) );
            memcpy(header, _header , sizeof(KVHeader));
            
        }
        

        Block::~Block()
        {
            
            // Destroy buffer if still in memory
            if( buffer )
                engine::MemoryManager::shared()->destroyBuffer( buffer );
        }
        
        
        bool Block::isLockedInMemory()
        {
            
            std::set< BlockList* >::iterator l;
            for ( l = lists.begin() ; l != lists.end() ; l++ )
                if( (*l)->lock_in_memory )
                    return true;
            
            return false;
        }

        size_t Block::getMinTaskId()
        {
            size_t _task_id = (size_t) -1;
            
            std::set< BlockList* >::iterator l;
            for ( l = lists.begin() ; l != lists.end() ; l++ )
                if( (*l)->task_id < _task_id )
                    _task_id= (*l)->task_id;
            
            return _task_id;
        }
        
        
        bool Block::compare( Block *b )
        {
            
            size_t my_task_id   = getMinTaskId();
            size_t your_task_id = b->getMinTaskId();
            
            return( my_task_id < your_task_id );
        }
        
        ::engine::DiskOperation* Block::getWriteOperation()
        {
            if( !buffer )
                LM_X(1,("Not possible to get write operation over a block that it is not in memory"));
            
            engine::DiskOperation* operation = engine::DiskOperation::newWriteOperation( buffer ,  SamsonSetup::shared()->blockFileName(id) , getEngineId()  );
            operation->addListener( BlockManager::shared()->getEngineId() );
            
            // set my id as environment property
            operation->environment.set("block_id" , id );
            
            return operation;
            
        }

        ::engine::DiskOperation* Block::getReadOperation()
        {
            if( !buffer )
                LM_X(1,("Not possible to get a read operation over a block that has not a buffer  in memory"));
            
            engine::DiskOperation* operation;

            // Create the operation
            std::string fileName = SamsonSetup::shared()->blockFileName(id);
            operation = engine::DiskOperation::newReadOperation( fileName.c_str(), 0, size, buffer->getSimpleBuffer() , getEngineId() );
            
            // Also add as listener the block manager to deal with reordering after this read opertion is finished
            operation->addListener( BlockManager::shared()->getEngineId() );
            
            // set my id as environment property ( used in BlockManager to understand what block is about )
            operation->environment.set("block_id" , id );
            
            return operation;
        }
        


        // Command over the block
        
        void Block::write()
        {
            if (state != on_memory )
            {
                LM_W(("No sense to call write for a block that state != on_memory "));
                return;
            }
            
            state = writing;
            
            engine::DiskOperation *diskOperation = getWriteOperation();
            diskOperation->environment.set(destroy_buffer_after_write, "no" );

            engine::DiskManager::shared()->add( diskOperation );
        }

        void Block::read()
        {
            if( state != on_disk )
            {
                LM_W(("No sense to call read for a block that state != on_disk"));
                return;
            }
            
            if ( buffer )
            {
                LM_W(("There is an unused buffer of data in a block with state = on_disk"));
                engine::MemoryManager::shared()->destroyBuffer(buffer);
                buffer = NULL;
            }
            
            
            // Allocate a buffer
            buffer = engine::MemoryManager::shared()->newBuffer("block", size, 0 ); 
            buffer->setSize( size );    // Set the final size ( only used after read opertion has finished )
            
            // Change the state to reading
            state = reading;

            // Schedule the read operation
            engine::DiskManager::shared()->add( getReadOperation() );
            
        }
                
        void Block::freeBlock()
        {
            if( state != ready )
            {
                LM_W(("No sense to call free to a Block that state != ready "));
                return;
            }

            if( isLockedInMemory() )
            {
                LM_W(("Not possible to free from memory a block that is locked "));
                return;
            }
            
            if( !buffer )
            {
                LM_W(("Buffer not present in a ready block"));
                return;
            }
            
            state = on_disk;
            
            engine::MemoryManager::shared()->destroyBuffer(buffer);
            buffer = NULL;
            
        }
        
        
        // Notitifications
        
        void Block::notify( engine::Notification* notification )
        {
            // A response for a disk operation?
            if( notification->isName( notification_disk_operation_request_response ) )
            {
                // Notify a read or write
                engine::DiskOperation *operation = (engine::DiskOperation*) notification->extractObject();
                delete operation;

                // What ever operation it was it is allways ready
                state = ready;
                
                
                if( canBeRemoved() )
                    BlockManager::shared()->check( this );
            }
            
        }

        // Get information about this block
        void Block::update( BlockInfo &block_info)
        {
            // Information about number of blocks
            block_info.num_blocks++;
            
            // Information about sizes
            block_info.size += size;
            if( isContentOnMemory() )
                block_info.size_on_memory += size;
            if( isContentOnDisk() )
                block_info.size_on_disk += size;
            
            if( isLockedInMemory() )
                block_info.size_locked += size;
            
            // Key-Value information
            block_info.info.append( header->info );
            
            block_info.push( getKVFormat() );
            
            block_info.pushTime( header->time );
            
            // Accumulate the number of divisions
            block_info.accumulate_divisions += getKVRange().getMaxNumDivisions();
            
        }
        
        size_t getSize( std::set<Block*> &blocks )
        {
            size_t total = 0;
            
            std::set<Block*>::iterator i;
            for ( i = blocks.begin() ; i != blocks.end () ; i++ )
                total += (*i)->getSize();
            
            return total;
        }
        
        std::string Block::str()
        {
            std::ostringstream output;
            output << "[ ";
            //output << "Task:" << task_id << " order: " << task_order << " ";
            if( header )
                output << "HG " << header->range.str();
            output << " ]";
            return output.str();
        }
        
        void Block::getInfo( std::ostringstream& output)
        {
            au::xml_open(output, "block");

            au::xml_simple(output, "id", id);
            au::xml_simple(output, "size", size);
            au::xml_simple(output, "state", getState());

            au::xml_open(output, "lists");
            std::set< BlockList* >::iterator l;
            for (l = lists.begin() ; l != lists.end() ; l++)
                au::xml_simple(output, "list", (*l)->name );
            au::xml_close(output, "lists");
            
            header->getInfo( output );

            au::xml_close(output, "block");
        }
        
        // Check if this block is not in any list anymore
        int Block::getNumberOfLists()
        {
            return lists.size();
        }
        
        // Function to check if this block can be removed from block manager ( basically it is not contained anywhere )
        int Block::canBeRemoved()
        {
            if( lists.size() != 0)
                return false;
            
            if( state == reading )
                return false;
            
            if( state == writing )
                return false;
            
            return true;
        }        
        
        bool Block::isOnDisk()
        {
            return ( state == on_disk );
        }
        
        bool Block::isWriting()
        {
            return ( state == writing );
        }
        
        bool Block::isReading()
        {
            return ( state == reading );
        }
        
        bool Block::isOnMemory()
        {
            return (state == on_memory);
        }
        
        bool Block::isReady()
        {
            return (state == ready);
        }
        
        bool Block::isContentOnMemory()
        {
            return (  (state == ready ) || (state == on_memory) || ( state == writing ));
        }
        
        bool Block::isContentOnDisk()
        {
            return (  (state == ready ) || (state == on_disk) || ( state == reading ));
        }
        
        
        size_t Block::getSize()
        {
            return size;
        }
        
        char *Block::getData()
        {
            if( ! isContentOnMemory() )
                LM_X(1,("Not possible to get data for a block that is not in memory"));
            return buffer->getData();
        }
        
        
        size_t Block::getSizeOnMemory()
        {
            if( isContentOnMemory() )
                return size;
            else
                return 0;
        }
        
        size_t Block::getSizeOnDisk()
        {
            if( isContentOnDisk() )
                return size;
            else
                return 0;
        }
        
        bool Block::isNecessaryForKVRange( KVRange range )
        {
            if( !header )
                return true;
            
            return header->range.overlap( range );
        }
        
        KVRange Block::getKVRange()
        {
            return header->range;
        }
        
        size_t Block::getId()
        {
            return id;
        }
        
        KVInfo* Block::getKVInfo()
        {
            KVInfo *info = (KVInfo *) ( buffer->getData() + sizeof( KVHeader ) );
            return info;
        }
        
        KVFormat Block::getKVFormat()
        {
            return header->getKVFormat();
        }




    }
}