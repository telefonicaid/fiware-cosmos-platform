

#include "DelilahComponent.h"		// Own interface
#include "Packet.h"					// ss::Packet
#include "Delilah.h"				// ss::Delilah
#include "EnvironmentOperations.h"	// copyEnviroment()
#include "CommandLine.h"            // ss::CommandLine


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
	
	void* DelilahUpdaterBackgroundThread( void* p )
	{
		((DelilahUpdater*)p)->run();
		return NULL;
	}
	
	
	void DelilahUpdater::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		// Process to update the local list of queues and operations
		std::ostringstream  txt;
		
		switch (msgCode) {
				
			case Message::CommandResponse:
			{
				
				if( packet->message->command_response().has_queue_list() )
				{
					// Copy the list of queues for auto-completion
					list_lock.lock();
					
					if( ql )
						delete ql;
					ql = new network::QueueList();
					ql->CopyFrom( packet->message->command_response().queue_list() );
					
					list_lock.unlock();
					
				}
				
				if( packet->message->command_response().has_operation_list() )
				{
					list_lock.lock();
					
					if( ol )
						delete ol;
					ol = new network::OperationList();
					ol->CopyFrom( packet->message->command_response().operation_list() );
					
					list_lock.unlock();
				}

                // Update of the worker status
				if( packet->message->command_response().has_worker_status_list() )
				{
					list_lock.lock();
					
					if( wl )
						delete wl;
					wl = new network::WorkerStatusList();
					wl->CopyFrom( packet->message->command_response().worker_status_list() );
					
					list_lock.unlock();
				}

                // Update of the controller status
				if( packet->message->command_response().has_controller_status() )
				{
					list_lock.lock();
					
					if( cs )
						delete cs;
					cs = new network::ControllerStatus();
					cs->CopyFrom( packet->message->command_response().controller_status() );
					
					list_lock.unlock();
				}
				
                
			}
				break;
				
			default:
				break;
		}
		
	}
	
	void DelilahUpdater::run()
	{
		while (true) {
			
			sleep(2);
			
			// Send a message to the controller to update the local list of operations and queues

			if( delilah->network->isConnected( delilah->network->controllerGetIdentifier()  ) )
			{
			
				{
				// Message to update the local list of queues
				Packet*           p = new Packet();
				network::Command* c = p->message->mutable_command();
				c->set_command( "ls" );
				p->message->set_delilah_id( id );
				//copyEnviroment( &environment , c->mutable_environment() );
				delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Command, p);
				}
				
				{
				// Message to update the local list of operations
				Packet*           p = new Packet();
				network::Command* c = p->message->mutable_command();
				c->set_command( "o" );
				p->message->set_delilah_id( id );
				//copyEnviroment( &environment , c->mutable_environment() );
				delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Command, p);
				}	
                
				{
                    // Message to update the worker status list
                    Packet*           p = new Packet();
                    network::Command* c = p->message->mutable_command();
                    c->set_command( "w" );
                    p->message->set_delilah_id( id );
                    //copyEnviroment( &environment , c->mutable_environment() );
                    delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Command, p);
				}	
                
			}
			
		}
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
		Packet*           p = new Packet();
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
		delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Command, p);
		
	}		
	
	std::string CommandDelilahComponent::getStatus()
	{
		std::ostringstream o;
		o << "Command Component: " << command;
		return o.str();
	}
	
	
}

