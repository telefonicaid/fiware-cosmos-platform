#ifndef _H_BLOCK
#define _H_BLOCK

/* ****************************************************************************
 *
 * FILE                      Block.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * Basic unit of data for stream processing
 * A block is a unit of data possibly in memory with replication of disk
 *
 */
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
#include "engine/BufferContainer.h"
#include "engine/ProcessItem.h"

#include "samson/module/KVSetStruct.h"              // samson::KVSetStruct

#include "BlockInfo.h"                              // struct BlockInfo
#include "BlockLookupList.h"

#include <set>

namespace samson 
{
    namespace stream
    {
        
        class BlockList;
 
        class Block :  public engine::Object
        {
            
            friend class BlockManager;  
            friend class BlockList;
            friend class PopQueueTask;
            friend class BlockBreakQueueTask;
            friend class BlockReader;
            friend class Queue;
            friend class StreamOutQueueTask;
            friend class BlockLookupList;
            friend class BlockBuilder;
            
            // Identifiers of this block
            size_t worker_id;               // Identifier of the worker in the cluster
            size_t id;                      // Identifier of the block ( in this node )
            
            // Buffer ( if it is on memory )
            engine::BufferContainer buffer_container;  // Buffer of data if content of this block is on memory
            
            KVHeader* header;               // Always on memory copy of the header
            
            size_t size;                    // Size of the buffer ( Not that buffer is NULL if content is not on memory )

            std::set< BlockList* > lists;   // List where this block is contained
            
            // Lock up table to quick access to key-values from REST interface
            au::Token token_lookupList;
            BlockLookupList* lookupList;

            typedef enum
            {
                building,       // Creating from several buffers
                on_memory,      // Initial state
                writing,        // A write order has been given
                ready,          // On disk and memory
                on_disk,        // Just on disk ( memory has been freed )
                reading         // Reading from disk to recover at memory
            } BlockState;

            BlockState state;              // State disk-memory
            
            au::Cronometer cronometer;       // Creation time of this block
            
        public:
            
            // Internal information used in the sort
            size_t min_task;
            int max_priority;
            size_t compare_time;   // Time since last update ( the lower the better )
            size_t order;             // Order inside the queue ( in case of same compare_time )
            
        private:
            
            // Constructor only visible in a BlockList
            Block( engine::BufferListContainer *buffer_list_container  );
            Block( size_t _worker_id , size_t _id , size_t _size , KVHeader* _header );

        public:

            ~Block();

            // Update sort information
            void update_sort_information();
            
            // Lookup for a particular queue ( lookupList is created if necessary )
            std::string  lookup(const char* key, std::string outputFormat);

            // Set and Get priority ( manual ordering if blocks are not assigned to tasks )
            void setPriority( int _priority );
            int getPriority();
                        
            // Check if this block is not in any list anymore
            int getNumberOfLists();

            // Function to check if this block can be removed from block manager ( basically it is not contained anywhere )
            int canBeRemoved();
                
            // Get file name for this block
            std::string getFileName();
            
        private:
            
            // Send notifications to write and read 
            void write();
            void read();
            
            // Release memory ( only if saved to disk previously )
            void freeBlock();

            // Get the operations
            ::engine::DiskOperation* getWriteOperation();
            ::engine::DiskOperation* getReadOperation();
                        
            // Notifications
            void notify( engine::Notification* notification );
            
        public:

            // Checking each state
            bool isOnDisk();
            bool isWriting();
            bool isReading();
            bool isOnMemory();
            bool isReady();

            // Checking if content is on memory and/or on disk
            bool isContentOnMemory();
            bool isContentOnDisk();

            // Checking if content is locked ( contained in a lock - block list )
            bool isLockedInMemory();

            // Getting information
            size_t getSize();
            size_t getSizeOnMemory();
            size_t getSizeOnDisk();
            KVFormat getKVFormat();
            const char* getState();
            size_t getId();
 
            // Working with KVRanges
            KVRange getKVRange();
            bool isNecessaryForKVRange( KVRange range );
            
            // Get information about this block
            void update( BlockInfo &block_info );
            
            // Debug string    
            std::string str();

            // Xml version of the info
            void getInfo( std::ostringstream& output);

            // Collection-like monitorization information
            void fill( samson::network::CollectionRecord* record , Visualization* visualization );
            
            // Accessing information
            char *getData();
            
            KVHeader getHeader()
            {
                return *header;
            }
            
        };

        class BufferKVsetContainer
        {
            KVFile kv_file;                           // KVFile to analyse the KVInfo vector
            engine::BufferContainer buffer_container; // buffer container to retain the engine::Buffer
            
        public:
            
            BufferKVsetContainer( engine::Buffer* buffer ) : kv_file( buffer->getData() )
            {
                buffer_container.setBuffer(buffer);
            }
            
            char* getDataForHashGroup( int hg )
            {
                return kv_file.dataForHashGroup(hg);
            }

            size_t getOffsetForHashGroup( int hg )
            {
                return kv_file.getKVInfoForHashGroup(hg).size;
            }
            
        };
        
        class BlockBuilder : public engine::ProcessItem
        {
            engine::BufferListContainer buffer_list_container;
            Block* block;
            
            friend class Block; // Necessary to ise buffer_list_container
            
        public:
            
            BlockBuilder( Block* _block ) : engine::ProcessItem( 10 ) // Max priority
            {
                block = _block;
            }
            
            // engine::ProcessItem interface            
            void run()
            {
                
                // Get the final buffer pointer
                engine::Buffer* final_buffer = block->buffer_container.getBuffer();

                // Just accumulation of buffers
                final_buffer->skipWrite( sizeof(KVHeader) );

                if( block->getKVFormat().isTxt() )
                {
                    
                    while( buffer_list_container.getNumBuffers() > 0 )
                    {
                        engine::Buffer* buffer = buffer_list_container.front();
                        
                        
                        if( !final_buffer->write( buffer->getData()+sizeof(KVHeader) , buffer->getSize()-sizeof(KVHeader) ) )
                        {
                            LM_W(("Error creating a block: Writing %s into %s" 
                                  , buffer->str().c_str() 
                                  , final_buffer->str().c_str() ));
                            error.set( au::str( "Not possible to write a component buffer %s" 
                                               , au::str( buffer->getSize() ).c_str() ));
                            return;
                        }
                        
                        buffer_list_container.pop();
                    }
                    
                    
                }
                else
                {
                    // Vector of kv files to joint vectors
                    au::vector<BufferKVsetContainer> buffer_kv_sets;
                    
                    while( buffer_list_container.getNumBuffers() > 0 )
                    {
                        engine::Buffer* buffer = buffer_list_container.front();
                        buffer_kv_sets.push_back( new BufferKVsetContainer(buffer) );
                        buffer_list_container.pop();
                    }

                    
                    // Copy to the final one
                    for ( int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++ )
                    {
                        for ( size_t f = 0 ; f < buffer_kv_sets.size() ; f++ )
                        {
                            size_t size = buffer_kv_sets[f]->getOffsetForHashGroup(hg);
                            char * data = buffer_kv_sets[f]->getDataForHashGroup(hg);
                            
                            if( !final_buffer->write(data, size) )
                            {
                                LM_W(("Error creating a block: Writing %s into %s" 
                                      , au::str(size).c_str() 
                                      , final_buffer->str().c_str() ));
                                error.set( au::str( "Not possible to write %s to buffer %s" 
                                                   , au::str( size ).c_str() , final_buffer->str().c_str() ));
                                return;
                                
                            }
                        }
                    }
                    
                    
                    // Remove all used buffers
                    buffer_kv_sets.clearVector();
                }
                
                // The block has been build, so now it is on memory... 
                block->state = Block::on_memory;
                
                if( final_buffer->getSize() != final_buffer->getMaxSize() )
                    error.set( au::str( "Error creating a blcok (Size:%s != MaxSize:%s)" 
                                       , au::str( final_buffer->getSize() ).c_str() 
                                       , au::str( final_buffer->getMaxSize() ).c_str() 
                                       ));
                
            }
            
            
        };        
        
        // Auxiliar functions
        size_t getSize( std::set<Block*> &blocks );
        
    }
}

#endif
