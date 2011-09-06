


#include "au/CommandLine.h"            // samson::CommandLine

#include "engine/Buffer.h"            // engine::Buffer


#include "samson/common/EnvironmentOperations.h"	// copyEnviroment()

#include "samson/network/Packet.h"					// samson::Packet

#include "samson/delilah/Delilah.h"				// samson::Delilah
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
        
        component_finished = true;
        delilah->delilahComponentFinishNotification( this );
    }

    void DelilahComponent::setComponentFinishedWithError( std::string error_message )
    {
        // Only mark as finished once
        if( component_finished )
            return;

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
	
    
}

