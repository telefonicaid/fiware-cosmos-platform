#include <iostream>				// std::cout ...

#include "logMsg/logMsg.h"             // lmInit, LM_*

#include "au/CommandLine.h"		// CommandLine
#include "au/Cronometer.h"      // au::Cronometer
#include "au/TokenTaker.h"                          // au::TokenTake


#include "engine/Buffer.h"      // engine::Buffer
#include "engine/Notification.h"    // engine::Notificaiton

#include "samson/common/Info.h"                 // samson::Info
#include "samson/common/Macros.h"             // EXIT, ...
#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "samson/common/EnvironmentOperations.h"

#include "samson/network/Message.h"            // Message::MessageCode, ...
#include "samson/network/Packet.h"				// samson::Packet
#include "samson/network/Network.h"			// NetworkInterface
#include "samson/network/Endpoint.h"			// Endpoint

#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/network/Packet.h"				// samson::Packet
#include "UploadDelilahComponent.h"           // samson::DelilahLoadDataProcess
#include "DownloadDelilahComponent.h"         // samson::DelilahLoadDataProcess
#include "PushDelilahComponent.h"               // samson::PushDelilahComponent
#include "PopDelilahComponent.h"                // samson::PopDelilahComponent
#include "CommandDelilahComponent.h"            // samson::CommandDelilahComponent
#include "WorkerCommandDelilahComponent.h"      // samson::WorkerCommandDelilahComponent


#include "samson/delilah/DelilahUtils.h"        // node_to_string_function
#include "samson/delilah/Delilah.h"             // Own interfce

#define notification_delilah_automatic_update "notification_delilah_automatic_update"


samson::Delilah* global_delilah = NULL;

namespace samson {
    
    
    /* ****************************************************************************
     *
     * Delilah::Delilah
     */
    Delilah::Delilah( NetworkInterface* _network , bool automatic_update ) : token("Delilah_token")
    {
		
        // Description for the PacketReceiver
        packetReceiverDescription = "delilah";
        
        network = _network;		// Keep a pointer to our network interface element
        network->setPacketReceiver(this);
        network->setNodeName("Delilah");
		
        id = 2;	// we start with process 2 because 0 is no process & 1 is global_update messages
		
        finish = false;				// Global flag to finish threads
        
        // By default, no traces
        trace_on =  false;
        
        // Default component to update local list of queues and operations
        listen( notification_delilah_automatic_update );
        
        // Emit a periodic notification
        int delilah_automatic_update_period = samson::SamsonSetup::getInt( "delilah.automatic_update_period" );
        engine::Engine::shared()->notify( new engine::Notification( notification_delilah_automatic_update ) , delilah_automatic_update_period );

        // Global pointer to access delilah
        global_delilah = this;
        
    }
    
    
    Delilah::~Delilah()
    {
        clearAllComponents();
    }

    
    void Delilah::notify( engine::Notification* notification )
    {
        if( !notification->isName(notification_delilah_automatic_update) )
            LM_X(1,("Delilah received an unexpected notification"));
        
        
        // Send a message to the controller to receive an update of the information
        if( network->isConnected( network->controllerGetIdentifier()  ) )
        {
            // Message to update the worker status list
            Packet*           p = new Packet(Message::StatusRequest);
            p->message->set_delilah_id( 1 );    // Spetial id for global update
            network->sendToController( p );
        }
        
        
    }
    
    
    /* ****************************************************************************
     *
     * quit - 
     */
    void Delilah::quit()
    {
        finish = true;
        network->quit();
    }
	
    
    
    /* ****************************************************************************
     *
     * receive - 
     */
    void Delilah::receive( Packet* packet )
    {
        LM_T(LmtNodeMessages, ("Delilah received %s" , packet->str().c_str()));
        
        int fromId = packet->fromId;
        Message::MessageCode msgCode = packet->msgCode;
        
        DelilahComponent *component = NULL;
        
        {
            au::TokenTaker tk( &token );
            
            size_t sender_id = packet->message->delilah_id();
            component = components.findInMap( sender_id );
            
            //LM_M(("Received with sender_id %lu (component %p)", sender_id , component));
            
            if ( component )
                component->receive( fromId, msgCode, packet );
            
        }
        
        // StatusResponses are processed here
        
        if( msgCode == Message::StatusResponse )
        {
            
            std::ostringstream output;
            
            output << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
            output << "<samson>\n";
            
            std::string controller_and_worker_status = packet->message->info();
            output << controller_and_worker_status;
            
            // Get my own status here ( delilah )
            getInfo( output );
            
            output << "</samson>\n";

            // Get the resulting string
            updateXMLString( output.str() );
            
            return;
        }
        
        
        if( !component )
        {
            
            // Forward the reception of this message to the client
            _receive( fromId , msgCode , packet );
        }
        
    }
	


/* ****************************************************************************
     *
     * notificationSent - 
     */
    void Delilah::notificationSent(size_t id, bool success)
    {
        // Do something
    }
    
    
#pragma mark Load data process
    
	/* ****************************************************************************
     *
     * loadData - 
     */
	size_t Delilah::addUploadData( std::vector<std::string> fileNames , std::string queue , bool compresion ,int max_num_threads)
	{
		DelilahUploadComponent * d = new DelilahUploadComponent( fileNames , queue , compresion, max_num_threads );
		
		size_t tmp_id = addComponent(d);	
		
		d->run();
		
		return tmp_id;
	}
	
	/* ****************************************************************************
	 *
	 * download data - 
	 */
	
	
	size_t Delilah::addDownloadProcess( std::string queue , std::string fileName , bool force_flag )
	{
		DelilahDownloadComponent *d = new DelilahDownloadComponent( queue , fileName , force_flag );
		size_t tmp_id = addComponent(d);	
		d->run();
		
		return tmp_id;
	}
    
	/* ****************************************************************************
     *
     * pushData - 
     */
	size_t Delilah::addPushData( std::vector<std::string> fileNames , std::vector<std::string> queues )
	{
        TXTFileSet *txtFileSet = new TXTFileSet( fileNames );
        return addPushData( txtFileSet , queues );
	}
    
    size_t Delilah::addPushData( DataSource* dataSource , std::vector<std::string> queues )
    {
		PushDelilahComponent * d = new PushDelilahComponent( dataSource , queues[0] );
        for ( size_t i = 1 ; i < queues.size() ; i++)
            d->addQueue( queues[i] );
        
		size_t tmp_id = addComponent(d);	
        
		d->run();
        
		return tmp_id;
        
    }
    

	/* ****************************************************************************
     *
     * popData - 
     */
    
	size_t Delilah::addPopData( std::string queue_name , std::string fileName , bool force_flag )
	{
		PopDelilahComponent * d = new PopDelilahComponent( queue_name , fileName , force_flag );
		size_t tmp_id = addComponent(d);	

        if( d->error.isActivated() )
        {
            
            
        }
        else
            d->run();
        
		return tmp_id;
	}
    
    
	
	
	size_t Delilah::addComponent( DelilahComponent* component )
	{
        au::TokenTaker tk( &token );
        
		size_t tmp_id = id++;
		component->setId(this, tmp_id);
		components.insertInMap( tmp_id , component );

        // Call the notifier
        delilahComponentStartNotification( component );
        
		return tmp_id;
	}
	
	void Delilah::clearComponents()
	{
        au::TokenTaker tk( &token );
        
		std::vector<size_t> components_to_remove;
		
		
		for ( au::map<size_t , DelilahComponent>::iterator c =  components.begin() ;  c != components.end() ; c++)
			if ( c->second->isComponentFinished() )
				components_to_remove.push_back( c->first );
        
		for (size_t i = 0 ; i < components_to_remove.size() ; i++)
		{
			DelilahComponent *component = components.extractFromMap( components_to_remove[i] );
			if( component )
				delete component;
		}
        
	}
    
	void Delilah::clearAllComponents()
	{
        au::TokenTaker tk( &token );
        
		std::vector<size_t> components_to_remove;
		
		
		for ( au::map<size_t , DelilahComponent>::iterator c =  components.begin() ;  c != components.end() ; c++)
            components_to_remove.push_back( c->first );
        
		for (size_t i = 0 ; i < components_to_remove.size() ; i++)
		{
			DelilahComponent *component = components.extractFromMap( components_to_remove[i] );
			if( component )
				delete component;
		}
        
	}
	
    
    
	std::string Delilah::getListOfLoads()
	{
		std::stringstream output;
		bool present = false;
		output << "-----------------------------------------------------------------\n";
		output << "Upload and download processes....\n";
		output << "-----------------------------------------------------------------\n";
		output << "\n";
		std::map<size_t,DelilahComponent*>::iterator iter;
		for (iter = components.begin() ; iter != components.end() ; iter++)
		{
			if ( iter->second->type == DelilahComponent::load )
			{
				output << iter->second->getStatus() << "\n";
				present = true;
			}
		}
		
		if( !present )
			output << "\tNo upload or download process.\n";
		output << "\n";
		output << "-----------------------------------------------------------------\n";
		
		return output.str();
	}
	
    std::string Delilah::getListOfComponents()
    {
		std::stringstream output;
		bool present = false;
		output << "-----------------------------------------------------------------\n";
		output << "List of delilah processes\n";
		output << "-----------------------------------------------------------------\n";
		output << "\n";
		std::map<size_t,DelilahComponent*>::iterator iter;
		for (iter = components.begin() ; iter != components.end() ; iter++)
		{
            output << iter->second->getDescription() << "\n";
            present = true;
		}
		
		if( !present )
			output << "\tNo processes here.\n";
		output << "\n";
		output << "-----------------------------------------------------------------\n";
		
		return output.str();    
    }
    
    size_t Delilah::sendCommand( std::string command , engine::Buffer *buffer )
	{
		
		// Add a components for the reception
		CommandDelilahComponent *c = new CommandDelilahComponent( command , buffer );
        
		// Get the id of this operation
		size_t tmp_id = addComponent( c );
		
		// Send the packet to the controller
		c->run();
		
		return tmp_id;
	}	
    
    size_t Delilah::sendWorkerCommand( std::string command , engine::Buffer *buffer )
	{
		
		// Add a components for the reception
		WorkerCommandDelilahComponent *c = new WorkerCommandDelilahComponent( command , buffer );
        
		// Get the id of this operation
		size_t tmp_id = addComponent( c );
		
		// Send the packet to the controller
		c->run();
		
		return tmp_id;
	}	
    
	
	bool Delilah::isActive( size_t id )
	{
        au::TokenTaker tk( &token );
        
		DelilahComponent *c = components.findInMap( id );
        
        if( !c )
        {
            LM_M(("No component found for id:%lu", id));
            return false;
        }
        
		return( !c->isComponentFinished() );
        
    }
    
    bool Delilah::hasError( size_t id )
    {
        au::TokenTaker tk( &token );
        
		DelilahComponent *c = components.findInMap( id );

        // No process, no error ;)
        if( !c )
            return false;
        
		return c->error.isActivated();
    }

    std::string Delilah::errorMessage( size_t id )
    {
        au::TokenTaker tk( &token );
        
		DelilahComponent *c = components.findInMap( id );
        
        // No process, no error ;)
        if( !c )
            return "Non valid delilah process";
        
		return c->error.getMessage();
    }
    
    
    std::string Delilah::getDescription( size_t id )
    {
        au::TokenTaker tk( &token );
        
		DelilahComponent *c = components.findInMap( id );
        
        if (!c)
            return "No process with this id";
        else
            return c->getStatus();
    }
    
    
    
    int Delilah::_receive(int fromId, Message::MessageCode msgCode, Packet* packet)
    {
        if( packet->buffer )
            engine::MemoryManager::shared()->destroyBuffer( packet->buffer );

        return 0;
    }    
	
    
    // Get information for monitorization
    void Delilah::getInfo( std::ostringstream& output)
    {
        
        au::xml_open(output, "delilah");
        
        // Engine
        engine::Engine::shared()->getInfo( output );

        // Engine system
        engine::getInfo(output);
        
        // Modules manager
        ModulesManager::shared()->getInfo( output );
        
        // Network
        network->getInfo( output );
        
        au::xml_close(output, "delilah");
        
    }    
    
    // Generic function to get a tabular information scaning the xml document
    
 
#pragma mark info interface ------------------------------------------------------------------------------------------------------------
    
    
#define LS_COMMAND "info_command -controller //queue /name,title=Name,l /num_files,t=#files /kv_info/kvs,t=#kvs,f=uint64 /kv_info/size,t=size,f=uint64 /format/key_format,t=key /format/value_format,t=value,left -no_title"
    
#define LS_QUEUES_COMMAND "info_command -worker //queue /name,t=name,left  /block_info/kv_info/kvs,t=#kvs,format=uint64 /block_info/kv_info/size,t=size,format=uint64  /block_info/format/key_format,t=key /block_info/format/value_format,t=value,left /status,t=status,left"
    
#define LS_QUEUES_INFO_COMMAND "info_command -worker //queue /name,t=name,left /block_info/num_blocks,t=#Blocks,format=uint64 /block_info/size,title=size,format=uint64 /block_info/size_on_memory^/block_info/size,format=per,t=on_memory /block_info/size_on_disk^/block_info/size,format=per,t=on_disk /block_info/size_locked^/block_info/size,format=per,t=locked  /block_info/min_time_diff,f=time,t=oldest /block_info/max_time_diff,f=time,t=earliest /num_divisions,t=#div,uint64 /block_info/num_divisions,t=#div"
    
    
#define ENGINE_SHOW_COMMAND "info_command -delilah -worker -controller //engine_system /process_manager/num_running_processes^/process_manager/num_processes,t=process,format=per /memory_manager/used_memory^/memory_manager/memory,t=memory,format=per  /disk_manager/num_pending_operations+/disk_manager/num_running_operations,t=disk,f=uint64"
    
#define LS_STREAM_OPERATIONS "info_command //stream_operation -controller /name,t=name /description,t=description,left"
    
    
    
    std::string Delilah::info( std::string command )
    {
        au::CommandLine cmd;
        cmd.parse( command );
        
        if( cmd.get_num_arguments() == 0)
            return "No command to display information about SAMSON platform.";
        
        std::string main_command = cmd.get_argument(0);

        if( main_command == "ls" )
            return infoCommand(LS_COMMAND);
        
        if( main_command == "ls_queues" )
            return infoCommand(LS_QUEUES_COMMAND);

        if( main_command == "ls_queues_info" )
            return infoCommand(LS_QUEUES_INFO_COMMAND);
        
        if( main_command == "engine_show" )
            return infoCommand(ENGINE_SHOW_COMMAND);
        
        return au::str("Command %s unkown" , main_command.c_str() );
        
    }


    
}

