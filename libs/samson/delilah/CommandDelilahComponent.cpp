


#include "au/CommandLine.h"            // samson::CommandLine

#include "engine/Buffer.h"            // engine::Buffer


#include "samson/common/EnvironmentOperations.h"	// copyEnviroment()

#include "samson/network/Packet.h"					// samson::Packet

#include "samson/delilah/Delilah.h"				// samson::Delilah
#include "DelilahComponent.h"                   
#include "CommandDelilahComponent.h"            // Own interface

namespace samson {

	
	CommandDelilahComponent::CommandDelilahComponent(std::string _command , engine::Buffer * _buffer) : DelilahComponent( DelilahComponent::command )
	{
		command = _command;
        buffer = _buffer;
        
        setConcept(command);    // The command is the description itself
	}
	
	void CommandDelilahComponent::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		if( msgCode == Message::CommandResponse )
		{
            if( packet->message->command_response().has_finish_command() && packet->message->command_response().finish_command() )
		{
		LM_M(("command:'%s' fromId(%d) setComponentFinished() by has_finish_command()", command.c_str(), fromId));
                setComponentFinished();
		}
            
			if( packet->message->command_response().has_finish_job_id() || packet->message->command_response().has_error_job_id() )
            {
                if( packet->message->command_response().has_error_job_id() )
		{
		LM_M(("command:'%s' fromId(%d) setComponentFinishedWithError() by .has_error_job_id()", command.c_str(), fromId));
                    setComponentFinishedWithError( packet->message->command_response().error_message() );
		    }
                else
		{
		LM_M(("command:'%s' fromId(%d) setComponentFinished() by .has_finish_job_id()", command.c_str(), fromId));
                    setComponentFinished();
		    }
            }
		}
		
		// Always forward the message to delilah
		//delilah->_receive( fromId , msgCode , packet );
	}
	
	void CommandDelilahComponent::run()
	{
		// Send the message with the command
		// Send the packet to create a job
		Packet*           p = new Packet( Message::Command );
		network::Command* c = p->message->mutable_command();
		c->set_command( command );
		p->message->set_delilah_id( id );
        
        Environment e;
        e.copyFrom( &delilah->environment );
        
        
        // Tokenie the command line to add new environment parameters
        
        std::vector<std::string> words =  au::simpleTockenize( command );
        
        for ( int i = 0 ; i < (int)words.size() ; i++)
        {
            
            std::string word = words[i];

            // Check if it contants "="
            size_t pos = word.find('=');

            if ( pos != std::string::npos )
            {
                std::string param = word.substr( 0, pos );
                std::string value = word.substr( pos+1, word.length() - (pos+1) );

                if( (param.length()>0) && (value.length()>0) )
                {
                    e.set(param, value );
                    delilah->showWarningMessage( au::str( "Set environment variable %s = %s" , param.c_str() , value.c_str() ) );
                }
                
            }
            
        }
        
		copyEnviroment( &e , c->mutable_environment() );
        
        // Set the buffer data ( if any )
        p->buffer = buffer;
        
		delilah->network->sendToController( p );
		
	}		
	
	std::string CommandDelilahComponent::getStatus()
	{
		std::ostringstream o;
		o << "Command: '" << command << "'";
		return o.str();
	}
	
}

