#ifndef _H_EngineOperationsContainer
#define _H_EngineOperationsContainer

#include <set>                  // std::set

#include "au/Token.h"           // au::Token

#include "EngineNotification.h" // engine::NotificationListener

namespace engine{

    class DiskOperation;
    class ProcessItem;
    class MemoryRequest;
    
    class OperationsContainer
    {
        au::Token token;
        
        // List of currently schedules disk operations and processes

        std::set< engine::MemoryRequest* > memoryRequests;
        std::set< engine::DiskOperation* > diskOperations;
        std::set< engine::ProcessItem* > process_items;
        
    public:
        
        OperationsContainer();
        
        void add( engine::MemoryRequest* memoryRequest );
        void add( engine::DiskOperation *diskOperation );
        void add( engine::ProcessItem *process_item );

        void erase( engine::MemoryRequest* memoryRequest );
        void erase( engine::DiskOperation *diskOperation );
        void erase( engine::ProcessItem *process_item );
        
        // Cancel all pending operations
        // Utility function to send a notification to cancel all included operations
        void cancelEngineOperations();
        
        bool hasPendingEngineOperations();

        std::string getOperationsContainerStr();
        
        
    };
    
}

#endif