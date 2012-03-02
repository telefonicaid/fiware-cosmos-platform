


#include "au/CommandLine.h"            // samson::CommandLine

#include "engine/Buffer.h"            // engine::Buffer


#include "samson/common/EnvironmentOperations.h"	// copyEnviroment()
#include "samson/common/MessagesOperations.h"

#include "samson/network/Packet.h"					// samson::Packet

#include "samson/delilah/Delilah.h"				// samson::Delilah
#include "DelilahComponent.h"                   
#include "WorkerCommandDelilahComponent.h"      // Own interface


namespace samson {

	WorkerCommandDelilahComponent::WorkerCommandDelilahComponent(std::string _command , engine::Buffer * _buffer ) 
        : DelilahComponent( DelilahComponent::worker_command )
	{
		command = _command;
        buffer = _buffer;
        
        // Remove the last return
        while( command.substr( command.size() - 1 ) == "\n" )
            command.erase(command.size() - 1 ,1 );
        
        setConcept( command );    // The command is the description itself
        
        // Identify specific flags in the command
        au::CommandLine cmdLine;
        cmdLine.set_flag_boolean("hidden");
        cmdLine.set_flag_boolean("save");     // Flag to identify if is necessary to save it locally
        cmdLine.set_flag_boolean("connected_workers");    // Flag to run the operation only with connected workers
        cmdLine.set_flag_uint64("worker" , (size_t)-1 );
        cmdLine.set_flag_string("group", "");
        cmdLine.set_flag_string("filter", "");
        cmdLine.set_flag_string("sort", "");
        cmdLine.set_flag_uint64("limit" , 0 );
        cmdLine.parse(command);
        
        worker_id         = cmdLine.get_flag_uint64("worker");
        hidden            = cmdLine.get_flag_bool("hidden");
        save_in_database  = cmdLine.get_flag_bool("save");
        group_field       = cmdLine.get_flag_string("group");
        filter_field      = cmdLine.get_flag_string("filter");
        sort_field        = cmdLine.get_flag_string("sort");
        connected_workers = cmdLine.get_flag_bool("connected_workers");
        limit             = cmdLine.get_flag_uint64("limit");
	}
	
	void WorkerCommandDelilahComponent::run()
	{
        // Get the workers involved in this operation
        au::Uint64Vector _workers( delilah->network->getConnectedWorkerIds() );
        
        // Check with all the workers
        std::vector<size_t> all_workers = delilah->network->getWorkerIds();
        
        // Check all the workers are connected
        if( worker_id == (size_t)-1 )
        {
            if( !connected_workers )
                if( all_workers.size() != _workers.size() )
                {
                    setComponentFinishedWithError( au::str( "Only %lu/%lu workers connected. Use -connected_workers to run this command only using connected workers" , _workers.size() , all_workers.size() ));
                    return;
                }
        
            if( all_workers.size() != _workers.size() )
                delilah->showWarningMessage(au::str("Process %lu executed only using %lu/%lu workers" , id , _workers.size() , all_workers.size() ));
        }
        
        if( _workers.size() == 0 )
        {
            setComponentFinishedWithError("Not connected to any worker");
            return;
        }
        
        // All the workers or just one of them is assigned to this worker command
        if( worker_id != (size_t)-1 )
        {
            workers.insert(worker_id);
            
            // Check if it is a real worker
            if( _workers.getNumberOfTimes(worker_id) == 0 )
            {
                setComponentFinishedWithError( au::str("Worker %lu not included in current cluster" , worker_id) );
                return;
            }
        }
        else
            workers.insert( _workers.begin() , _workers.end() );
        
        // Send to all involved workers
        std::set<size_t>::iterator it_workers;
        for ( it_workers = workers.begin() ; it_workers != workers.end() ; it_workers ++ )
        {
            Packet*           p = new Packet( Message::WorkerCommand );
            network::WorkerCommand* c = p->message->mutable_worker_command();
            c->set_command( command );
            p->message->set_delilah_component_id( id );
            
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
            
            // Duplicate data to send to this worker
            if( buffer )
            {
                size_t size = buffer->getSize();
                engine::Buffer* _buffer = engine::MemoryManager::shared()->newBuffer( "worker_commnad" , size , 0 );
                memcpy(_buffer->getData(), buffer->getData(), size );
                _buffer->setSize( size );            
                p->buffer = _buffer;
            }
            else
                p->buffer = NULL;
            
            // Information about destination....
            p->to.node_type = WorkerNode;
            p->to.id = *it_workers;

            // Send message
            delilah->network->send( p );                        
            
        }
		
	}	    
    
	void WorkerCommandDelilahComponent::receive( Packet* packet )
	{
        if ( packet->from.node_type != WorkerNode )
            LM_X(1, ("Samson protocol error"));
        
        size_t worker_id = packet->from.id;

		if( packet->msgCode == Message::WorkerCommandResponse )
        {
            
            if( workers.find( worker_id ) == workers.end() )
            {
                setComponentFinishedWithError( au::str("WorkerCommandResponse received from worker %lu not involved in this operation" ) );
                return;
            }
            confirmed_workers.insert( worker_id );
        }

        // If error is returned, worker_command is automatically canceled
        if( packet->message->worker_command_response().has_error() )
        {
            setComponentFinishedWithError( packet->message->worker_command_response().error().message() );
            return;
        }
        
        
        // Extract collections if included ( adding worker_id field )
        for( int i = 0 ; i < packet->message->collection_size() ; i++ )
        {
            std::string name = packet->message->collection(i).name();
            std::string title = packet->message->collection(i).title();
            
            network::Collection* collection = collections.findOrCreate(name);
            collection->set_name(name);
            collection->set_title(title);
            
            for( int j = 0 ; j < packet->message->collection(i).record_size() ; j++ )
            {
                network::CollectionRecord*record = collection->add_record();

                // Add always worker_id
                ::samson::add( record  , "worker_id" , worker_id , "different" );
                
                // Add all elements in the sent collection item
                for ( int k = 0 ; k < packet->message->collection(i).record(j).item_size() ; k++ )
                    record->add_item()->CopyFrom( packet->message->collection(i).record(j).item(k) );
            }
        }
        
        if( confirmed_workers.size() == workers.size() )
        {
            //LM_M(("setComponentFinished() fromId(%d) num_confirmed_workers(%d)", fromId, num_confirmed_workers));
            setComponentFinished();
            
            
            // Print the result in a table
            au::map<std::string, network::Collection >::iterator it;
            for( it = collections.begin() ; it != collections.end() ; it++ )
                print_content( it->second );
        }
 	}
	
    
    void WorkerCommandDelilahComponent::print_content( network::Collection * collection )
    {
        std::string title = collection->title();
        
        if( collection->record_size() == 0 )
        {
            if( !hidden )
                delilah->showWarningMessage("No records\n");
            return;
        }

        std::string table_name = collection->name();
        
        au::StringVector fields;
        au::StringVector formats;
                
        for ( int i = 0 ; i < collection->record(0).item_size() ; i ++ )
        {
            fields.push_back( collection->record(0).item(i).name() );
            formats.push_back( collection->record(0).item(i).format() );
        }
        
        au::tables::Table* table = new au::tables::Table( fields , formats );
        
        for (int r = 0 ; r < collection->record_size() ; r++ )
        {
            au::StringVector values;
            for ( int i = 0 ; i < collection->record(r).item_size() ; i ++ )
                values.push_back( collection->record(r).item(i).value() );
            table->addRow(values);
        }

        if( !hidden )
        {
            // Print the generated table
            delilah->showMessage( table->strSortedGroupedAndfiltered(title, group_field, sort_field, filter_field , limit ) );
        }
        
        if( save_in_database )
        {
            delilah->database.addTable( table_name , table );
            if( !hidden )
                delilah->showWarningMessage(
                    au::str("Table %s has been created locally. Type set_database_mode to check content...\n",table_name.c_str())
                                            );
        }
    }
	
	std::string WorkerCommandDelilahComponent::getStatus()
	{
        std::set<size_t>::iterator it;
		std::ostringstream o;
        o << "Workers              ";
        for( it = workers.begin() ; it != workers.end() ; it++ )
            o << *it << " ";
        o << "\n";
        
        o << "Confirmed workers    ";
        for( it = confirmed_workers.begin() ; it != confirmed_workers.end() ; it++ )
            o << *it  << " ";
        o << "\n";
		return o.str();
	}
	
}

