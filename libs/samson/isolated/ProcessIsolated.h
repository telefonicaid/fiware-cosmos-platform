#ifndef _H_SAMSON_PROCESS_ISOLATED
#define _H_SAMSON_PROCESS_ISOLATED


#include <cstring>				// size_t		
#include <string>				// std::string
#include <sstream>				// std::ostringstream

#include "samson/common/samson.pb.h"			// samson::network::...
#include "samson/common/coding.h"				// samson::ProcessAssistantSharedFile
#include "samson/common/MemoryTags.h"           

#include "samson/module/Environment.h"              // samson::Enviroment

#include "samson/isolated/ProcessItemIsolated.h"	// samson:ProcessItemIsolated


#define WORKER_TASK_ITEM_CODE_FLUSH_BUFFER          1
#define WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH	2

namespace samson {
    
    
    class ProcessWriter;
    class ProcessTXTWriter;
    class Operation;
    class KVWriter;
    class TXTWriter;
    
    /*
     Base class for the elements that decides what to do with the output buffers of a ProcessIsolated
     */
    
    class ProcessIsolatedOutput
    {
        
    public:
        
        virtual void processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish )=0;
        virtual void processOutputTXTBuffer( engine::Buffer *buffer , bool finish )=0;
    };
    
    class ProcessIsolatedDataGenerator
    {
    public:
		virtual void generateKeyValues( KVWriter *writer ){};
		virtual void generateTXT( TXTWriter *writer ){};
    };
    
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
        
        int shm_id;                     // Shared memory area used in this operation
        engine::SharedMemoryItem *item; // Share memory item
        
        ProcessWriter *writer;          // Object used to emit key-values if type=key_value 
        ProcessTXTWriter *txtWriter;    // Object used to emit txt content if type=txt
        
    public:
        
        int num_outputs;    // Number of outputs
        int num_workers;    // Number of workers in the cluster
        
        // Auxiliar information to give correct format to output buffers
        std::vector<KVFormat> outputFormats;
        
    public:
        
        Environment operation_environment;                // Environment for this process
        
    public:
        
        ProcessIsolated( std::string description,  ProcessBaseType _type );
        virtual ~ProcessIsolated();

        // Function to specify if we are ready to be executed of continued from a halt
        bool isReady();
        
        // Get the writers to emit key-values
        ProcessWriter* getWriter();

        // Get writer to emit txt content
        ProcessTXTWriter* getTXTWriter();      
        
		// Flush the buffer ( front process ) in key-value and txt mode
		void flushBuffer( bool finish );
		void flushKVBuffer( bool finish );
		void flushTXTBuffer( bool finish );
		
		// Function executed at this process side when a code is sent from the background process
		void runCode( int c );

        // Methods overwritted from ProcessItemIsolated ( running on a different process )
        void runIsolated();
        
        // --------------------------------------------------------------------------
        // Methods implemented by subclases to generate content
        // Not pure virtual
        // --------------------------------------------------------------------------
		virtual void generateKeyValues( KVWriter *writer ){};
		virtual void generateTXT( TXTWriter *writer ){};
        
        // --------------------------------------------------------------------------
        // Methods implemented by subclases to do something with the output buffers
        // at front-process
        // --------------------------------------------------------------------------
        
        virtual void processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish )=0;
        virtual void processOutputTXTBuffer( engine::Buffer *buffer , bool finish )=0;
        
        // ---------------------------------------------------------------
        // SETUP ProcessIsolated
        // ---------------------------------------------------------------
 
        // Generic function to add outputs to this ProcesIsolated
        void addOutput( KVFormat format );
        void addOutputsForOperation( Operation *op );

        // Function to set the number of workers ( necessary )
        void setNumWorkers( int _num_workers );

        // Chage the type of usage
		void setProcessBaseMode(ProcessBaseType _type);
        
    };
    
}


#endif