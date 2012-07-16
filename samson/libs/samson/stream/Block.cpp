
#include "logMsg/logMsg.h"                          // LM_W


#include "au/mutex/TokenTaker.h"

#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/DiskManager.h"                     // notification_disk_operation_request_response
#include "engine/Engine.h"                          // engine::Engine
#include "engine/Notification.h"                    // engine::Notification
#include "engine/MemoryManager.h"                   // engine::MemoryManager
#include "engine/ProcessManager.h"

#include "samson/common/MessagesOperations.h"
#include "samson/common/MemoryTags.h"               // MemoryBlocks
#include "samson/common/SamsonSetup.h"              // samson::SamsonSetup
#include "samson/common/KVHeader.h"

#include "samson/module/KVFormat.h"
#include "samson/module/ModulesManager.h"           // ModulesManager

#include "PopQueueTask.h"                           // samson::stream::PopQueueTask
#include "BlockManager.h"                           // BlockManager
#include "BlockList.h"                              // BlockList

#include "Block.h"                                  // Own interface



namespace samson {
    namespace stream
    {
 
        const char* Block::getState()
        {
            switch (state) {
                case building:
                    return "B ";
                case Block::on_memory:
                    if( isLockedInMemory() )
                        return "L ";
                    else
                        return "M ";
                    break;
                case Block::on_disk:
                    return " D";
                    break;
                case Block::ready:
                    if( isLockedInMemory() )
                        return "LD";
                    else
                        return "MD";
                    break;
                case Block::writing:
                    if( isLockedInMemory() )
                        return "LW";
                    else
                        return "MW";
                    break;
                case Block::reading:
                    return "RD";
                    break;
            }
            
            return "Unknown";
        }
        
        Block::Block( engine::BufferListContainer *buffer_list_container  ) : token_lookupList("token_lookupList")
        {
            
            size_t num_buffers = buffer_list_container->getNumBuffers();
            
            if( num_buffers == 0 )
                LM_X(1, ("Internal error: No buffer when creating block"));

            if( num_buffers == 1 )
            {
                // Get the first (unique) buffer
                engine::Buffer *buffer = buffer_list_container->front();
                // Set name of buffer
                buffer->setNameAndType( au::str("%lu-%lu" , worker_id , id) , "blocks" );
                
                // Get the size of the buffer
                size = buffer->getSize();

                // Keep in the intenal buffer container
                buffer_container.setBuffer(buffer);
                
                // Default state is on_memory because the buffer has been given at memory
                state = on_memory;

                // Get a new unique id from the block manager
                worker_id = BlockManager::shared()->getWorkerId();
                id = BlockManager::shared()->getNextBlockId();
                
                // Get a copy of the header
                header = (KVHeader*) malloc( sizeof( KVHeader ) );
                memcpy(header, buffer->getData(), sizeof(KVHeader));
                
                LM_T(LmtBlockManager, ("Block created from buffer: %s", this->str().c_str()));
                lookupList = NULL;
                
                // First idea of sort information
                update_sort_information();

            }
            else
            {
                KVHeader* tmp_header = (KVHeader*) buffer_list_container->front()->getData();
                KVFormat format = tmp_header->getKVFormat();
                KVInfo info;
                KVRange range = tmp_header->range;

                // Buffer finally considered to create the block
                BlockBuilder * block_builder = new BlockBuilder(this);

                while( buffer_list_container->getNumBuffers() > 0 )
                {
                    engine::Buffer* buffer = buffer_list_container->front();
                    
                    KVHeader* tmp_header = (KVHeader*) buffer->getData();
                    if( tmp_header->getKVFormat() != format )
                    {                        
                        LM_W(("Ignoring a buffer %s with format %s when creating a block with format %s"
                              , au::str( buffer->getSize() ).c_str() 
                              , tmp_header->getKVFormat().str().c_str()
                              , format.str().c_str() ));
                    }
                    else
                    {
                        // Increase range to cover all individual buffers
                        range.add( tmp_header->range );
                        
                        // Update counter for the final header
                        info.append( tmp_header->info );
                        
                        // Append total info
                        block_builder->buffer_list_container.push_back(buffer);
                    }
                    
                    buffer_list_container->pop();
                }
                
                // Compute the total size and create the buffer
                size = sizeof( KVHeader ) + info.size;
                buffer_container.create(au::str("%lu-%lu" , worker_id , id) , "blocks"  , size );
                
                // Copy the header to the final buffer and set the final size
                KVHeader* final_header = (KVHeader*) buffer_container.getBuffer()->getData();
                memcpy( final_header , tmp_header, sizeof(KVHeader));
                final_header->setInfo(info);
                final_header->range = range; // Set the global range
                
                // State will be building until a BlockBuildtask is exewcuted
                state = building;

                // Get a new unique id from the block manager
                worker_id = BlockManager::shared()->getWorkerId();
                id = BlockManager::shared()->getNextBlockId();
                
                // Get a copy of the header
                engine::Buffer *buffer = buffer_container.getBuffer();
                if(!buffer)
                    LM_X(1, ("Internal error"));
                header = (KVHeader*) malloc( sizeof( KVHeader ) );
                memcpy(header, buffer->getData(), sizeof(KVHeader));
                
                LM_T(LmtBlockManager, ("Block created from buffer: %s", this->str().c_str()));
                lookupList = NULL;
                
                // First idea of sort information
                update_sort_information();

                
                // Schedule the operation
                engine::ProcessManager::shared()->add( block_builder , getEngineId() );
            }
            
        }
        
        Block::Block( size_t _worker_id , size_t _id , size_t _size , KVHeader* _header ) : token_lookupList("token_lookupList")
        {
            // Get a new unique id from the block manager
            worker_id = _worker_id;
            id = _id;
            
            // Get the size of the packet
            size = _size;
            
            // Default state is on_memory because the buffer has been given at memory
            state = on_disk;
            
            // Get a copy of the header
            header = (KVHeader*) malloc( sizeof( KVHeader ) );
            memcpy(header, _header , sizeof(KVHeader));
            
            LM_T(LmtBlockManager,("Block created from id: %s", this->str().c_str()));

            lookupList = NULL;
            
            // First idea of sort information
            update_sort_information();
        }
        

        Block::~Block()
        {
            if( lookupList )
            {
                delete lookupList;
                lookupList = NULL;
            }

            if (header != NULL)
            {
                free(header);
                header = NULL;
            }

        }
        
        void Block::update_sort_information()
        {
            // Compute min tasks
            // ---------------------------------------------------
            min_task = (size_t) -1;
            std::set< BlockList* >::iterator l;
            for ( l = lists.begin() ; l != lists.end() ; l++ )
                if( (*l)->task_id < min_task )
                    min_task= (*l)->task_id;
            
            // Compute max priority
            // ---------------------------------------------------
            max_priority = -1000000;
            for ( l = lists.begin() ; l != lists.end() ; l++ )
                if( (*l)->priority > max_priority )
                    max_priority = (*l)->priority;
            
            
            // Compute compare_time 
            // ---------------------------------------------------
            compare_time = (size_t)-1;
            order = (size_t)-1;
            for ( l = lists.begin() ; l != lists.end() ; l++ )
                if( (*l)->queue ) // Only list that are proper queues affect to this
                {
                    size_t tmp_compare_time = (*l)->getOldestBlockTime();
                    if( tmp_compare_time < compare_time )
                    {
                        compare_time = tmp_compare_time;
                        order = (*l)->getPosition(this);
                    }
                }
            
        }

        
        
        bool Block::isLockedInMemory()
        {
            // If building this block, it is considered locked ( cannot be removed )
            if( state == building )
                return true;
            
            std::set< BlockList* >::iterator l;
            for ( l = lists.begin() ; l != lists.end() ; l++ )
                if( (*l)->lock_in_memory )
                    return true;
            
            return false;
        }

        ::engine::DiskOperation* Block::getWriteOperation()
        {
            // Get a pointer to the internal buffer ( retained in buffer_container )
            engine::Buffer * buffer = buffer_container.getBuffer();
            
            if( !buffer )
                LM_X(1,("Not possible to get write operation over a block that it is not in memory"));
            
            engine::DiskOperation* operation = engine::DiskOperation::newWriteOperation( buffer 
                                                                                        ,  getFileName() 
                                                                                        , getEngineId()  );
            operation->addListener( BlockManager::shared()->getEngineId() );
            
            // set my id as environment property
            operation->environment.set("block_id" , id );
            
            return operation;
            
        }

        ::engine::DiskOperation* Block::getReadOperation()
        {
            // Get a pointer to the internal buffer ( retained in buffer_container )
            engine::Buffer * buffer = buffer_container.getBuffer();

            if( !buffer )
                LM_X(1,("Not possible to get a read operation over a block that has not a buffer  in memory"));
            
            engine::DiskOperation* operation;

            // Create the operation
            std::string fileName = getFileName();
            operation = engine::DiskOperation::newReadOperation( fileName.c_str(), 0, size, buffer->getSimpleBuffer() , getEngineId() );
            
            // Also add as listener the block manager to deal with reordering after this read opertion is finished
            operation->addListener( BlockManager::shared()->getEngineId() );
            
            // set my id as environment property ( used in BlockManager to understand what block is about )
            operation->environment.set("block_id" , id );
            
            return operation;
        }
        
        
        std::string Block::getFileName()
        {
            return SamsonSetup::shared()->blockFileName( worker_id , id );
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
            diskOperation->environment.set("operation_size", getSize() );

            engine::DiskManager::shared()->add( diskOperation );
            
            // Relase object since now it is retained by DiskManager....
            diskOperation->release();
        }

        void Block::read()
        {
            if( state != on_disk )
            {
                LM_W(("No sense to call read for a block that state != on_disk"));
                return;
            }
            
            if ( buffer_container.getBuffer() )
            {
                LM_W(("There is an unused buffer of data in a block with state = on_disk"));
                // No problem since previous buffer is auytomatically released in buffer_container
            }
            
            // Allocate a buffer ( it is retained since we are the creators )
            std::string buffer_title = au::str("%lu-%lu" , worker_id , id);
            engine::Buffer* buffer = engine::MemoryManager::shared()->createBuffer( buffer_title  , "blocks", size ); 
            buffer->setSize( size );    // Set the final size ( only used after read opertion has finished )
            
            // Release indice the buffer_container and release the just created buffer
            buffer_container.setBuffer( buffer );
            buffer->release();
            
            // Change the state to reading
            state = reading;

            // Schedule the read operation
            engine::DiskOperation *diskOperation = getReadOperation();
            diskOperation->environment.set("operation_size", getSize() );
            engine::DiskManager::shared()->add( diskOperation );
         
            // Relase object since now it is retained by DiskManager....
            diskOperation->release();
            
        }
                
        void Block::freeBlock()
        {
            // Remove the lookup table if exist
            {
                au::TokenTaker tt(&token_lookupList);
                if( lookupList )
                {
                    delete lookupList;
                    lookupList = NULL;
                }
            }
            
            
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

            state = on_disk;

            if( !buffer_container.getBuffer() )
            {
                LM_W(("Buffer not present in a ready block"));
                return;
            }

            LM_T(LmtBlockManager,("destroyBuffer for block:'%s'", str().c_str()));

            // Relase buffer
            buffer_container.release();
            

        }
        
        // Notifications
        
        void Block::notify( engine::Notification* notification )
        {
            // A response for a disk operation?
            if( notification->isName( notification_disk_operation_request_response ) )
            {

                // Whatever operation it was it is always ready ( comming from reading or writing )
                state = ready;
                
                LM_T(LmtBlockManager,("Block::notify block state set to ready for block:'%s'", str().c_str()));
                return;
            }
            
            if( notification->isName( notification_process_request_response ) )
            {
                BlockBuilder* block_builder = (BlockBuilder*) notification->getObject();
                
                if( block_builder )
                {
                    
                    if( block_builder->error.isActivated() )
                    {
                        LM_W(("Error received when creating a block: %s" , 
                              block_builder->error.getMessage().c_str() ));
                    }
                    
                }
                else
                    LM_W(("No block_builder in a process item operation notification received in a Block"));

                // Response from build
                return;
            }
            
            LM_W(("Unknown notification at Block: %s" , notification->getDescription().c_str() ));
            
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
                output << " id=" << worker_id << "-" << id << " size=" << size << " " <<  header->range.str() << "(" << getState() << ")";
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
            std::ostringstream output_lists;
            std::ostringstream output_tasks_str;
            
            std::set< BlockList* >::iterator l;
            std::set<size_t> tasks_ids;
            for (l = lists.begin() ; l != lists.end() ; l++)
            {
                au::xml_simple(output, "list", (*l)->name );
                output_lists << (*l)->name << " ";

                // Show tasks ids
                size_t task_id = (*l)->task_id;
                if( task_id != (size_t) -1 )
                    tasks_ids.insert((*l)->task_id);
            }

            for ( std::set<size_t>::iterator it_tasks_ids = tasks_ids.begin() ; it_tasks_ids != tasks_ids.end() ; it_tasks_ids++ )
                output_tasks_str << (*it_tasks_ids) << " ";
            
            au::xml_close(output, "lists");
            
            au::xml_simple(output, "lists_str", output_lists.str() );
            
            header->getInfo( output );


            au::xml_simple(output, "tasks_str", output_tasks_str.str() );
            
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
            
            if( state == building )
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
            return buffer_container.getBuffer()->getData();
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
        
        KVFormat Block::getKVFormat()
        {
            return header->getKVFormat();
        }


        /*
         au::xml_open(output, "block");
         
         
         au::xml_open(output, "lists");
         std::ostringstream output_lists;
         std::ostringstream output_tasks_str;
         
         std::set< BlockList* >::iterator l;
         std::set<size_t> tasks_ids;
         for (l = lists.begin() ; l != lists.end() ; l++)
         {
         au::xml_simple(output, "list", (*l)->name );
         output_lists << (*l)->name << " ";
         
         // Show tasks ids
         size_t task_id = (*l)->task_id;
         if( task_id != (size_t) -1 )
         tasks_ids.insert((*l)->task_id);
         }
         
         for ( std::set<size_t>::iterator it_tasks_ids = tasks_ids.begin() ; it_tasks_ids != tasks_ids.end() ; it_tasks_ids++ )
         output_tasks_str << (*it_tasks_ids) << " ";
         
         au::xml_close(output, "lists");
         
         au::xml_simple(output, "lists_str", output_lists.str() );
         
         header->getInfo( output );
         
         
         au::xml_simple(output, "tasks_str", output_tasks_str.str() );
         
         au::xml_close(output, "block");
         */
        
        void Block::fill( samson::network::CollectionRecord* record , Visualization* visualization )
        {
            samson::add( record , "id" , au::str("%lu-%lu" , worker_id ,id) , "left,different" );
            
            samson::add( record , "size" , size , "f=uint64,sum" );
            samson::add( record , "state" , getState() , "left,different" );

            samson::add( record , "KVRange" , getKVRange().str() , "left,different" );

            // Next task
            if( min_task == (size_t)(-1))
                samson::add( record , "next task" , "none" , "left,different" );
            else
                samson::add( record , "next task" , min_task , "left,different" );

            samson::add( record , "priority" , max_priority , "left,different" );
            
            samson::add( record , "created" , cronometer.str() , "left,different" );

            if( compare_time == (size_t)-1 )
                samson::add( record , "sort_time" , "-" , "left,different" );
            else
                samson::add( record , "sort_time" , au::str_time( compare_time ) , "left,different" );

            if( order == (size_t)-1 )
                samson::add( record , "order in queue" , "-" , "left,different" );
            else
                samson::add( record , "order in queue" , order , "left,different" );
            
            std::ostringstream output_tasks_str;
            std::ostringstream output_block_lists_str;
            
            std::set< BlockList* >::iterator l;
            std::set<size_t> tasks_ids;
            for (l = lists.begin() ; l != lists.end() ; l++)
            {
                // List of lists where this block is included
                BlockList* block_list = *l;
                output_block_lists_str << block_list->strShortDescription() << " ";
                
                // Show tasks ids
                if( block_list->task_id != (size_t) -1 )
                    output_tasks_str << block_list->task_id << " ";
            }
            
            samson::add( record , "tasks" , output_tasks_str.str() ,       "left,different" );
            samson::add( record , "lists" , output_block_lists_str.str() , "left,different" );
            
            if (visualization == NULL)
                return;
        }

        //au::Token token_lookupList;
        //BlockLookupList* lookupList;

        std::string Block::lookup(const char* key, std::string outputFormat)
        {
            // Mutex preotection
            au::TokenTaker tt( &token_lookupList );

            // We should check if the block can be locked in memory...
            
            if( !lookupList )
            {
                lookupList = new BlockLookupList( this );
                
                // Detect error during creating
                if( lookupList->error.isActivated() )
                {
                    LM_E(("Error creating BlockLookupList (%s)" , lookupList->error.getMessage().c_str()));
                    delete lookupList;
                    lookupList = NULL;
                    return au::xml_simple("error", "Error creating BlockLookupList");
                }
            }
            else
            {
                LM_M(("lookupList already created for block:%lu", id));
            }
            
            return lookupList->lookup(key, outputFormat);
        }
        


    }
}
