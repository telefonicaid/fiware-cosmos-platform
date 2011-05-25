/* ****************************************************************************
*
* FILE                     MemoryManager.cpp
*
* AUTHOR                   Andreu Urruela
*
* CREATION DATE            2010
*
*/

#include "logMsg/logMsg.h"                   // LM_*
#include "engine/Buffer.h"		              // samson::Buffer
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
    
    
    
    void MemoryManager::init( size_t _memory )
    {
        if ( memoryManager )
            LM_X(1, ("Please, init Memory manager only once"));
        
        memoryManager =  new MemoryManager( _memory );
                
    }

    void MemoryManager::destroy( )
    {
        LM_M(("MemoryManager terminating"));
        
        if( memoryManager )
        {
            delete memoryManager;
            memoryManager = NULL;
        }
    }
    
    MemoryManager* MemoryManager::shared()
    {
        if (!memoryManager )
            LM_X(1, ("Please, init Memory manager before using it"));
        
        return memoryManager;
    }
    
    
#pragma mark ------------------------------------------------------------------------
	
	MemoryManager::MemoryManager( size_t _memory )
	{
		
		// Total available memory
        memory = _memory;
        
		// Add the MemoryManager as a listner for the memory_request channel
        Engine::add( notification_memory_request, this );
	
    }
	
	MemoryManager::~MemoryManager()
	{
	}
	
	Buffer *MemoryManager::newBuffer( std::string name , size_t size , int tag )
	{
		token.retain();
		Buffer *b = _newBuffer( name , size , tag );
		token.release();
		
		return b;
	}

	Buffer *MemoryManager::_newBuffer( std::string name , size_t size , int tag )
	{
		
		Buffer *b = new Buffer( name, size, tag );
        
        // Insert in the temporal set of active buffers
        buffers.insert( b );
        
		return b;
	}	
	
	void MemoryManager::destroyBuffer(Buffer* b)
	{
		if (b == NULL)
			return;

		token.retain();
		
        // Remove from the temporal list of buffer
        buffers.erase( b );
        
		LM_T(LmtMemory, ("Destroying buffer with max size %sbytes", au::Format::string( b->getMaxSize() ).c_str() ) );
				
		
		b->free();
		delete b;
		
		token.release();

		// Check requests to schedule new notifications
		checkMemoryRequests();
		
		// Check process halted in the Engine
        Engine::add( new Notification( notification_process_manager_check_background_process ) );
		
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
		
        double memory_input_usage = getMemoryUsageByTag( 0 );
        
		token.retain();
		
		while( true )
		{

			MemoryRequest *r = NULL;
				
			if (  memory_input_usage < 0.5 )  // Maximum usage for input ( tag == 0) 50% of memory
				r = memoryRequests.extractFront();

			if( !r )
			{
				// Nothing to notify
				break;
			}
			else
			{
				Buffer *buffer = _newBuffer("Buffer from request", r->size , 0 );   // By default ( tag == 0 )
                Notification *notification  = new Notification( notification_memory_request_response , buffer  );
                notification->environment.copyFrom( r );
                Engine::add(notification);
                
                delete r; // Remove the memory request ( only used internally )
			}
			
		}
		
		token.release();
		
		LM_T( LmtMemory , ("[DONE] Checking memory requests Pending requests %u" , memoryRequests.size() ));
		
	}	 
    
    
    size_t MemoryManager::getMemory()
    {
        return memory;
    }

	 
    int MemoryManager::getNumBuffers()
    {
        
        token.retain();
        size_t num = buffers.size();
        token.release();
        
        return num;
        
    }

    size_t MemoryManager::getUsedMemory()
    {
        size_t total = 0;
        token.retain();
        std::set<Buffer*>::iterator i;
        for ( i = buffers.begin() ; i != buffers.end() ; i++)
                total+= (*i)->getMaxSize();
        token.release();
        
        return total;
        
        
    }

    double MemoryManager::getMemoryUsage()
    {
        return (double) getUsedMemory() / (double) memory;
    }

    
    
    int MemoryManager::getNumBuffersByTag( int tag )
    {
        size_t num = 0;
        token.retain();
        std::set<Buffer*>::iterator i;
        for ( i = buffers.begin() ; i != buffers.end() ; i++)
            if( (*i)->tag == tag)
                num++;
        token.release();
        
        return num;

    }

    size_t MemoryManager::getUsedMemoryByTag( int tag )
    {
        size_t total = 0;
        token.retain();
        std::set<Buffer*>::iterator i;
        for ( i = buffers.begin() ; i != buffers.end() ; i++)
            if( (*i)->tag == tag)
                total+= (*i)->getMaxSize();
        token.release();
        
        return total;
        
    }

    size_t MemoryManager::getMemoryUsageByTag( int tag )
    {
        return (double) getUsedMemoryByTag(tag) / (double) memory;
    }
   

    

    
	

}
