

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
                setProcessItemOperationName( "stream:" + streamOperation.operation() );
            }
            
            ~ParserQueueTask()
            {
            }

            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );

            // Get a string with the status of this task
            virtual std::string getStatus();
            
        };

        // ParserOut task is used only in popQueue operations
        
        
        class ParserOutQueueTask : public stream::QueueTask 
        {
            KVRange range;
            
        public:
            
            ParserOutQueueTask( size_t id  , const network::StreamOperation& streamOperation , KVRange _range  ) : stream::QueueTask(id , streamOperation )
            {
                // Set operation name for debugging
                setProcessItemOperationName( "stream:" + streamOperation.operation() );

                // Set the limits
                range = _range;
                
                // Change to txt mode ( not key-value )
                setProcessBaseMode(ProcessIsolated::txt);

            }
                        
            // Function to generate output key-values
            void generateTXT( TXTWriter *writer );
            
            // Get a string with the status of this task
            virtual std::string getStatus();
            
        };
        
        

        // Parser QueueTask ( at the same time is the ProcessItem in the engine library to be executed )
        
        class MapQueueTask : public stream::QueueTask 
        {
            KVRange range;
            
        public:
            
            MapQueueTask( size_t id , const network::StreamOperation& streamOperation , KVRange _range  ) :stream::QueueTask(id , streamOperation )
            {
                // Set operation name for debugging
                setProcessItemOperationName( "stream:" + streamOperation.operation() );
                
                // Set the limits
                range = _range;
            }
            
            ~MapQueueTask()
            {
            }
            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );
            
            // Get a string with the status of this task
            virtual std::string getStatus();
            
        };

        
        class ReduceQueueTask : public stream::QueueTask 
        {
            
            QueueItem *queueItem;       // Pointer to notify state vectors
            
            KVRange range;              // Range of hash-groups
            
        public:
            
            ReduceQueueTask( size_t id , const network::StreamOperation& streamOperation , QueueItem * _queueItem , KVRange range  );
            
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

