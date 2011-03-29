/* ****************************************************************************
*
* FILE                     MemoryManager.cpp
*
* AUTHOR                   Andreu Urruela
*
* CREATION DATE            2010
*
*/

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "MemoryManager.h"		// Own interface
#include "Buffer.h"				// ss::Buffer
#include "SamsonSetup.h"		// ss:SamsonSetup
#include <sstream>				// std::stringstream
#include "MemoryRequest.h"		// ss::MemoryRequest
#include "EngineDelegates.h"	
#include "Engine.h"				// ss::Engine



namespace ss
{
	
	MemoryManager::MemoryManager()
	{
		// Init usage counters
		used_memory_input=0;
		used_memory_output=0;
		
		num_buffers_input  = 0;
		num_buffers_output = 0;
		
		// Total available memory
		memory = SamsonSetup::shared()->memory;

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
		Engine::shared()->checkBackgroundProcesses();
		
	}
	
	
	
	// Fill information
	void MemoryManager::fill(network::WorkerStatus*  ws)
	{
				
		std::ostringstream output;

		output << "Input: " << ((int)(getMemoryUsageInput()*100.0)) << "% "; 
		output << "Output: " << ((int)(getMemoryUsageOutput()*100.0)) << "% "; 
		
		output << "[ Input: " << num_buffers_input << " buffers with " << au::Format::string( used_memory_input , "B" );
		output << " Output: " << num_buffers_output << " buffers with " << au::Format::string( used_memory_output , "B" ) << " ]";
		
		ws->set_memory_status( output.str() );
		ws->set_total_memory( memory );
		ws->set_used_memory( getUsedMemory() );
	}
	
	void MemoryManager::addMemoryRequest( MemoryRequest *request)
	{
		
		if( request->size > memory )
			LM_X(-1,("Error managing memory: excesive memory request"));	

		LM_T( LmtMemory , ("Adding memory request for %s" , au::Format::string( request->size , "B" ).c_str() ));
		
		token.retain();
		memoryRequests.push_back( request );
		token.release();

		LM_T( LmtMemory , ("[DONE] Adding memory request for %s" , au::Format::string( request->size , "B" ).c_str() ));
		
		checkMemoryRequests();
	}
	
	 
	// Function to check memory requests and notify using Engine if necessary
	void MemoryManager::checkMemoryRequests()
	{
		LM_T( LmtMemory , ("Checking memory requests Pending requests %u" , memoryRequests.size() ));
		
		token.retain();
		
		while( true )
		{
			double p = getMemoryUsageInput();	// Only used for inputs

			MemoryRequest *r = NULL;
				
			if ( p < 1.0 )
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
				*(r->buffer) = _newBuffer("Buffer from request", r->size , Buffer::input);
				Engine::shared()->add( new MemoryRequestNotification( r ) );
			}
			
		}
		
		token.release();
		
		LM_T( LmtMemory , ("[DONE] Checking memory requests Pending requests %u" , memoryRequests.size() ));
		
	}	 
	 
	size_t MemoryManager::getUsedMemory()
	{
		return used_memory_input + used_memory_output;
	}

	size_t MemoryManager::getUsedMemoryInput()
	{
		return used_memory_input;
	}

	size_t MemoryManager::getUsedMemoryOutput()
	{
		return used_memory_output;
	}
	
	size_t MemoryManager::getMemory()
	{
		return memory;
	}	
	
	double MemoryManager::getMemoryUsageInput()
	{
		double per;
		if( memory == 0 )
			per = 0;
		else
			per =  2 * ( (double) used_memory_input / (double) memory );
		
		return per;
	}
	
	double MemoryManager::getMemoryUsageOutput()
	{
		double per;
		if( memory == 0 )
			per = 0;
		else
			per =  2 * ( (double) used_memory_output / (double) memory  );
		
		return per;
	}

	size_t MemoryManager::getMemoryOutput()
	{
		return ( memory )/2;
	}
	
	size_t MemoryManager::getMemoryInput()
	{
		return ( memory )/2;
	}
	
	
	bool MemoryManager::availableMemoryOutput()
	{
		return ( getMemoryUsageOutput() < 1.0 );
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
