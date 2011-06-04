
#include "Block.h"                      // Own interface
#include "BlockManager.h"               // BlockManager
#include "samson/common/SamsonSetup.h"                // samson::SamsonSetup
#include "engine/DiskOperation.h"       // engine::DiskOperation
#include "engine/DiskManager.h"        // notification_disk_operation_request_response
#include "engine/Engine.h"              // engine::Engine
#include "logMsg/logMsg.h"                     // LM_W
#include "engine/MemoryManager.h"       // engine::MemoryManager
#include "samson/common/MemoryTags.h"                 // MemoryBlocks

namespace samson {
    namespace stream
    {

        
        std::ostream &operator<<(std::ostream &out, const Block &b)
        {
            out << "[";
            
            out << " " << b.id;
            
            if( b.tasks.size() > 0 )
            {
                out << " Tasks: <";
                std::set<size_t>::iterator i;
                for ( i=b.tasks.begin() ; i != b.tasks.end() ; )
                {
                    out << *i;
                    i++;
                    if( i != b.tasks.end() )
                        out << ",";
                }
                out << "> ";
            }
            
            if( b.retain_counter > 0 )
                out << " Retain " << b.retain_counter << " ";
            
            if( b.priority != 0)
                out <<  " Priority (" << b.priority << ")";
            
            switch (b.state) {
                case Block::on_memory:
                    out << " M ";
                    break;
                case Block::on_disk:
                    out << "  D";
                    break;
                case Block::ready:
                    out << " MD";
                    break;
                case Block::writing:
                    out << " MW";
                    break;
                case Block::reading:
                    out << " RD";
                    break;
            }
            
            out << " ]";
            return out;
        }
     
        
        Block::Block( engine::Buffer *_buffer , bool txt )
        {
            // Buffer of data
            buffer = _buffer;  
            buffer->tag = MemoryBlocks;     // Set the tag to MemoryBlock to controll the memory used by this system
            
            // DEfault priority
            priority = 0;
            
            // Get the size of the packet
            size = buffer->getSize();
            
            // By default it is used when created
            retain_counter = 1;
            
            // Currenyly not used
            lock_counter = 0;

            // Default state is on_memory because the buffer has been given at memory
            state = on_memory;
            
            // Add automatically to the Block Manager
            BlockManager::shared()->add(this);   

            // Add this object as a listener of notification_disk_operation_request_response
            listen( notification_disk_operation_request_response );
            

            if( !txt )
            {
                header = (KVHeader*) malloc( sizeof( KVHeader ) );
                memcpy(header, buffer->getData(), sizeof(KVHeader));
            }
            else
                header = NULL;

            // By default no task & order ( push from delilah )
            task_id = 0;             
            order=0;
            
        }

        Block::~Block()
        {
            
            // Destroy buffer if still in memory
            if( buffer )
                engine::MemoryManager::shared()->destroyBuffer( buffer );
        }
        
        
        // Set priority
        void Block::setPriority( int _priority )
        {
            priority = _priority;
            BlockManager::shared()->reorder( this );
        }
        
        int Block::getPriority()
        {
            return priority;
        }
        
        bool Block::compare( Block *b )
        {
            if( tasks.size() > 0 )
            {
                if( b->tasks.size() > 0)
                {
                    
                    // Both are associated to some task
                    
                    size_t my_task_id = *tasks.begin();
                    size_t your_task_id = *b->tasks.begin();
                    
                    return( my_task_id < your_task_id );
                }
                else
                    return true;// I am retained by some task
            }
            else
            {
                if ( b->tasks.size() > 0)
                    return false;
            }
            
            // Here no one of the blocks are retained by any task, so this is just a matter of priority
            return ( priority >= b->priority);
            
        }
        
        ::engine::DiskOperation* Block::getWriteOperation()
        {
            if( !buffer )
                LM_X(1,("Not possible to get write operation over a block that it is not in memory"));
            
            return engine::DiskOperation::newWriteOperation( buffer ,  getFileName()  );
        }

        ::engine::DiskOperation* Block::getReadOperation()
        {
            if( !buffer )
                LM_X(1,("Not possible to get a read operation over a block that has not a buffer  in memory"));
            
            return engine::DiskOperation::newReadOperation( getFileName(), 0, size, buffer->getSimpleBuffer() );
        }
        
        std::string Block::getFileName()
        {
            return au::Format::string( "%s/block_%lu", SamsonSetup::shared()->blocksDirectory.c_str()  , id );
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
            
            engine::Notification *notification = new engine::Notification( notification_disk_operation_request ,diskOperation );
            notification->environment.set("target", "Block");
            notification->environment.setSizeT("id", id);
            notification->environment.set("type","write");
            
            engine::Engine::add( notification );
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
            
            engine::Notification *notification = new engine::Notification( notification_disk_operation_request , getReadOperation() );
            notification->environment.set("target", "Block");
            notification->environment.setSizeT("id", id);
            notification->environment.set("type","read");
            
            engine::Engine::add( notification );
            
        }
                
        void Block::freeBlock()
        {
            if( state != ready )
            {
                LM_W(("No sense to call free to a Block that state != ready "));
                return;
            }

            if( lock_counter > 0 )
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
            }
            
        }
        
        bool Block::acceptNotification( engine::Notification* notification )
        {
            if( notification->isName( notification_disk_operation_request_response ) )
                if( notification->environment.get("target", "") == "Block"  )
                    if( notification->environment.getSizeT("id", 0) == id  )
                        return true;
            return false;
        }


        
        size_t getSize( std::set<Block*> &blocks )
        {
            size_t total = 0;
            
            std::set<Block*>::iterator i;
            for ( i = blocks.begin() ; i != blocks.end () ; i++ )
                total += (*i)->getSize();
            
            return total;
        }
        
        
        
        
        

    }
}