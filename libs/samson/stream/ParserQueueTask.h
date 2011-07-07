

#ifndef _H_PARSER_QUEUE_TASK
#define _H_PARSER_QUEUE_TASK

#include "samson/stream/StreamProcessBase.h"        // parent class 
#include "samson/stream/QueueTask.h"                // parent class 

namespace samson
{
    
    namespace stream
    {
        // Parser QueueTask ( at the same time is the ProcessItem in the engine library to be executed )
        
        class ParserQueueTask : public stream::QueueTask 
        {
            
        public:
            
            ParserQueueTask( size_t id , std::string queue_name , network::StreamQueue * streamQueue  ) : stream::QueueTask(id,  queue_name , streamQueue )
            {
                operation_name = "stream:" + streamQueue->operation();
            }
            
            ~ParserQueueTask()
            {
            }

            // Get the required blocks to process
            void getBlocks( BlockMatrix *matrix );
            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );

            // Get a string with the status of this task
            virtual std::string getStatus();

            
            StreamProcessBase* getStreamProcess()
            {
                return this;
            }
            
        };
        

        // Parser QueueTask ( at the same time is the ProcessItem in the engine library to be executed )
        
        class MapQueueTask : public stream::QueueTask 
        {
            
        public:
            
            MapQueueTask( size_t id , std::string queue_name , network::StreamQueue * streamQueue  ) :stream::QueueTask(id,  queue_name , streamQueue )
            {
                operation_name = "stream:" + streamQueue->operation();
            }
            
            ~MapQueueTask()
            {
            }
            
            // Get the required blocks to process
            void getBlocks( BlockMatrix *matrix );
            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );
            
            // Get a string with the status of this task
            virtual std::string getStatus();
            
            
            
            StreamProcessBase* getStreamProcess()
            {
                return this;
            }
            
        };

        // Parser QueueTask ( at the same time is the ProcessItem in the engine library to be executed )
        
        class ReduceQueueTask : public stream::QueueTask 
        {
            int hg_begin;
            int hg_end;
            
        public:
            
            ReduceQueueTask( size_t id , std::string queue_name , network::StreamQueue * streamQueue  ) : stream::QueueTask(id,  queue_name , streamQueue )
            {
                operation_name = "stream:" + streamQueue->operation();
                hg_begin = -1;
                hg_end = -1;
            }
            
            ~ReduceQueueTask()
            {
            }
            
            void setHashGroups( int _hg_begin , int _hg_end )
            {
                hg_begin = _hg_begin;
                hg_end = _hg_end;
            }
                        
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );
            
            // Get a string with the status of this task
            virtual std::string getStatus();
            
            StreamProcessBase* getStreamProcess()
            {
                return this;
            }
            
        };        
        
        
        
        
    }    
}

#endif
