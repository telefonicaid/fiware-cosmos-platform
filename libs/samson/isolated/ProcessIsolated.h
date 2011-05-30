#ifndef _H_SAMSON_PROCESS_ISOLATED
#define _H_SAMSON_PROCESS_ISOLATED


#include <cstring>				// size_t		
#include <string>				// std::string
#include <sstream>				// std::ostringstream
#include "samson/common/samson.pb.h"			// samson::network::...
#include "samson/common/coding.h"				// samson::ProcessAssistantSharedFile
#include "samson/isolated/ProcessItemIsolated.h"	// ss:ProcessItemIsolated
#include "samson/common/MemoryTags.h"           


#define WORKER_TASK_ITEM_CODE_FLUSH_BUFFER          1
#define WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH	2

namespace samson {
    
    
    class ProcessWriter;
    class ProcessTXTWriter;
    
    /**
     
     ProcessIsolated is an isolated process that share 
     one or more shared-memory buffers between process and platform
     
     */
    
    class ProcessIsolated : public ProcessItemIsolated
    {
        
    public:
        
        typedef enum
		{
			key_value,      // Emit key-values to multiple outputs / multiple workers
			txt             // Emit txt content using the entire buffer
		} ProcessBaseType;
		
		
		ProcessBaseType type;

    public:
        
        int shm_id;// Shared memory area used in this operation
        engine::SharedMemoryItem *item;// Share memory item
        
        
        ProcessWriter *writer;
        ProcessTXTWriter *txtWriter;
        
    public:
        
        int num_outputs;// Number of outputs
        int num_workers;// Number of workers in the cluster
        
    public:
        
        ProcessIsolated( ProcessBaseType _type , int _num_outputs , int _num_workers );
        ~ProcessIsolated();

        // Chage the type of usage
		void setProcessBaseMode(ProcessBaseType _type)
		{
			type = _type;
		}
        
        
        // Function to specify if we are ready to be executed of continued from a halt
        bool isReady();
        
        // Get the writers to emit key-values
        ProcessWriter* getWriter();
        
        ProcessTXTWriter* getTXTWriter();      
        
		// Flush the buffer ( front process ) in key-value and txt mode
		void flushBuffer( bool finish );
		void flushKVBuffer( bool finish );
		void flushTXTBuffer( bool finish );
		
		// Function executed at this process side when a code is sent from the background process
		void runCode( int c )
		{
			switch (c) {
				case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER:
					flushBuffer(false);	// Flush the generated buffer with new key-values
					return;
                    break;
				case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH:
					flushBuffer(true);	// Flush the generated buffer with new key-values
                    return;
                    break;
				default:
					error.set("System error: Unknown code in the isolated process communication");
					break;
			}
			
			return;
		}

        // Pure virtual methods to process output buffers of data
        virtual void processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish )=0;
        virtual void processOutputTXTBuffer( engine::Buffer *buffer , bool finish )=0;
        
        
        
    };
    
}


#endif
