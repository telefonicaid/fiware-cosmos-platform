


#include "au/CommandLine.h"            // samson::CommandLine

#include "engine/Buffer.h"            // engine::Buffer
#include "engine/Notification.h"

#include "samson/common/EnvironmentOperations.h"	// copyEnviroment()
#include "samson/common/NotificationMessages.h"     // notification_delilah_review_repeat_tasks

#include "samson/network/Packet.h"					// samson::Packet

#include "samson/delilah/Delilah.h"				// samson::Delilah
#include "samson/delilah/DelilahConsole.h"				// samson::DelilahConsole
#include "DelilahComponent.h"                   // Own interface


namespace samson {

	DelilahComponent::DelilahComponent( DelilaComponentType _type )
	{
		component_finished =  false;
		type = _type;
        
        cronometer.start();
        
        concept = "Unknown";
        progress = 0;
        
	}
	
	void DelilahComponent::setId( Delilah * _delilah ,  size_t _id )
	{
		delilah = _delilah;
		id = _id;
	}
	
    
    std::string DelilahComponent::getTypeName()
    {
        switch (type) {
            case command:               return "[ Command   ]";
            case load:                  return "[ Load      ]";
            case updater:               return "[ Updater   ]";
            case push:                  return "[ Push      ]";
            case pop:                   return "[ Pop       ]";
            case worker_command:        return "[ W Comamnd ]";
            case repeat:                return "[ Repeat    ]";
        }
        
        LM_X(1, ("Impossible error"));
        return "";
    }

    std::string DelilahComponent::getDescription()
    {
        std::ostringstream output;

        output << "[ " << id << " ]";
        // Andreu: Not really necessary to show the type of operation anymore
        // output << " " << getCodeName();
        
        if( error.isActivated() )
            output << " [ ERROR    ] ";
        else
        {
            
            if( component_finished )
                output << " [ FINISHED ] ";
            else
            {
                output << " [ RUNNING  ] ";
                output << "[ " << au::percentage_string( progress ) << " ] ";
                output << "[ " << cronometer.str() << " ] ";
            }
        }
        
        output << concept;
        return output.str();
        
    }
    
    std::string DelilahComponent::getIdAndConcept()
    {
        std::ostringstream output;
        output << "[ " << id << " ] " << concept;
        return output.str();        
    }

    
    bool DelilahComponent::isComponentFinished()
    {
        return component_finished;
    }
    
    void DelilahComponent::setComponentFinished()
    {
        // Only mark as finished once
        if( component_finished )
            return;
        
        cronometer.stop();
        
        //LM_M(("setComponentFinished for id:%lu", id));
        component_finished = true;
        delilah->delilahComponentFinishNotification( this );
    }
    
    void DelilahComponent::setComponentFinishedWithError( std::string error_message )
    {
        // Only mark as finished once
        if( component_finished )
        {
            //LM_M(("setComponentFinishedWithError for id:%lu with error message:'%s'; Component already finished", id, error_message.c_str()));
            return;
        }
        
        //LM_M(("setComponentFinishedWithError for id:%lu with error message:'%s'", id, error_message.c_str()));
        component_finished = true;
        
        error.set( error_message );
        
        delilah->delilahComponentFinishNotification( this );
    }
    
    void DelilahComponent::setConcept( std::string _concept )
    {
        concept = _concept;
    }
    
    void DelilahComponent::setProgress( double p )
    {
        progress = p;
    }
    
	
    RepeatDelilahComponent::RepeatDelilahComponent( std::string _command , int _seconds ) 
        : DelilahComponent( DelilahComponent::repeat )
    {
        command = _command;
        seconds = _seconds;
        
        listen( notification_delilah_review_repeat_tasks );
        listen( notification_delilah_stop_repeat_tasks );
        
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
        else if( notification->isName(notification_delilah_stop_repeat_tasks) )
        {
            setComponentFinished();
        }
    }
    
    
    
}

