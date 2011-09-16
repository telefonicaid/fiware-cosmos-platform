


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
            
            // Check if this command is finished
            if( packet->message->command_response().has_finish_command() )
            {
                
                if ( packet->message->command_response().has_error() )
                {
                    // Finish with an error
                    setComponentFinishedWithError( packet->message->command_response().error().message() );
                }
                else
                {
                    // Just finish without an error
                    setComponentFinished();
                }
            }
            
            // Show messages if any
			if( packet->message->command_response().has_message() )
            {
                delilah->showMessage( au::str("Message for process %lu: %s" , id , packet->message->command_response().message().c_str() ) ); 
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

