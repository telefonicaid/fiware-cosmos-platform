

#include "DelilahComponent.h"		// Own interface
#include "Packet.h"					// ss::Packet
#include "Delilah.h"				// ss::Delilah
#include "EnvironmentOperations.h"	// copyEnviroment()
#include "au/CommandLine.h"            // ss::CommandLine


namespace ss {

	DelilahComponent::DelilahComponent( DelilaComponentType _type )
	{
		component_finished =  false;
		type = _type;
	}
	
	void DelilahComponent::setId( Delilah * _delilah ,  size_t _id )
	{
		delilah = _delilah;
		id = _id;
	}
	
    
#pragma mark ----
    
    bool isOneOf( char c , std::string s )
	{
		for (size_t i = 0 ; i < s.size() ; i++)
			if( s[i] == c )
				return true;
		
		return false;
	}
    
    std::vector<std::string> simpleTockenize( std::string txt )
	{
		std::string tockens = " #\r\t\r\n{};\"";//All possible delimiters
		
		std::vector<std::string> items;
		
		// Simple parser
		size_t pos = 0;
		for (size_t i = 0 ; i < txt.size() ; i++)
		{
			if ( isOneOf( txt[i] , tockens ) )
			{
				if ( i > pos )
					items.push_back(txt.substr(pos, i - pos ));
				/*
				//Emit the literal with one letter if that is the case
				std::ostringstream o;
				o << txt[i];
				items.push_back( o.str() );
				*/
				pos = i+1;
			}
		}

        if ( txt.size() > pos )
            items.push_back(txt.substr(pos, txt.size() - pos ));
		
		
		return items;
	}
	
	
#pragma mark ----
	
	CommandDelilahComponent::CommandDelilahComponent(std::string _command) : DelilahComponent( DelilahComponent::command )
	{
		command = _command;
	}
	
	void CommandDelilahComponent::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		if( msgCode == Message::CommandResponse )
		{
            if( packet->message->command_response().has_finish_command() && packet->message->command_response().finish_command() )
                component_finished = true;
            
			if( packet->message->command_response().has_finish_job_id() || packet->message->command_response().has_error_job_id() )
				component_finished = true;	
		}
		
		// Always forward the message to delilah
		delilah->_receive( fromId , msgCode , packet );
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
        
        std::vector<std::string> words =  simpleTockenize( command );
        std::string current_param = "";
        std::string param_name;
        
        for ( int i = 0 ; i < (int)words.size() ; i++)
        {
            
            current_param = words[i];
            
            if( param_name != "" )
            {
                if( current_param.c_str()[0] == '-' )
                    e.set(param_name, "true" );
                else
                    e.set(param_name, current_param );

            }
            
            if( current_param.c_str()[0] == '-' )
                param_name = current_param.substr(1, current_param.length() - 1 );

            else
                param_name = "";
            
            
        }
        
		copyEnviroment( &e , c->mutable_environment() );
		delilah->network->send(delilah, delilah->network->controllerGetIdentifier() , p);
		
	}		
	
	std::string CommandDelilahComponent::getStatus()
	{
		std::ostringstream o;
		o << "Command Component: " << command;
		return o.str();
	}
	
	
}

