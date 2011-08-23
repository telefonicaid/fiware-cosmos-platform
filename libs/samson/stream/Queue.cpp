
#include <sstream>       

#include "QueuesManager.h"              // samson::stream::QueuesManager
#include "Block.h"                      // samson::stream::Block
#include "BlockManager.h"               // samson::stream::BlockManager

#include "engine/ProcessManager.h"      // engine::ProcessManager
#include "engine/Notification.h"        // engine::Notification

#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/common/Info.h"                     // samson::Info

#include "samson/module/ModulesManager.h"           
#include "QueueTask.h"
#include "samson/network/NetworkInterface.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/stream/ParserQueueTask.h"          // samson::stream::ParserQueueTask

#include "PopQueue.h"       // samson::stream::PopQueue

#include "Queue.h"          // Own interface


namespace samson {
    namespace stream
    {
        
        
#pragma Queue
        
        Queue::Queue( std::string _name , QueuesManager * _qm ) 
        {
            name = _name;
            qm = _qm;
                        
            list = new BlockList( au::Format::string( "Queue %s" , name.c_str() ) );
        }
        
        Queue::~Queue()
        {
            delete list;
            
        }
                
        // Print the status of a particular queue
        
        std::string Queue::getStatus()
        {
            
            std::ostringstream output;
            
            output << "Queue " << name << "\n";            
            output << au::Format::indent( list->str() ) << "\n";
            
            return output.str();
        }
        
        void Queue::getInfo( std::ostringstream& output)
        {
            
            output << "<queue name=\"" << name << "\">\n"; 

            output << "<name>" << name << "</name>\n";
            
            // Put the matrix information here
            list->getInfo(output);
            
            output << "</queue>\n";
            
        }
  
        
        
    }
}