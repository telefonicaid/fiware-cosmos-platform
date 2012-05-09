

#include "au/CommandLine.h"            // samson::CommandLine

#include "engine/Buffer.h"            // engine::Buffer
#include "engine/Notification.h"

#include "samson/common/EnvironmentOperations.h"	// copyEnviroment()
#include "samson/common/NotificationMessages.h"     // notification_delilah_review_repeat_tasks

#include "samson/network/Packet.h"					// samson::Packet

#include "samson/delilah/Delilah.h"				// samson::Delilah
#include "samson/delilah/DelilahConsole.h"				// samson::DelilahConsole
#include "DelilahComponent.h"                   // Own interface


#include "RepeatDelilahComponent.h" // Own interface

namespace samson 
{
/*
 
    RepeatDelilahComponent::RepeatDelilahComponent( std::string _command , int _seconds ) 
    : DelilahComponent( DelilahComponent::repeat )
    {
        command = _command;
        seconds = _seconds;
        
        listen( notification_delilah_review_repeat_tasks );
        
        concept = au::str("Repeating %s",command.c_str());
    }
    
    std::string RepeatDelilahComponent::getStatus()
    {
        return au::str("Repeating %s",command.c_str());
    }
    
    
    void RepeatDelilahComponent::run()
    {
        // Run once here...
        DelilahConsole * delilah_console = (DelilahConsole *) delilah;
        delilah_console->run_repeat_command( command );
    }
    
    void RepeatDelilahComponent::notify( engine::Notification* notification )
    {
        // A response for a disk operation
        if( notification->isName( notification_delilah_review_repeat_tasks  ) )
        {
            if ( cronometer.diffTimeInSeconds() >= seconds )
            {
                if ( isComponentFinished() )
                    return;
                
                // Run once here...
                DelilahConsole * delilah_console = (DelilahConsole *) delilah;
                delilah_console->run_repeat_command( command );
                
                cronometer.reset();
            }
        }
    }
 
 */
 
}