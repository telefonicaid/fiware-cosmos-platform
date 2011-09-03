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


#include "au/map.h"                         // au::simple_map

#include "engine/MemoryManager.h"

#include "samson/common/coding.h"

#include "engine/DiskOperation.h"       // engine::DiskOperation
#include "engine/Object.h"  // engien::EngineListener
#include "engine/Object.h"              // engine::Object

#include "samson/module/KVSetStruct.h"	// samson::KVSetStruct

#include <set>

namespace samson {
    namespace stream
    {
        
        class BlockList;
        
        /**
         Main class to hold a block on memory
         */
        
        class Block :  public engine::Object
        {
            
            friend class BlockManager;  
            friend class BlockList;
            friend class PopQueueTask;
            
            size_t id;                  // Identifier of the block ( in this node )
            
            engine::Buffer *buffer;     // Buffer of data if content of this block is on memory
            
            KVHeader* header;           // Always on memory copy of the header
            
            size_t size;                // Size of the buffer ( Not that buffer is NULL if content is not on memory )

            std::set< BlockList* > lists;   // List where this block is contained
            
            typedef enum
            {
                on_memory,      // Initial state
                writing,        // A write order has been given
                ready,          // On disk and memory
                on_disk,        // Just on disk ( memory has been freed )
                reading         // Reading from disk to recover at memory
            } BlockState;

            BlockState state;

            au::simple_map< KVRange , KVInfo > infos; // Map containing sevel divisions of information
            
        private:
            
            // Constructor only visible in a BlockList
            Block( engine::Buffer *buffer  );
            
        public:
            
            ~Block();
            
            // Check if this block is not in any list anymore
            int getNumberOfLists()
            {
                return lists.size();
            }

            int canBeRemoved()
            {
                if( lists.size() != 0)
                    return false;
                
                if( state == reading )
                    return false;
                
                if( state == writing )
                    return false;

                return true;
            }
            
            
            // Set and Get priority
            void setPriority( int _priority );
            int getPriority();
                        
        private:
            
            // Send notifications to write and read 
            void write();
            void read();
            
            // Release memory ( only if saved to disk previously )
            void freeBlock();

            bool compare( Block *b );
            
            // Get the operations
            ::engine::DiskOperation* getWriteOperation();
            ::engine::DiskOperation* getReadOperation();
            
            std::string getFileName();
            
            // Notifications
            void notify( engine::Notification* notification );
            
        public:
            
            bool isOnDisk()
            {
                return ( state == on_disk );
            }
            
            bool isWriting()
            {
                return ( state == writing );
            }
            
            bool isReading()
            {
                return ( state == reading );
            }
            
            bool isOnMemory()
            {
                return (state == on_memory);
            }
            
            bool isReady()
            {
                return (state == ready);
            }
            
            bool isContentOnMemory()
            {
                return (  (state == ready ) || (state == on_memory) || ( state == writing ));
            }
            
            bool isContentOnDisk()
            {
                return (  (state == ready ) || (state == on_disk) || ( state == reading ));
            }
            
            bool isLockedInMemory();
            
            size_t getSize()
            {
                return size;
            }
            
            char *getData()
            {
                if( ! isContentOnMemory() )
                    LM_X(1,("Not possible to get data for a block that is not in memory"));
                return buffer->getData();
            }
            
            
            size_t getSizeOnMemory()
            {
                if( isContentOnMemory() )
                    return size;
                else
                    return 0;
            }
            
            size_t getSizeOnDisk()
            {
                if( isContentOnDisk() )
                    return size;
                else
                    return 0;
            }
            
            bool isNecessaryForKVRange( KVRange range )
            {
                if( !header )
                    return true;
                
                return header->range.overlap( range );
            }
            
            // Get information
            KVInfo getKVInfo();
            KVInfo getKVInfo( KVRange r);
            
            // Debug string    
            std::string str();

            void getInfo( std::ostringstream& output);
            
            const char* getState();
            
            size_t getId()
            {
                return id;
            }
            
            
            
            // Info for KVRanges
            
            void computeKVInfoForRange( KVRange r )
            {
                //LM_M(("Computing info for range..%s" , r.str().c_str() ));
                
                if( !r.isValid() )
                {
                    LM_W(("Not possible to compute info for range %s sice the range is not valid" , r.str().c_str()));
                    return;
                }
                
                if( header->isTxt() )
                {
                    //LM_W(("Not possible to compute info for range sice the block is a txt block"));
                    // A warning is not necessary since it is part of the protocol for QueueItem
                    return;
                }
                
                // See if already comptued
                if( infos.isInMap(r) )
                    return;
                
                if( !isContentOnMemory() )
                {
                    LM_W(("Not possible to compute info for range %s sice the block is not in memory" , r.str().c_str()));
                    return;
                }
                
                KVInfo *info = (KVInfo *) ( buffer->getData() + sizeof( KVHeader ) );

                KVInfo total;
                for ( int i = r.hg_begin ; i < r.hg_end ; i++ )
                    total.append( info[i] );

                
                infos.insertInMap ( r , total );
                
                //LM_M(("Finish Computing info for range.."));
                
            }
            
            bool isKVInfoForRange( KVRange r )
            {
                return infos.isInMap(r);
            }
            
            KVInfo getKVInfoForRange( KVRange r )
            {
                if( !infos.isInMap(r) )
                    LM_X(1,("Internal error"));
                
                return infos.findInMap(r);
            }
            
        private:
            
            // Get the minimum task id to get the order of the blocks
            size_t getMinTaskId();
            
        };
            
        
        // Auxiliar functions
        size_t getSize( std::set<Block*> &blocks );
        
    }
}

#endif