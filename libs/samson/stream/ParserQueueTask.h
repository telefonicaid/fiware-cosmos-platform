

#ifndef _H_PARSER_QUEUE_TASK
#define _H_PARSER_QUEUE_TASK

#include "samson/stream/StreamProcessBase.h"        // parent class 
#include "samson/stream/QueueTask.h"                // parent class 

namespace samson
{
    
    namespace stream
    {
        // Parser QueueTask ( at the same time is the ProcessItem in the engine library to be executed )
        
        class ParserQueueTask : public stream::QueueTask , public StreamProcessBase
        {
            
        public:
            
            ParserQueueTask( network::StreamQueue * streamQueue  ) : StreamProcessBase( key_value , streamQueue )
            {
                operation_name = "stream:" + streamQueue->operation();
            }
            
            ~ParserQueueTask()
            {
                LM_M(("Destroying ParserQueueTask"));
            }
            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );
            
            
            virtual std::string getStatus()
            {
                std::ostringstream output;
                output << "[" << id << "] ";
                output << "Parser " << streamQueue->operation() << " processing " << blocks.size() << " blocks with " << au::Format::string( getSize( blocks ) );
                return output.str();
            }
            
            
            StreamProcessBase* getStreamProcess()
            {
                return this;
            }
            
        };
        

        // Parser QueueTask ( at the same time is the ProcessItem in the engine library to be executed )
        
        class MapQueueTask : public stream::QueueTask , public StreamProcessBase
        {
            
        public:
            
            MapQueueTask( network::StreamQueue * streamQueue  ) : StreamProcessBase( key_value , streamQueue )
            {
                operation_name = "stream:" + streamQueue->operation();
            }
            
            ~MapQueueTask()
            {
                LM_M(("Destroying ParserQueueTask"));
            }
            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );
            
            
            virtual std::string getStatus()
            {
                std::ostringstream output;
                output << "[" << id << "] ";
                output << "Map " << streamQueue->operation() << " processing " << blocks.size() << " blocks with " << au::Format::string( getSize( blocks ) );
                return output.str();
            }
            
            
            StreamProcessBase* getStreamProcess()
            {
                return this;
            }
            
        };
        
        
        
        
    }    
}

#endif
