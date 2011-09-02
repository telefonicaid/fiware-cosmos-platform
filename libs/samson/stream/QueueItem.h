

#include <string>           // std::string

#include "engine/Buffer.h"              // engine::Buffer

#include "samson/common/samson.pb.h"    // samson::network::....
#include "engine/Buffer.h"              // engine::Buffer


namespace samson {
    
    namespace stream
    {
        
        class BlockList;
        class Queue;
        class Block;
        class QueueTask;
        
        class QueueItem
        {
            
            friend class QueuesManager;
            friend class Queue;
            
            Queue * myQueue;            // Pointer to the queue this item belong
            
            BlockList* list;          // List of blocks contained in this QueueItem
            BlockList* future_list;   // List of blocks that will be contained in this QueueItem when the "working operation" finish

            bool working;               // Flag to indicate that this list is block by a loking operations ( typically reduce state )
            QueueTask *task;            // Task that is currently working on this item ( working = true )
            
            // Limit in the hash groups for this QueueItem
            KVRange range;
            
        public:
            
            QueueItem( Queue * _myQueue,  KVRange range );
            ~QueueItem();
            
            // Push data into this item
            void push( BlockList *list );

            // Push data into this item ( from the working operation that blocks this item )
            void push( QueueTask *task , engine::Buffer *buffer );
                        
            // Notify that the current task is finished
            void notifyFinishOperation( QueueTask* _task );
        
            // Get Information in XML format
            void getInfo( std::ostringstream& output);
            
            bool isReadyToRun();

            void setRunning( QueueTask* task );
            
            bool isWorking();

            FullKVInfo getFullKVInfo();
            
        private:
         
            void prepareBlockList( BlockList *list );
            void prepareBlock( Block *block );
            
            
        };
        
    }
}