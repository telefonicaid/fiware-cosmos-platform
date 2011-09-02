


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
	}
	
	void DelilahComponent::setId( Delilah * _delilah ,  size_t _id )
	{
		delilah = _delilah;
		id = _id;
	}
	
    
    std::string DelilahComponent::getCodeName()
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
        
        output << "[ " << id << " ] ";
        output << getCodeName();
        
        if( error.isActivated() )
            output << " [ ERROR    ] ";
        else
        {
            
            if( component_finished )
                output << " [ FINISHED ] ";
            else
                output << " [ RUNNING  ] ";
        }
        output << "[ " << cronometer.str() << " ] ";
        
        output << getShortStatus();
        
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
	
	CommandDelilahComponent::CommandDelilahComponent(std::string _command , engine::Buffer * _buffer) : DelilahComponent( DelilahComponent::command )
	{
		command = _command;
        buffer = _buffer;
	}
	
	void CommandDelilahComponent::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		if( msgCode == Message::CommandResponse )
		{
            if( packet->message->command_response().has_finish_command() && packet->message->command_response().finish_command() )
                setComponentFinished();
            
			if( packet->message->command_response().has_finish_job_id() || packet->message->command_response().has_error_job_id() )
            {
                if( packet->message->command_response().has_error_job_id() )
                    setComponentFinishedWithError( packet->message->command_response().error_message() );
                else
                    setComponentFinished();
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
    
	std::string CommandDelilahComponent::getShortStatus()
	{
		std::ostringstream o;
		o << "Command Component: " << command;
		return o.str();
	}
    
#pragma mark ----
	
	WorkerCommandDelilahComponent::WorkerCommandDelilahComponent(std::string _command , engine::Buffer * _buffer) 
        : DelilahComponent( DelilahComponent::worker_command )
	{
		command = _command;
        buffer = _buffer;
	}
	
	void WorkerCommandDelilahComponent::run()
	{
        num_workers = delilah->network->getNumWorkers();
        num_confirmed_workers = 0;
        
        
        for ( int worker = 0 ; worker < num_workers ; worker++ )
        {
            Packet*           p = new Packet( Message::WorkerCommand );
            network::WorkerCommand* c = p->message->mutable_worker_command();
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
            
            // Set the buffer data ( if any )
            p->buffer = buffer;
            
            delilah->network->sendToWorker( worker , p );            
            
            
        }
		
	}	    
    
	void WorkerCommandDelilahComponent::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		if( msgCode == Message::WorkerCommandResponse )
            num_confirmed_workers++;
        
        if( num_confirmed_workers == num_workers )
            setComponentFinished();
        
        
        if( packet->message->worker_command_response().has_error() )
            error.set( packet->message->worker_command_response().error().message()  );
	}
	
	
	std::string WorkerCommandDelilahComponent::getStatus()
	{
		std::ostringstream o;
		o << "Running '" << command << "' : Confirmed " << num_confirmed_workers << " / " << num_workers << " workers";
		return o.str();
	}
    
	std::string WorkerCommandDelilahComponent::getShortStatus()
	{
        return getStatus();
	}
	
	
}

