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

#include "au/mutex/TokenTaker.h"                  // au::TokenTaker
#include "au/xml.h"         // au::xml...

#include "engine/Buffer.h"		              // samson::Buffer

#include <sstream>		              // std::stringstream

#include "engine/Notification.h"                // engine::Notification
#include "engine/MemoryRequest.h"	          // engine::MemoryRequest
#include "engine/Engine.h"		              // engine::Engine
#include "engine/DiskManager.h"               // enigne::DiskManager
#include "engine/ProcessManager.h"            // engine::ProcessManager

#include "engine/MemoryManager.h"	          // Own interface

NAMESPACE_BEGIN(engine)

#pragma mark ------------------------------------------------------------------------

//Initialise singleton instance pointer
MemoryManager* MemoryManager::memoryManager = NULL;


void MemoryManager::destroy( )
{
    LM_V(("MemoryManager destroy"));
    
    if ( !memoryManager )
        LM_E(("Please, init Memory manager first"));
    
    delete memoryManager;
    memoryManager = NULL;
}

void MemoryManager::init( size_t _memory )
{
    LM_VV(("MemoryManager init with %s" , au::str( _memory ).c_str() ));
    if ( memoryManager )
        LM_E(("Please, init Memory manager only once"));
    
    memoryManager =  new MemoryManager( _memory );
    
}

MemoryManager* MemoryManager::shared()
{
    if (!memoryManager )
        LM_E(("Please, init Memory manager before using it, calling memoryManager::init()"));
    
    return memoryManager;
}


#pragma mark ------------------------------------------------------------------------

MemoryManager::MemoryManager( size_t _memory ) : token("Memory Manager")
{
    // Total available memory
    memory = _memory;
    
    // Public values to be accessed without token
    public_max_memory = memory;
    public_used_memory = 0;
}

MemoryManager::~MemoryManager()
{
    memoryRequests.clearList(); // Remove pending requests
}

Buffer *MemoryManager::createBuffer( std::string name , std::string type , size_t size , double mem_limit  )
{
    au::Cronometer cronometer;
    int count = 0;
    while (true)
	{
        //LM_M(("Before new mutex  buffer:%s, size:%lu, tag:%d\n", name.c_str(), size, tag));
        
        
        if (count > 60 * 100)
        {
            LM_M(("Already waiting 60 seconds (count:%d) for a memory buffer. Let's give it", count));
            mem_limit = 0;
        }
        
        Buffer *b = NULL;
        {	   
            au::TokenTaker tk( &token );
            b = _newBuffer( name , type , size , mem_limit );
        }
        
        if (b)
        {
            return b;
        }
        else
        {
            if (count %100 == 0)
            {
                LM_M(("Allocation for new memory buffer with size:%lu for '%s' delayed since memory %s > %s ( delayed %s secs)"
                      , size
                      , name.c_str()
                      , au::str_percentage( _getMemoryUsage() ).c_str()
                      , au::str_percentage( mem_limit ).c_str()
                      , au::str_time( cronometer.diffTimeInSeconds() ).c_str()
                      ));
            }
            usleep (10000);
            count ++;
        }
	}
    
    LM_X(1, ("Internal error"));
    return NULL;
    
}

Buffer *MemoryManager::_newBuffer( std::string name , std::string type , size_t size , double mem_limit  )
{
    if ((mem_limit != 0) && (mem_limit < _getMemoryUsage()))
       	return NULL;
    
    Buffer *b = new Buffer( name, type , size );
    
    //LM_M(("Alloc buffer '%s' " , b->str().c_str() ));
    
    // Insert in the temporal set of active buffers
    buffers.insert( b );
    
    //LM_M(("New memory buffer:%s, size:%lu, tag:%d, memory:%lu\n", b->str().c_str(), size, tag, _getUsedMemory()));

    // Update public value
    public_used_memory += size;
    
    LM_T(LmtMemory, ("New memory buffer %s size:%lu, acum_memory:%lu\n"
                     , b->str().c_str(), size, public_used_memory));
    
    return b;
}	

void MemoryManager::destroyBuffer(Buffer* b)
{
	//LM_M(("Before delete mutex  buffer:%s\n", b->str().c_str()));
    au::TokenTaker tk( &token );
    
    //LM_M(("Dealloc buffer '%s' " , b->str().c_str() ));
    
    if (b == NULL)
        return;
    
    
    // Remove from the temporal list of buffer
    buffers.erase( b );
    
    LM_T(LmtMemory, ("Dealloc buffer with max size %sbytes", au::str( b->getMaxSize() ).c_str() ) );
    //LM_M(("destroying memory buffer:%s, memory:%lu\n", b->str().c_str(), _getUsedMemory()));
    
    public_used_memory -= b->getMaxSize();
    
    LM_T(LmtMemory, ("destroying memory buffer:%s, acum_memory:%lu\n", b->str().c_str(), public_used_memory));
    
    
    b->free();
    delete b;
    
    // Check requests to schedule new notifications
    _checkMemoryRequests();
    
}

void MemoryManager::add( MemoryRequest *request )
{
    au::TokenTaker tk( &token );
    
    if( request->size > memory )
        LM_X(-1,("Error managing memory: excessive memory request %s ( total memory %s)" , au::str(request->size).c_str()  , au::str(memory).c_str()  ));
    
    LM_T( LmtMemory , ("Adding memory request for %s" , au::str( request->size , "B" ).c_str() ));
    
    memoryRequests.push_back( request );
    
    LM_T( LmtMemory , ("[DONE] Adding memory request for %s" , au::str( request->size , "B" ).c_str() ));
    
    _checkMemoryRequests();
    
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
void MemoryManager::_checkMemoryRequests()
{
    // Only used privately... no need to protect
    LM_T( LmtMemory , ("Checking memory requests Pending requests %u" , memoryRequests.size() ));
    
    while( true )
    {
        
        MemoryRequest *r = memoryRequests.findFront();
        
        if( !r )
            return;
        
        LM_T(LmtMemory, ("Checking memory request with size %lu type percentage %f "  , r->size , r->mem_percentadge ));
        
        // Full memory request, it is granted if memory is bellow 100%
        double memory_usage = _getMemoryUsage();
        
        LM_T(LmtMemory, ("memory usage %f"  , memory_usage ));
        
        if( memory_usage < r->mem_percentadge )
        {
            // Extract the request properly
            r = memoryRequests.extractFront();
            if (!r )
                LM_X(1,("Internal error"));
            
            // Get the buffer
            r->buffer = _newBuffer("Buffer from general request", "request" ,  r->size , 0  );   // By default ( tag == 0 )
            
            // Send the answer with a notification
            Engine::shared()->notify( new Notification( notification_memory_request_response , r , r->listner_id ) );
            
        }
        else
            return;
        
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
    // Protection needed because will run across the list of buffers to compute memory usage
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

au::tables::Table MemoryManager::getTableOfBuffers()
{
    au::tables::Table table( "Type,left,different|Name,left,different|Size,f=uint64,sum" );
    
    au::TokenTaker tt(&token);
    std::set<Buffer*>::iterator it_buffers;
    for( it_buffers = buffers.begin() ; it_buffers != buffers.end() ; it_buffers++ )
    {
        Buffer* buffer = *it_buffers;
        au::StringVector values;
        
        values.push_back( buffer->getType() );
        values.push_back( buffer->getName() );
        values.push_back( au::str("%lu" , buffer->getSize() ) );
        
        table.addRow( values );
        
    }
    
    return table;
    
}


NAMESPACE_END

