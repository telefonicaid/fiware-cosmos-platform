
#include "logMsg/logMsg.h"                     // LM_W


#include "engine/DiskOperation.h"       // engine::DiskOperation
#include "engine/DiskManager.h"        // notification_disk_operation_request_response
#include "engine/Engine.h"              // engine::Engine
#include "engine/Notification.h"       // engine::Notification
#include "engine/MemoryManager.h"       // engine::MemoryManager

#include "samson/common/MemoryTags.h"                 // MemoryBlocks
#include "samson/common/SamsonSetup.h"                // samson::SamsonSetup


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
        
        
        Block::Block( engine::Buffer *_buffer , bool txt )
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
            

            if( !txt )
            {
                header = (KVHeader*) malloc( sizeof( KVHeader ) );
                memcpy(header, buffer->getData(), sizeof(KVHeader));
            }
            else
                header = NULL;

            
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
            
            engine::DiskOperation* operation = engine::DiskOperation::newWriteOperation( buffer ,  getFileName() , getEngineId()  );
            operation->addListener( BlockManager::shared()->getEngineId() );
            
            // set my id as environment property
            operation->environment.set("block_id" , id );
            
            return operation;
            
        }

        ::engine::DiskOperation* Block::getReadOperation()
        {
            if( !buffer )
                LM_X(1,("Not possible to get a read operation over a block that has not a buffer  in memory"));
            
            engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( getFileName(), 0, size, buffer->getSimpleBuffer() , getEngineId() );
            operation->addListener( BlockManager::shared()->getEngineId() );
            
            // set my id as environment property
            operation->environment.set("block_id" , id );
            
            return operation;
        }
        
        std::string Block::getFileName()
        {
            return au::str( "%s/block_%lu", SamsonSetup::shared()->blocksDirectory.c_str()  , id );
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
            
            state = reading;

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

        KVInfo Block::getKVInfo()
        {
            if( header )
                return header->info;
            else
                return KVInfo( size , 0 );
        }

        KVInfo Block::getKVInfo( KVRange r )
        {
            if( header )
            {
                if( !infos.isInMap(r) )
                {
                    LM_W(("Information for this range %s was not previously computed" , r.str().c_str()));
                    return KVInfo( 0 , 0 );
                }
                
                return infos.findInMap( r );
            }
            else
                return KVInfo( size , 0 );
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
            output << "<block id=\"" << id << "\" size=\"" << size << "\" state=\"" << getState() << "\">\n";

            std::set< BlockList* >::iterator l;
            for (l = lists.begin() ; l != lists.end() ; l++)
                output << "<list name=\"" << (*l)->name << "\" />";

            output << "</block>\n";
        }
        
        
        

    }
}