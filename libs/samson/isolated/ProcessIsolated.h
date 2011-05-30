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
    
    /**
     
     ProcessIsolated is an isolated process that share 
     one or more shared-memory buffers between process and platform
     
     */
    
    class ProcessIsolated : public ProcessItemIsolated
    {
        
    public:
        
        int shm_id;// Shared memory area used in this operation
        engine::SharedMemoryItem *item;// Share memory item
        
        
        ProcessWriter *writer;
        ProcessTXTWriter *txtWriter;
        
    public:
        
        int num_outputs;// Number of outputs
        int num_workers;// Number of workers in the cluster
        
    public:
        
        ProcessIsolated( int _num_outputs , int _num_workers )
        {
            num_outputs = _num_outputs;
            num_workers = _num_workers;
            
            // By default we have no asignation of shared memory
            shm_id = -1;
            item = NULL;
        }
        
        ~ProcessIsolated()
        {
            if( shm_id != -1 )
                engine::SharedMemoryManager::shared()->releaseSharedMemoryArea( shm_id );
            
            if ( item )
                engine::SharedMemoryManager::shared()->freeSharedMemory( item );
        }
        
        // Function to specify if we are ready to be executed of continued from a halt
        bool isReady()
        {
            if( shm_id == -1 )
            {
                // Try to get a shared memory buffer to produce output
                shm_id = engine::SharedMemoryManager::shared()->retainSharedMemoryArea();
                if( shm_id != -1 )
                    item = engine::SharedMemoryManager::shared()->getSharedMemory( shm_id );
            }
            
            bool available_memory = true;
            
            engine::MemoryManager *mm = engine::MemoryManager::shared();
            double memory_output_network    = mm->getMemoryUsageByTag( MemoryOutputNetwork );
            double memory_output_disk       = mm->getMemoryUsageByTag( MemoryOutputDisk );
            
            if( (memory_output_network + memory_output_disk ) > 0.5 )
                available_memory = false;
            
            // Return true only if output memory is available and there is a shared memory buffer for me
            return  ( available_memory && item );
        }        
        
        // Get the writers to emit key-values
        ProcessWriter* getWriter()
        {
            if( ! writer )
                writer = new ProcessWriter( this );
            return writer;
        }
        
        ProcessTXTWriter* getTXTWriter()
        {
            if( ! txtWriter )
                txtWriter = new ProcessTXTWriter( this );
            return txtWriter;
        }        
        
    };
    
}


#endif
