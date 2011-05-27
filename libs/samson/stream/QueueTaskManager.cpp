

#include "QueueTaskManager.h"   // Own interface
#include "QueueTask.h"          // samson::stream::QueueTask

#include <sstream>              // std::ostringstream

namespace samson {
    namespace stream {
        
        // Insert a queue in the system
        
        void QueueTaskManager::add( QueueTask* task )
        {
            task->id = id++;    // Set the id of this task
            queueTasks.push_back( task );
        }
        
        std::string QueueTaskManager::getStatus()
        {
            std::ostringstream output;
            
            au::list< QueueTask >::iterator i;
            for ( i = queueTasks.begin() ; i!= queueTasks.end() ; i++ )
            {
                output << (*i)->getStatus() << "\n";
            }
            
            return output.str();
        }

        
    }
    
}