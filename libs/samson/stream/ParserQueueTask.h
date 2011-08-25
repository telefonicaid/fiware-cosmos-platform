

#ifndef _H_PARSER_QUEUE_TASK
#define _H_PARSER_QUEUE_TASK

#include "samson/stream/StreamProcessBase.h"        // parent class 
#include "samson/stream/QueueTask.h"                // parent class 


namespace samson
{
    
    namespace stream
    {
        class StateItem;
        
        // Parser QueueTask ( at the same time is the ProcessItem in the engine library to be executed )
        
        class ParserQueueTask : public stream::QueueTask 
        {
            
        public:
            
            ParserQueueTask( size_t id  , const network::StreamOperation& streamOperation  ) : stream::QueueTask(id , streamOperation )
            {
                operation_name = "stream:" + streamOperation.operation();
            }
            
            ~ParserQueueTask()
            {
            }

            // Get the required blocks to process
            void getBlocks( BlockList * input );
            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );

            // Get a string with the status of this task
            virtual std::string getStatus();
            
        };

        // ParserOut task is used only in popQueue operations
        
        
        class ParserOutQueueTask : public stream::QueueTask 
        {
            
        public:
            
            ParserOutQueueTask( size_t id  , const network::StreamOperation& streamOperation  ) : stream::QueueTask(id , streamOperation )
            {
                operation_name = "stream:" + streamOperation.operation();
                
                // Change to txt mode ( not key-value )
                setProcessBaseMode(ProcessIsolated::txt);

            }
            
            ~ParserOutQueueTask()
            {
            }
            
            // Get the required blocks to process
            void getBlocks( BlockList *matrix );
            
            // Function to generate output key-values
            void generateTXT( TXTWriter *writer );
            
            // Get a string with the status of this task
            virtual std::string getStatus();
            
        };
        
        

        // Parser QueueTask ( at the same time is the ProcessItem in the engine library to be executed )
        
        class MapQueueTask : public stream::QueueTask 
        {
            
        public:
            
            MapQueueTask( size_t id , const network::StreamOperation& streamOperation  ) :stream::QueueTask(id , streamOperation )
            {
                operation_name = "stream:" + streamOperation.operation();
            }
            
            ~MapQueueTask()
            {
            }
            
            // Get the required blocks to process
            void getBlocks( BlockList *input );
            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );
            
            // Get a string with the status of this task
            virtual std::string getStatus();
            
        };

        
        class ReduceQueueTask : public stream::QueueTask 
        {
            
            StateItem *stateItem;       // Pointer to notify state vectors
            
            int hg_begin;               // Limits in the hash-group considered in this operation
            int hg_end;
            
        public:
            
            ReduceQueueTask( size_t id , const network::StreamOperation& streamOperation , StateItem *_stateItem , int _hg_begin , int _hg_end  );
            
            ~ReduceQueueTask()
            {
            }

            // Get the blocks for this operation
            void getBlocks( BlockList *input , BlockList *state );
            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );
            
            // Get a string with the status of this task
            virtual std::string getStatus();

            // Particular way to process data    
            void processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish );

            // Funcion executed when task is finished
            void finalize();
            
        };          
        
        
        
    }    
}

#endif

