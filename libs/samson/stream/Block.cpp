
#include "logMsg/logMsg.h"                          // LM_W


#include "au/TokenTaker.h"

#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/DiskManager.h"                     // notification_disk_operation_request_response
#include "engine/Engine.h"                          // engine::Engine
#include "engine/Notification.h"                    // engine::Notification
#include "engine/MemoryManager.h"                   // engine::MemoryManager

#include "samson/common/MessagesOperations.h"
#include "samson/common/MemoryTags.h"               // MemoryBlocks
#include "samson/common/SamsonSetup.h"              // samson::SamsonSetup
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
        
        
        Block::Block( engine::Buffer *_buffer ) : token_lookup_creation("lookupCreation")
        {
            // Get a new unique id from the block manager
            worker_id = BlockManager::shared()->getWorkerId();
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

            LM_T(LmtBlockManager, ("Block created from buffer: %s", this->str().c_str()));

            lookupList.head     = NULL;
            lookupList.hashInfo = NULL;
        }
        
        Block::Block( size_t _worker_id , size_t _id , size_t _size , KVHeader* _header ) : token_lookup_creation("lookupCreation")
        {
            // Get a new unique id from the block manager
            worker_id = _worker_id;
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
            
            LM_T(LmtBlockManager,("Block created from id: %s", this->str().c_str()));

            lookupList.head     = NULL;
            lookupList.hashInfo = NULL;
        }
        

        Block::~Block()
        {
            if (lookupList.head != NULL)
                free(lookupList.head);
            if (lookupList.hashInfo != NULL)
                free(lookupList.hashInfo);

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
        
        size_t Block::getLiveTime()
        {
            return last_used.diffTimeInSeconds();
        }

        
        bool Block::compare( Block *b )
        {
            
            size_t my_task_id   = getMinTaskId();
            size_t your_task_id = b->getMinTaskId();
            
            
            // If no tasks involved, just decide by time-stamp
            if( my_task_id == (size_t)-1 )
                if( your_task_id == (size_t)-1 )
                    return (last_used.diffTimeInSeconds() < b->last_used.diffTimeInSeconds() );
            
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
            
            LM_T(LmtBlockManager,("destroyBuffer for block:'%s'", str().c_str()));

            engine::MemoryManager::shared()->destroyBuffer(buffer);
            buffer = NULL;
            
        }
        
        
        // Notifications
        
        void Block::notify( engine::Notification* notification )
        {
            // A response for a disk operation?
            if( notification->isName( notification_disk_operation_request_response ) )
            {
                // Notify a read or write
                engine::DiskOperation *operation = (engine::DiskOperation*) notification->extractObject();
                delete operation;

                // Whatever operation it was it is always ready
                state = ready;
                
                LM_T(LmtBlockManager,("Block::notify block state set to ready for block:'%s'", str().c_str()));
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
                output << "HG id=" << id << " size=" << size << " " <<  header->range.str() << "(" << getState() << ")";
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
        	// If block has been read because of "not in memory" requests, we don't want to remove it too soon;
            // Andreu: Review mechanism to avoid histeresis...
            /*
        	if ((state == ready) && (requests > 0))
        	{
        		requests--;
        		return false;
        	}
             */
            
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
            
            size_t task = getMinTaskId();

            if( task == (size_t)(-1))
                samson::add( record , "next task" , "none" , "left,different" );
            else
                samson::add( record , "next task" , task , "left,different" );
            
            samson::add( record , "created" , au::str_time( last_used.diffTime() ) , "left,different" );
            
            std::ostringstream output_tasks_str;
            std::ostringstream output_block_lists_str;
            
            std::set< BlockList* >::iterator l;
            std::set<size_t> tasks_ids;
            for (l = lists.begin() ; l != lists.end() ; l++)
            {
                BlockList* block_list = *l;
                output_block_lists_str << block_list->name << " ";
                
                // Show tasks ids
                if( block_list->task_id != (size_t) -1 )
                    output_tasks_str << block_list->task_id << " ";
            }
            
            samson::add( record , "tasks" , output_tasks_str.str() , "left,different" );
            samson::add( record , "lists" , output_block_lists_str.str() , "left,different" );
            
        }



        void Block::lookupListCreate(void)
        {
            au::TokenTaker tt(&token_lookup_creation);

            LM_T(LmtRest, ("Creating lookup list"));
            // semTake();
            lookupList.head = (BlockLookupRecord*) calloc(header->info.kvs, sizeof(BlockLookupRecord));
            lookupList.size = header->info.kvs;

            if (lookupList.head == NULL)
                LM_RVE(("Error allocating lookupList.head of %d bytes", header->info.kvs * sizeof(BlockLookupRecord)));

            lookupList.hashInfo = (BlockHashLookupRecord*) calloc(KVFILE_NUM_HASHGROUPS, sizeof(BlockHashLookupRecord)); 
            if (lookupList.hashInfo == NULL)
            {
                free(lookupList.head);
                lookupList.head = NULL;
                LM_RVE(("Error allocating lookupList.hashInfo of %d bytes", KVFILE_NUM_HASHGROUPS * sizeof(BlockHashLookupRecord)));
            }

            LM_T(LmtRest, ("Created a lookup list for %d records", header->info.kvs));
            
            unsigned int  hashIx;
            unsigned int  kvIx;
            KVFormat      kvFormat            = getKVFormat();
            unsigned int  offset              = 0;
            unsigned int  noOfKvs             = 0;
            KVInfo*       kvInfoV             = (KVInfo*) (getData() + sizeof(KVHeader)); 
            char*         kvsStart            = getData() + sizeof(KVHeader) + sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS;
            Data*         keyData             = ModulesManager::shared()->getData(kvFormat.keyFormat);
            Data*         valueData           = ModulesManager::shared()->getData(kvFormat.valueFormat);
            DataInstance* keyDataInstance     = (DataInstance*) keyData->getInstance();
            DataInstance* valueDataInstance   = (DataInstance*) valueData->getInstance();
            int           maxEntries          = 0;
            int           maxEntryHashGroup   = -1;

            for (hashIx = 0; hashIx < KVFILE_NUM_HASHGROUPS; hashIx++)
            {
                if (kvInfoV[hashIx].kvs != 0)
                {
                    int entries = kvInfoV[hashIx].kvs;

                    if (entries > maxEntries)
                    {
                        maxEntries        = entries;
                        maxEntryHashGroup = hashIx;
                    }

                    LM_T(LmtRest, ("setting hashInfo[%d]: %d-%d (%d entries in hashgroup - max entries is %d)", hashIx, noOfKvs, noOfKvs + kvInfoV[hashIx].kvs, entries, maxEntries));
                }

                lookupList.hashInfo[hashIx].startIx = noOfKvs;
                lookupList.hashInfo[hashIx].endIx   = noOfKvs + kvInfoV[hashIx].kvs;

                for (kvIx = 0; kvIx < kvInfoV[hashIx].kvs; kvIx++)
                {
                    int keySize   = keyDataInstance->parse(kvsStart + offset);
                    int valueSize = valueDataInstance->parse(kvsStart + offset + keySize);

                    lookupList.head[noOfKvs].keyP = kvsStart + offset;

                    offset += (keySize + valueSize);
                    ++noOfKvs;
                }
            }
            LM_T(LmtRest, ("Hash Group %d has %d entries", maxEntryHashGroup, maxEntries));

            // int wordIx = 0;
            // for (size_t ix = lookupList.hashInfo[maxEntryHashGroup].startIx; ix <= lookupList.hashInfo[maxEntryHashGroup].endIx; ix++)
            // {
            //     samson::system::String string;
            //     string->parse(lookupList.head[ix].keyP);
            // 
            //     LM_T(LmtHash, ("HG %d, Word %02d: '%s'", maxEntryHashGroup, wordIx, string->value.c_str()));
            //     ++wordIx;
            // }

            LM_T(LmtRest, ("lookup list created"));
            // semGive();
        }


        BlockLookupList* Block::lookupListGet(void)
        {
            return &lookupList;
        }


        std::string Block::lookup(const char* key)
        {
            int            hashGroup;
            int            keySize;
            int            testKeySize;
            char           keyName[1024];
            KVFormat       kvFormat            = getKVFormat();
            Data*          keyData             = ModulesManager::shared()->getData(kvFormat.keyFormat);
            DataInstance*  keyDataInstance     = (DataInstance*) keyData->getInstance();
            int            compare;

            LM_T(LmtRest, ("looking up key '%s'", key));
            keyDataInstance->setFromString(key);

            keySize      = keyDataInstance->serialize(keyName);
            hashGroup    = keyDataInstance->hash(KVFILE_NUM_HASHGROUPS);

            int startIx  = lookupList.hashInfo[hashGroup].startIx;
            int endIx    = lookupList.hashInfo[hashGroup].endIx;
            int testIx   = (endIx - startIx) / 2 + startIx;

            while (true)
            {
                LM_T(LmtRest, ("looking up key '%s' - comparing with ix %d (from ix %d to %d)", key, testIx, startIx, endIx));
                compare = keyDataInstance->serial_compare(keyName, lookupList.head[testIx].keyP);

                if (compare == 0)
                {
                    testKeySize = keyDataInstance->parse(lookupList.head[testIx].keyP);

                    Data*          valueData          = ModulesManager::shared()->getData(kvFormat.valueFormat);
                    char*          valueP             = (char*) ((size_t) lookupList.head[testIx].keyP + testKeySize);
                    DataInstance*  valueDataInstance  = (DataInstance*) valueData->getInstance();

                    valueDataInstance->parse(valueP);

                    return keyDataInstance->strXMLInternal("key") + valueDataInstance->strXMLInternal("value");
                }

                if (compare < 0) // keyName < testKey => Go to the left - to 'smaller' key names
                    endIx = testIx - 1;
                else
                    startIx = testIx + 1;

                testIx = (endIx - startIx) / 2 + startIx;

                if (startIx > endIx) // Not found
                    return NULL;
            }
        }
    }
}
