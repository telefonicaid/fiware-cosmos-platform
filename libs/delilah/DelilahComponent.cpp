

#include "DelilahComponent.h"		// Own interface
#include "Packet.h"					// ss::Packet
#include "Delilah.h"				// ss::Delilah
#include "EnvironmentOperations.h"	// copyEnviroment()

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
				
				if( packet->message.command_response().has_queue_list() )
				{
					// Copy the list of queues for auto-completion
					list_lock.lock();
					
					if( ql )
						delete ql;
					ql = new network::QueueList();
					ql->CopyFrom( packet->message.command_response().queue_list() );
					
					list_lock.unlock();
					
				}
				
				if( packet->message.command_response().has_operation_list() )
				{
					list_lock.lock();
					
					if( ol )
						delete ol;
					ol = new network::OperationList();
					ol->CopyFrom( packet->message.command_response().operation_list() );
					
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
				network::Command* c = p->message.mutable_command();
				c->set_command( "ls" );
				p->message.set_delilah_id( id );
				//copyEnviroment( &environment , c->mutable_environment() );
				delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Command, p);
				}
				
				{
				// Message to update the local list of operations
				Packet*           p = new Packet();
				network::Command* c = p->message.mutable_command();
				c->set_command( "o" );
				p->message.set_delilah_id( id );
				//copyEnviroment( &environment , c->mutable_environment() );
				delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Command, p);
				}	
			}
			
		}
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
			if( packet->message.command_response().has_finish_job_id() || packet->message.command_response().has_error_job_id() )
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
		network::Command* c = p->message.mutable_command();
		c->set_command( command );
		p->message.set_delilah_id( id );
		copyEnviroment( &delilah->environment , c->mutable_environment() );
		delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), Message::Command, p);
		
	}		
	
	std::string CommandDelilahComponent::getStatus()
	{
		std::ostringstream o;
		o << "Command Component: " << command;
		return o.str();
	}
	
	
}

