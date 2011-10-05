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

#include "au/string.h"						// au::Format
#include "au/CommandLine.h"				// au::CommandLine

#include "pugi/pugi.h"                      // pugi::Pugi

#include "engine/MemoryManager.h"				// samson::MemoryManager

#include "samson/common/EnvironmentOperations.h"						// Environment operations (CopyFrom)
#include "samson/common/Info.h"                             // samson::Info

#include "samson/network/Packet.h"						// ss:Packet

#include "samson/module/samsonVersion.h"		// SAMSON_VERSION
#include "samson/module/samsonVersion.h"

#include "samson/delilah/Delilah.h"					// samson::Delailh
#include "samson/stream/BlockManager.h"     // samson::stream::BlockManager

#include "UploadDelilahComponent.h"		// samson::DelilahUploadComponent
#include "DownloadDelilahComponent.h"		// samson::DelilahDownloadComponent
#include "PushDelilahComponent.h"
#include "PopDelilahComponent.h"

#include "samson/delilah/DelilahUtils.h"        // getXXXInfo()

#include "DelilahMonitorization.h"				// Own interface


namespace samson
{	
    
    std::string DelilahMonitorization::getHeaderLeft()
    {
        return getCommand();
    }
    
    std::string DelilahMonitorization::getHeaderRight()
    {
        std::ostringstream txt;
        txt << " ( Updated: " << getUpdateSeconds() <<  " )";
        return txt.str();
    }
    
    
    void DelilahMonitorization::evalComamnd( )
    {
        // NOthing spetial to fo
        
    }
    
    
    void DelilahMonitorization::printContent()
    {
        std::string _command = getCommand();
        if( _command.length() > 0 )
            running_command = _command;

        print("");
        
        // Spetial command for monitorization
        if( running_command == "overview" )
        {
           printLines( getStringInfo("/engine_system", getEngineSimplifiedSystemInfo, i_worker ) );
            return;
        }

        // General command...samsonL
        printLines( info( running_command ) );
                
/*        
        switch (type) {
            case general:
                break;
                
            case task:
                printLines( getStringInfo("/controller_task_manager//controller_task", getTaskInfo, i_controller ) ); 

                printLines( getStringInfo("/worker_task_manager//worker_task", getWorkerTaskInfo, i_worker  ) ); 
                break;
                
            case queues:
                printLines(getStringInfo("/stream_manager/queues/queue", getQueueInfo, i_worker  ));
                break;
                
            case queues_tasks:
                printLines( getStringInfo("/stream_manager//queue_task", getQueueTaskInfo, i_worker ) );
                break;
                
        }
*/
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
