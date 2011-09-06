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


#include "samson/delilah/Delilah.h"             // Own interfce

#define notification_delilah_automatic_update "notification_delilah_automatic_update"


namespace samson {

    // Private token to protect the local list of components
    au::Token token_xml_info("token_xml_info");
    // Global xml-based information from the system
    std::string xml_info;
    // General document with the content of xml_info
    pugi::xml_document doc;
    // Cronometer for xml_info update
    au::Cronometer cronometer_xml_info;

    
    
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
            au::TokenTaker tt( &token_xml_info );
            
            std::ostringstream output;
            
            output << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
            output << "<samson>\n";
            
            std::string controller_and_worker_status = packet->message->info();
            output << controller_and_worker_status;
            
            // Get my own status here ( delilah )
            getInfo( output );
            
            output << "</samson>\n";

            // Get the resulting string
            xml_info = output.str();
            
            std::istringstream is_xml_document( xml_info );
            
            cronometer_xml_info.reset();
            
            doc.reset();
            pugi::xml_parse_result result = doc.load( is_xml_document );
            
            if( !result )
            {
                // Do something with this error
            }
            
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
            return false;
        
		return( !c->isComponentFinished() );
        
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
	
    int Delilah::getUpdateSeconds()
    {
        au::TokenTaker tt( &token_xml_info );
        
        int time = cronometer_xml_info.diffTimeInSeconds();
   
        int worker_update_time = (int) pugi::UInt64( doc , "//controller/update_time" );
        
        time += worker_update_time;
        
        return time;
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
    
    std::string Delilah::getStringInfo( std::string path , node_to_string_function _node_to_string_function  , int options )
    {
        
        if( !checkXMLInfoUpdate() )
            return 0;
        
        {
            au::TokenTaker tt( &token_xml_info );
            
            
            std::ostringstream output;
            
            output << "\n";
            
            if( options & i_controller )
            {
                if( !(options & i_no_title ) )
                {
                    output << "============================================================\n";
                    output << "Controller :\n";
                    output << "============================================================\n";
                    output << "\n";
                }
                
                pugi::xpath_node_set nodes  = pugi::select_nodes( doc , "//controller" + path );
                
                for ( size_t i = 0 ; i < nodes.size() ; i++ )
                {
                    const pugi::xml_node& node = nodes[i].node(); 
                    output << _node_to_string_function( node ) << "\n" ;
                }      
                output << "\n";
            }
            
            if( options & i_worker )
            {
                pugi::ValuesCollection workers_ids = pugi::values(doc, "//worker/id");
                
                for ( size_t w = 0 ; w < workers_ids.size() ; w++ )
                {
                    
                    if( !(options & i_no_title ) )
                    {
                        output << "============================================================\n";
                        output << "Worker " << workers_ids[w] << ":\n";
                        output << "============================================================\n";
                        output << "\n";
                    }
                    
                    pugi::xpath_node_set nodes  = pugi::select_nodes( doc , "//worker[id=" + workers_ids[w] + "]" + path );
                    
                    for ( size_t i = 0 ; i < nodes.size() ; i++ )
                    {
                        const pugi::xml_node& node = nodes[i].node(); 
                        output << _node_to_string_function( node ) << "\n" ;
                    }            
                    
                    output << "\n";
                    
                }
            }
            
            
            if( options & i_delilah )
            {
                if( !(options & i_no_title ) )
                {
                    output << "============================================================\n";
                    output << "Delilah :\n";
                    output << "============================================================\n";
                    output << "\n";
                }
                
                pugi::xpath_node_set nodes  = pugi::select_nodes( doc , "//delilah" + path );
                
                for ( size_t i = 0 ; i < nodes.size() ; i++ )
                {
                    const pugi::xml_node& node = nodes[i].node(); 
                    output << _node_to_string_function( node ) << "\n" ;
                }            
            }
            
            output << "\n";
            return output.str();
            
        }
        
        
    }    
    
    bool Delilah::checkXMLInfoUpdate()
    {
        int soft_limit = 10;
        int hard_limit  = 60;
        
        int time = getUpdateSeconds();
        
        if( time < soft_limit )
            return true;
        
        if( time < hard_limit )
        {
            showWarningMessage( au::str( "Monitorization information is %d seconds old" , time ) );
            return true;
        }
        
        showErrorMessage( au::str( "Monitorization information is %d seconds old" , time ) );
        return false;
    }
    
}

