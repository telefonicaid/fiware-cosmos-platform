
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
                        
            list = new BlockList( au::str( "Queue %s" , name.c_str() ) );
        }
        
        Queue::~Queue()
        {
            delete list;
        }
        
        void Queue::copyFrom( BlockList* _list )
        {
            // Report command to worker "log"
            std::ostringstream log_command;
            
            log_command << "push " << name << " ";
            for ( au::list<Block>::iterator b = _list->blocks.begin() ; b!= _list->blocks.end() ; b++)
                log_command << (*b)->getId();
            qm->worker->dataManager->simple_log( log_command.str() );

            // Copy the modules properly
            list->copyFrom( _list );
        }
        
        void Queue::extractTo( BlockList* output_list , size_t max_size )
        {
            // Report command to worker "log"
            std::ostringstream log_command;
            
            BlockList _list("tmp extractTo");
            list->extractTo( &_list , max_size );

            // Report the blocks we are extracting
            log_command << "pop " << name << " ";
            for ( au::list<Block>::iterator b = _list.blocks.begin() ; b!= _list.blocks.end() ; b++)
                log_command << (*b)->getId();
            qm->worker->dataManager->simple_log( log_command.str() );
            
            // Copy the blocks we have just extracted
            output_list->copyFrom( &_list );
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