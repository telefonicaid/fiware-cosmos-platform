


#include "au/CommandLine.h"            // samson::CommandLine

#include "engine/Buffer.h"            // engine::Buffer


#include "samson/common/EnvironmentOperations.h"	// copyEnviroment()

#include "samson/network/Packet.h"					// samson::Packet

#include "samson/delilah/Delilah.h"				// samson::Delilah
#include "DelilahComponent.h"                   
#include "WorkerCommandDelilahComponent.h"      // Own interface


namespace samson {

	WorkerCommandDelilahComponent::WorkerCommandDelilahComponent(std::string _command , engine::Buffer * _buffer) 
        : DelilahComponent( DelilahComponent::worker_command )
	{
		command = _command;
        buffer = _buffer;
        
        setConcept(command);    // The command is the description itself
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
            std::vector<std::string> words =  au::simpleTockenize( command );
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

        // If error is returned, worker_command is automatically canceled
        if( packet->message->worker_command_response().has_error() )
        {
            setComponentFinishedWithError( packet->message->worker_command_response().error().message() );
            return;
        }
        
        if( num_confirmed_workers == num_workers )
        {
            //LM_M(("setComponentFinished() fromId(%d) num_confirmed_workers(%d)", fromId, num_confirmed_workers));
            setComponentFinished();
        }
        
        
        if( packet->message->worker_command_response().has_error() )
            error.set( packet->message->worker_command_response().error().message()  );
	}
	
	
	std::string WorkerCommandDelilahComponent::getStatus()
	{
		std::ostringstream o;
		o << "Confirmed " << num_confirmed_workers << " / " << num_workers << " workers";
		return o.str();
	}
	
}

