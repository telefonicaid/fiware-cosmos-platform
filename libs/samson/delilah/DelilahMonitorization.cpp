/* ****************************************************************************
 *
 * FILE                     DelilahConsole.h
 *
 * DESCRIPTION			   Console terminal for delilah
 *
 */

#include <ncurses.h>
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	
#include <iostream>
#include <iomanip>

#include "au/Format.h"						// au::Format
#include "au/CommandLine.h"				// au::CommandLine
#include "au/Info.h"                    // au::Info


#include "pugi/pugi.h"                      // pugi::Pugi

#include "engine/MemoryManager.h"				// samson::MemoryManager

#include "samson/common/EnvironmentOperations.h"						// Environment operations (CopyFrom)
#include "samson/common/Info.h"                             // samson::Info

#include "samson/network/Packet.h"						// ss:Packet

#include "samson/module/samsonVersion.h"		// SAMSON_VERSION
#include "samson/module/samsonVersion.h"

#include "samson/delilah/Delilah.h"					// samson::Delailh

#include "samson/stream/BlockManager.h"     // samson::stream::BlockManager
#include "DelilahUploadDataProcess.h"		// samson::DelilahUpLoadDataProcess
#include "DelilahDownloadDataProcess.h"		// samson::DelilahDownLoadDataProcess

#include "PushComponent.h"

#include "samson/delilah/DelilahUtils.h"        // getXXXInfo()

#include "DelilahMonitorization.h"				// Own interface


namespace samson
{	
    
    std::string DelilahMonitorization::getHeaderLeft()
    {
        switch (type) {
            case general:
                return "General information";
                break;
                
            case task:
                return "Tasks";
                break;
                
            case queues:
                return "Stream queues information";
                break;
                
            case queues_tasks:
                return "Stream queue tasks";
                break;
        }
        
        return "Unknown information";
        
    }
    
    std::string DelilahMonitorization::getHeaderRight()
    {
        std::ostringstream txt;
        txt << " ( Updated: " << getUpdateSeconds() <<  " )";
        return txt.str();
    }
    
    
    void DelilahMonitorization::evalComamnd( )
    {
        if( command == "ps_tasks" )
        {
            type = task;
            return;
        }
        
        if( command == "engine_show" )
        {
            type = general;
            return;
        }
        
        if( command == "ls_queues" )
        {
            type = queues;
            return;
        }
        
        if( command == "ps_stream" )
        {
            type = queues_tasks;
            return;
        }

        if( command == "quit" )
        {
            quitConsole = true;
            return;
        }
        
        
    }
    
    
    void DelilahMonitorization::printContent()
    {

        print("");
        
        switch (type) {
            case general:
                printLines( getStringInfo("/engine_system", getEngineSystemInfo, i_worker ) );
                break;
                
            case task:
                printLines( getStringInfo("/controller_task_manager//controller_task", getTaskInfo, i_controller ) ); 

                printLines( getStringInfo("/worker_task_manager//worker_task", getWorkerTaskInfo, i_worker  ) ); 
                break;
                
            case queues:
                printLines(getStringInfo("/stream_manager/queues/queue", getQueueInfo, i_worker | i_no_title ));
                break;
                
            case queues_tasks:
                printLines( getStringInfo("/stream_manager//queue_task", getQueueTaskInfo, i_worker ) );
                break;
                
        }

    }
    
       
    void DelilahMonitorization::auto_complete
        ( std::vector<std::string>& previous_words , std::string& current_word ,std::vector<std::string>& command_options )
    {
        // If no previous command, just main commands
        if( previous_words.size() == 0)
        {
            command_options.insert( command_options.end() , main_commands.begin() ,  main_commands.end() );
            return;
        }
    }

    
    
}
