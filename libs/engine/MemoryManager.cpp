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
#include "logMsg/traceLevels.h"               

#include "au/TokenTaker.h"                  // au::TokenTaker
#include "au/xml.h"         // au::xml...

#include "engine/Buffer.h"		              // samson::Buffer

#include <sstream>		              // std::stringstream

#include "engine/Notification.h"                // engine::Notification
#include "engine/MemoryRequest.h"	          // engine::MemoryRequest
#include "engine/Engine.h"		              // engine::Engine
#include "engine/DiskManager.h"               // enigne::DiskManager
#include "engine/ProcessManager.h"            // engine::ProcessManager

#include "engine/MemoryManager.h"	          // Own interface

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
	
	MemoryManager::MemoryManager( size_t _memory ) : token("Memory Manager")
	{
		// Total available memory
        memory = _memory;
	
    }
	
	MemoryManager::~MemoryManager()
	{
	}
	
	Buffer *MemoryManager::newBuffer( std::string name , size_t size , int tag )
	{
        au::TokenTaker tk( &token );

		Buffer *b = _newBuffer( name , size , tag );
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
        au::TokenTaker tk( &token );
        
        if (b == NULL)
            return;
        
        
        // Remove from the temporal list of buffer
        buffers.erase( b );
        
        LM_T(LmtMemory, ("Destroying buffer with max size %sbytes", au::str( b->getMaxSize() ).c_str() ) );
        
        
        b->free();
        delete b;
		
		// Check requests to schedule new notifications
		checkMemoryRequests();
		
		// Check process halted in the Engine
        Engine::shared()->notify( new Notification( notification_process_manager_check_background_process ) );
		
	}
    
    void MemoryManager::add( MemoryRequest *request )
    {
        au::TokenTaker tk( &token );
        
        if( request->size > memory )
            LM_X(-1,("Error managing memory: excesive memory request"));	
        
        LM_T( LmtMemory , ("Adding memory request for %s" , au::str( request->size , "B" ).c_str() ));
        
        memoryRequests.push_back( request );
        
        LM_T( LmtMemory , ("[DONE] Adding memory request for %s" , au::str( request->size , "B" ).c_str() ));
        
        checkMemoryRequests();
        
    }
    
    void MemoryManager::cancel( MemoryRequest *request )
    {
        au::TokenTaker tk( &token );
        
        
        if( memoryRequests.extractFromList( request ) )
        {
            // Get the buffer
            request->buffer = NULL;
            
            // Send the answer with a notification
            Engine::shared()->notify( new Notification( notification_memory_request_response , request , request->listner_id ) );
            
        }        
        
    }    
	 
	// Function to check memory requests and notify using Engine if necessary
	void MemoryManager::checkMemoryRequests()
	{
        // Only used privatelly... no need to protect
        
		LM_T( LmtMemory , ("Checking memory requests Pending requests %u" , memoryRequests.size() ));
		
        
		while( true )
		{
            
            MemoryRequest *r = memoryRequests.findFront();
            
            if( !r )
                return;

            //LM_M(("Cheking memory request with size %lu type percentadge %f "  , r->size , r->mem_percentadge ));
            
            if( r->mem_percentadge == 1.0 )
            {
                
                // Extract the request properly
                r = memoryRequests.extractFront();
                if (!r )
                    LM_X(1,("Internal error"));

                
                // Full memory request, it is granted if memory is bellow 100%
                double memory_usage = _getMemoryUsage();

                //LM_M(("memory usage %f"  , memory_usage ));
                
                if( memory_usage < 1.0 )
                {
                    // Get the buffer
                    r->buffer = _newBuffer("Buffer from general request", r->size , 0 );   // By default ( tag == 0 )
                    
                    // Send the answer with a notification
                    Engine::shared()->notify( new Notification( notification_memory_request_response , r , r->listner_id ) );
                    
                }
                else
                    return;
                
                //LM_M(("Finish Cheking memory request 100%"));

            }
            else
            {
                
                // Normal memory requests ( for classical batch processing )
                double memory_input_usage = _getMemoryUsageByTag( 0 );
                
                if (  memory_input_usage < 0.5 )  // Maximum usage for input ( tag == 0) 50% of memory
                    r = memoryRequests.extractFront();
                
                if( !r )
                {
                    //LM_X((1,"Internal error"));
                    return;
                }
                else
                {
                    // Get the buffer
                    r->buffer = _newBuffer("Buffer from request", r->size , 0 );   // By default ( tag == 0 )
                    
                    // Send the answer with a notification
                    Engine::shared()->notify( new Notification( notification_memory_request_response , r , r->listner_id ) );
                }
            }
			
		}
		
		LM_T( LmtMemory , ("[DONE] Checking memory requests Pending requests %u" , memoryRequests.size() ));
		
	}	 
    
    
    size_t MemoryManager::getMemory()
    {
        return memory;
    }

	 
    int MemoryManager::getNumBuffers()
    {
        au::TokenTaker tk( &token );
        
        return buffers.size();
    }

    size_t MemoryManager::getUsedMemory()
    {
        au::TokenTaker tk( &token );
        return _getUsedMemory();
        
    }
    
    size_t MemoryManager::_getUsedMemory()
    {
        size_t total = 0;

        std::set<Buffer*>::iterator i;
        for ( i = buffers.begin() ; i != buffers.end() ; i++)
                total+= (*i)->getMaxSize();

        return total;
        
        
    }

    double MemoryManager::getMemoryUsage()
    {
        return (double) getUsedMemory() / (double) memory;
    }
    
    double MemoryManager::_getMemoryUsage()
    {
        return (double) _getUsedMemory() / (double) memory;
    }

    
    
    int MemoryManager::getNumBuffersByTag( int tag )
    {
        au::TokenTaker tk( &token );

        return _getNumBuffersByTag(tag);
    }

    size_t MemoryManager::getUsedMemoryByTag( int tag )
    {
        au::TokenTaker tk( &token );
        return _getUsedMemoryByTag(tag);
    }
    
    double MemoryManager::getMemoryUsageByTag( int tag )
    {
        return (double) getUsedMemoryByTag(tag) / (double) memory;
    }

    
    int MemoryManager::_getNumBuffersByTag( int tag )
    {
        size_t num = 0;
        std::set<Buffer*>::iterator i;
        for ( i = buffers.begin() ; i != buffers.end() ; i++)
            if( (*i)->tag == tag)
                num++;
        
        return num;
        
    }
    
    size_t MemoryManager::_getUsedMemoryByTag( int tag )
    {
        size_t total = 0;
        std::set<Buffer*>::iterator i;
        for ( i = buffers.begin() ; i != buffers.end() ; i++)
            if( (*i)->tag == tag)
                total+= (*i)->getMaxSize();
        return total;
        
    }
    
    
    double MemoryManager::_getMemoryUsageByTag( int tag )
    {
        return (double) _getUsedMemoryByTag(tag) / (double) memory;
    }
   

    void MemoryManager::getInfo( std::ostringstream& output)
    {
        
        au::xml_open(output, "memory_manager");

        // Globla memory
        au::xml_simple(output, "memory", memory );

        
        // Used memory
        au::xml_simple( output , "used_memory" , getUsedMemory()  );
        au::xml_simple( output , "num_buffers" , getNumBuffers() );

        {
        au::TokenTaker tk( &token );

        // Memory request
        au::xml_iterate_list( output , "memory_requests" , memoryRequests);
        
        // Buffers information
        au::xml_iterate_list( output , "buffers" , buffers );
            
        }

        
        au::xml_close(output, "memory_manager");
        
    }
    

    
	

}
