

#include "DiskOperation.h"
#include "ProcessItem.h"

#include "au/TokenTaker.h"                          // au::TokenTake

#include "Engine.h"                         // engine::Engine

#include "DiskManager.h"                    // notification_disk_operation_cancel
#include "ProcessManager.h"                 // notification_process_cancel

#include "OperationsContainer.h"      // Own interface


NAMESPACE_BEGIN(engine)


OperationsContainer::OperationsContainer() : token( "OperationsContainer" )
{
}

void OperationsContainer::add( engine::MemoryRequest *memoryRequest )
{
    au::TokenTaker tokenTaker( &token ); 
    memoryRequests.insert( memoryRequest );
}

void OperationsContainer::add( engine::DiskOperation *diskOperation )
{
    au::TokenTaker tokenTaker( &token ); 
    diskOperations.insert( diskOperation );
}

void OperationsContainer::add( engine::ProcessItem *process_item )
{
    au::TokenTaker tokenTaker( &token ); 
    process_items.insert( process_item );
}

void OperationsContainer::erase( engine::MemoryRequest *memoryRequest )
{
    au::TokenTaker tokenTaker( &token ); 
    memoryRequests.erase( memoryRequest );
}

void OperationsContainer::erase( engine::DiskOperation *diskOperation )
{
    au::TokenTaker tokenTaker( &token ); 
    diskOperations.erase( diskOperation );
}

void OperationsContainer::erase( engine::ProcessItem *process_item )
{
    au::TokenTaker tokenTaker( &token ); 
    process_items.erase( process_item );
}

// Cancel all pending operations
void OperationsContainer::cancelEngineOperations()
{
    au::TokenTaker tokenTaker( &token ); 
    
    // Send a notification to cancel each one of pending disk operations
    std::set< engine::MemoryRequest* >::iterator m;
    for ( m = memoryRequests.begin() ; m != memoryRequests.end() ; m++ )
    {
        //LM_M(("Canceling memory request"));
        engine::MemoryManager::shared()->cancel(*m);
    }
    
    // Send a notification to cancel each one of pending disk operations
    std::set< engine::DiskOperation* >::iterator d;
    for ( d = diskOperations.begin() ; d != diskOperations.end() ; d++ )
    {
        //LM_M(("Canceling disk operation"));
        engine::DiskManager::shared()->cancel(*d);
    }
    
    // Send a notification to cancel each one of the pending process items
    std::set< engine::ProcessItem* >::iterator p;
    for ( p = process_items.begin() ; p != process_items.end() ; p++ )
    {
        //LM_M(("Canceling process item"));
        engine::ProcessManager::shared()->cancel(*p);
    }
    
}

bool OperationsContainer::hasPendingEngineOperations()
{
    au::TokenTaker tokenTaker( &token ); 
    
    if( diskOperations.size() > 0 )
        return true;
    if( process_items.size() > 0 )
        return true;
    
    return false;
}

std::string OperationsContainer::getOperationsContainerStr()
{
    au::TokenTaker tokenTaker( &token ); 
    
    std::ostringstream output;
    output << "[ " << diskOperations.size() << " disk ops & " << process_items.size() << " item ops ]";
    return output.str();
}

NAMESPACE_END
