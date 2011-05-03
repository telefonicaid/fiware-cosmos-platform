/* ****************************************************************************
*
* FILE                     MemoryManager.cpp
*
* AUTHOR                   Andreu Urruela
*
* CREATION DATE            2010
*
*/

#include "logMsg.h"                   // LM_*
#include "engine/Buffer.h"		              // ss::Buffer
#include <sstream>		              // std::stringstream

#include "engine/MemoryManager.h"	          // Own interface
#include "engine/MemoryRequest.h"	          // engine::MemoryRequest
#include "engine/Engine.h"		              // engine::Engine
#include "engine/DiskManager.h"               // enigne::DiskManager
#include "engine/ProcessManager.h"            // engine::ProcessManager

namespace engine
{
        
#pragma mark ------------------------------------------------------------------------
    
    MemoryManager* memoryManager = NULL;
    
    
    void destroy_memory_manager()
    {
        LM_M(("MemoryManager terminating"));
        
        if( memoryManager )
        {
            delete memoryManager;
            memoryManager = NULL;
        }
    }
    
    void MemoryManager::init( size_t _memory )
    {
        if ( memoryManager )
            LM_X(1, ("Please, init Memory manager only once"));
        
        memoryManager =  new MemoryManager( _memory );
        
        //atexit( destroy_memory_manager );
        
    }

    MemoryManager* MemoryManager::shared()
    {
        if (!memoryManager )
            LM_X(1, ("Please, init Memory manager before using it"));
        
        return memoryManager;
    }
    
    size_t MemoryManager::getMemory()
    {
        if( memoryManager )
            return  memoryManager->memory;
        else
            return 0;
    }
    
    size_t MemoryManager::getUsedMemory()
    {
        if( memoryManager )
            return  memoryManager->used_memory_input +  memoryManager->used_memory_output;
        else
            return 0;
    }
    
    
    
    
#pragma mark ------------------------------------------------------------------------
	
	MemoryManager::MemoryManager( size_t _memory )
	{
		// Init usage counters
		used_memory_input=0;
		used_memory_output=0;
		
		num_buffers_input  = 0;
		num_buffers_output = 0;
		
		// Total available memory
        memory = _memory;
        
		// Add the MemoryManager as a listner for the memory_request channel
        Engine::add( notification_memory_request, this );
	
    }
	
	MemoryManager::~MemoryManager()
	{
	}
	
	Buffer *MemoryManager::newBuffer( std::string name , size_t size , Buffer::BufferType type )
	{
		token.retain();
		Buffer *b = _newBuffer( name , size , type );
		token.release();
		
		return b;
	}

	Buffer *MemoryManager::_newBuffer( std::string name , size_t size , Buffer::BufferType type )
	{
		switch (type) {
			case Buffer::input:
				used_memory_input += size;
				num_buffers_input++;
				break;
			case Buffer::output:
				used_memory_output += size;
				num_buffers_output++;
				break;
		}
		
		Buffer *b = new Buffer( name, size, type );
		return b;
	}	
	
	void MemoryManager::destroyBuffer(Buffer* b)
	{
		if (b == NULL)
			return;

		token.retain();
		
		LM_T(LmtMemory, ("Destroying buffer with max size %sbytes", au::Format::string( b->getMaxSize() ).c_str() ) );
		
		switch (b->getType()) {
			case Buffer::input:
				used_memory_input -= b->getMaxSize();
				num_buffers_input--;
				break;
			case Buffer::output:
				used_memory_output -= b->getMaxSize();
				num_buffers_output--;
				break;
				
		}
		

		LM_T(LmtMemory, ("[DONE] Destroying buffer with max size %sbytes", au::Format::string( b->getMaxSize() ).c_str() ) );
		
		
		b->free();
		delete b;
		
		token.release();

		// Check requests to schedule new notifications
		checkMemoryRequests();
		
		// Check process halted in the Engine
        Engine::add( new Notification( notification_process_manager_check_background_process ) );
		
	}
	
	

	// Fill information
    std::string MemoryManager::_str()
	{
				
		std::ostringstream output;

		output << "Input: " << ((int)(getMemoryUsageInput()*100.0)) << "% "; 
		output << "Output: " << ((int)(getMemoryUsageOutput()*100.0)) << "% "; 
		
		output << "[ Input: " << num_buffers_input << " buffers with " << au::Format::string( used_memory_input , "B" );
		output << " Output: " << num_buffers_output << " buffers with " << au::Format::string( used_memory_output , "B" ) << " ]";
		

        return output.str();
	}

    std::string MemoryManager::str()
    {
        if ( memoryManager )
            return memoryManager->_str();
        else
            return "Memory manager not initialized";
    }
    
    void MemoryManager::notify( Notification* notification )
    {
        if ( notification->isName(notification_memory_request) )
        {
                //LM_M(("Memory manager received a notification for memory"));
               
                MemoryRequest *request = new MemoryRequest( &notification->environment );
                
                if( request->size > memory )
                    LM_X(-1,("Error managing memory: excesive memory request"));	
                
                LM_T( LmtMemory , ("Adding memory request for %s" , au::Format::string( request->size , "B" ).c_str() ));
                
                token.retain();
                memoryRequests.push_back( request );
                token.release();
                
                LM_T( LmtMemory , ("[DONE] Adding memory request for %s" , au::Format::string( request->size , "B" ).c_str() ));
                
                checkMemoryRequests();
            
        } 
        else 
            LM_X(1,("Memory manager received a wrong notification"));
        
    }

    	
	 
	// Function to check memory requests and notify using Engine if necessary
	void MemoryManager::checkMemoryRequests()
	{
		LM_T( LmtMemory , ("Checking memory requests Pending requests %u" , memoryRequests.size() ));
		
		token.retain();
		
		while( true )
		{

			MemoryRequest *r = NULL;
				
			if ( getMemoryUsageInput() < 0.5 )  // Maximum usage for input 50% of memory
			{
				r = memoryRequests.extractFront();
			}

			if( !r )
			{
				// Nothing to notify
				break;
			}
			else
			{
				Buffer *buffer = _newBuffer("Buffer from request", r->size , Buffer::input);
                Notification *notification  = new Notification( notification_memory_request_response , buffer  );
                notification->environment.copyFrom( r );
                Engine::add(notification);
                
                delete r; // Remove the memory request ( only used internally )
			}
			
		}
		
		token.release();
		
		LM_T( LmtMemory , ("[DONE] Checking memory requests Pending requests %u" , memoryRequests.size() ));
		
	}	 
	 

	size_t MemoryManager::getUsedMemoryInput()
	{
		return used_memory_input;
	}

	size_t MemoryManager::getUsedMemoryOutput()
	{
		return used_memory_output;
	}
		
    double MemoryManager::getMemoryUsage()
    {
		return ( ((double) used_memory_output + (double) used_memory_input) / (double) memory );
    }

	double MemoryManager::getMemoryUsageInput()
	{
		return ( (double) used_memory_input / (double) memory );
	}
	
	double MemoryManager::getMemoryUsageOutput()
	{
		return ( (double) used_memory_output / (double) memory );
	}
	
	bool MemoryManager::availableMemoryOutput()
	{
		return ( getMemoryUsage() < 1.0 );
	}
	
	int MemoryManager::getNumBuffersInput()
	{
		return num_buffers_input;
	}
	
	int MemoryManager::getNumBuffersOutput()
	{
		return num_buffers_output;
	}
    
	

}
