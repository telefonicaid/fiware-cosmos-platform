#include <iostream>				// std::cout ...
#include <iomanip>
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	
#include <fnmatch.h>

#include "logMsg/logMsg.h"             // lmInit, LM_*

#include "au/CommandLine.h"		// CommandLine
#include "au/Cronometer.h"      // au::Cronometer
#include "au/mutex/TokenTaker.h"                          // au::TokenTake

#include "au/tables/Table.h"

#include "engine/Buffer.h"      // engine::Buffer
#include "engine/Notification.h"    // engine::Notificaiton
#include "engine/DiskOperation.h"

#include "samson/common/Macros.h"             // EXIT, ...
#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "samson/common/EnvironmentOperations.h"
#include "samson/common/NotificationMessages.h" // notification_network_diconnected

#include "samson/network/Message.h"            // Message::MessageCode, ...
#include "samson/network/Packet.h"				// samson::Packet
#include "samson/network/NetworkInterface.h"			// NetworkInterface
#include "samson/network/DelilahNetwork.h"
#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/network/Packet.h"				// samson::Packet
#include "PushDelilahComponent.h"               // samson::PushDelilahComponent
#include "PopDelilahComponent.h"                // samson::PopDelilahComponent
#include "WorkerCommandDelilahComponent.h"      // samson::WorkerCommandDelilahComponent


#include "samson/delilah/SamsonDataSet.h"       // samson::SamsonDataSet
#include "samson/delilah/Delilah.h"             // Own interfce
#include "samson/delilah/DelilahConsole.h"      // samson::DelilahConsole


extern size_t delilah_random_code;

namespace samson {
    
    
    /* ****************************************************************************
     *
     * Delilah::Delilah
     */
    Delilah::Delilah(  ) : token("Delilah_token")
    {
		
        // we start with process 2 because 0 is no process & 1 is global_update messages
        id = 2;	
                
        // Listen notification about netowrk disconnection
        listen( notification_network_diconnected );

        
        // No receiver to deal with live data from queues by default ( used in samsonClient library )
        data_receiver_interface = NULL;
                
        // Notification to update state
        listen( notification_update_status );
        {
            int update_period = samson::SamsonSetup::shared()->getInt("general.update_status_period" );
            engine::Notification *notification = new engine::Notification(notification_update_status);
            engine::Engine::shared()->notify( notification, update_period );
        }        
     
        // By default update everything ( canceled in samsonClient )
        automatic_update = true;
    }
    
    
    Delilah::~Delilah()
    {
        clearAllComponents();
    }


    
    void Delilah::notify( engine::Notification* notification )
    {
        
        if( notification->isName( notification_network_diconnected ) )
        {
            std::string type = notification->environment.get("type","unknown");
            size_t id        = notification->environment.getSizeT("id",-1);

            // At the moment only a warning
            showWarningMessage(au::str("Disconnected (%s %lu )" , type.c_str() , id ));
            return;
        }
        
        if( notification->isName(notification_disk_operation_request_response) )
        {
            // Nothing to do here...
            
            return;
        }        
        
        if ( notification->isName(notification_update_status))
        {
            if( !isConnected() )
                return;
            
            au::ErrorManager error;
            
            // Create a xml version of monitorization ( common to all delilahs )
            std::ostringstream info_str;
            getInfo( info_str );
            
            // Get vector of all workers
            std::vector<size_t> workers = getWorkerIds(&error);
            
            if( error.isActivated() )
                return;
            
            // Send this message to all delilahs
            for ( size_t i = 0 ; i < workers.size() ; i++ )
            {
                Packet* p  = new Packet( Message::StatusReport );
                
                // This message is not critical - to be thrown away if worker not connected
                p->disposable = true;
                
                // Include generic information about this worker
                p->message->set_info(info_str.str() );
                
                // Packet direction
                p->to.node_type = WorkerNode;
                p->to.id = workers[i];
                
                // Send this message to all delilahs connected
                send( p , &error );
                
                // Release packet
                p->release();
                
                if( error.isActivated() )
                    return;

            }
            return;
        }        
        
        
        LM_X(1,("Delilah received an unexpected notification %s" , notification->getName() ));
        
    }
    
    
    /* ****************************************************************************
     *
     * receive - 
     */
    void Delilah::receive( Packet* packet )
    {
        LM_T(LmtNetworkNodeMessages, ("Delilah received %s" , packet->str().c_str()));

        // Message received
        Message::MessageCode msgCode = packet->msgCode;
        
        DelilahComponent *component = NULL;

        // Show messages ( usually from Network bellow )
        if( msgCode == Message::Message )
        {
            showWarningMessage( packet->message->message() );
            return;
        }

        // --------------------------------------------------------------------
        // NetworkNotification messages
        // --------------------------------------------------------------------
        
        if ( msgCode == Message::NetworkNotification )
        {
            
            if( packet->message->has_network_notification() )
            {
                if( packet->message->network_notification().has_connected_worker_id() )
                {
                    size_t worker_id = packet->message->network_notification().connected_worker_id();
                    showWarningMessage( au::str("Connected worker %lu\n", worker_id) );
                    
                    //Update operations every time a worker is connected
                    sendWorkerCommand( au::str("ls_operations -hidden -save"), NULL);
                    
                } else if( packet->message->network_notification().has_disconnected_worker_id() )
                {
                    size_t worker_id = packet->message->network_notification().disconnected_worker_id();
                    showWarningMessage( au::str("Disconnected worker %lu\n", worker_id) );
                }
                else
                    LM_W(("NetworkNotification without required information..."));
                    
                
            }
            else
                LM_W(("NetworkNotification without required information"));
            
            return;
        }
        
        // --------------------------------------------------------------------
        // StatusReport messages
        // --------------------------------------------------------------------
        
        if( msgCode == Message::StatusReport )
        {
            int worker_id = packet->from.id;
            if( worker_id != -1 )
            {
			   //LM_M(("Delilah received a status report... worker id: %d", worker_id));
                updateWorkerXMLString( worker_id , packet->message->info() );
            }
            else
            {
                LM_W(("Status report received from an unknown endpoint"));
                return;
            }
             
            return;
        }
        
        // --------------------------------------------------------------------
        // Streaming out content of a queue
        // --------------------------------------------------------------------
        
        if( msgCode == Message::StreamOutQueue )
        {
            std::string queue = packet->message->stream_output_queue().queue();
            
            if( !packet->getBuffer() )
            {
                LM_W(("StreamOutQueue message without a buffer"));
                return;
            }
            
            if( data_receiver_interface )
                data_receiver_interface->receive_buffer_from_queue(queue, packet->getBuffer() );
            else
                receive_buffer_from_queue( queue , packet->getBuffer() );
            
            return;
            
        }
        
        {
            au::TokenTaker tk( &token );
            
            size_t delilah_component_id = packet->message->delilah_component_id();
            component = components.findInMap( delilah_component_id );
            
            //LM_M(("Received with sender_id %lu (component %p)", sender_id , component));
            
            if ( component )
            {
                component->receive( packet );
                return; // If process by component, not process anywhere else
            }
            
        }
        
        // Forward the reception of this message to the client
        _receive( packet );
        
    }
	


/* ****************************************************************************
     *
     * notificationSent - 
     */
    void Delilah::notificationSent(size_t id, bool success)
    {
        // Do something
        if ((id == 0) || (success == false))
            return;
    }
    
    
#pragma mark Load data process
    
    
	/* ****************************************************************************
     *
     * pushData - 
     */
	size_t Delilah::addPushData( std::vector<std::string> fileNames , std::vector<std::string> queues )
	{
        TXTFileSet *txtFileSet = new TXTFileSet( fileNames );

        //LM_M(("Push data from files with size %lu" , txtFileSet->getTotalSize() ));
        
        if( txtFileSet->getTotalSize() == 0 )
        {
            std::ostringstream message;
            
            if( fileNames.size() == 0 )
                message << "No valid files provided";
            else
            {
                message << "No content at ";
                for (size_t i = 0 ; i < fileNames.size() ; i++)
                    message << fileNames[i] << " ";
            }
            
            showErrorMessage( message.str() );  
            
            return 0;
        }
        
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
    
    size_t Delilah::addPushData( engine::Buffer* buffer , std::vector<std::string> queues )
    {
		BufferPushDelilahComponent * d = new BufferPushDelilahComponent( buffer , queues[0] );
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
    
	size_t Delilah::addPopData( std::string queue_name , std::string fileName , bool force_flag , bool show_flag )
	{
		PopDelilahComponent * d = new PopDelilahComponent( queue_name , fileName , force_flag , show_flag );
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
    
    void Delilah::cancelComponent( size_t _id )
    {
        au::TokenTaker tk( &token );
    
        DelilahComponent* component = components.findInMap( _id );
        
        if( component )
            component->setComponentFinishedWithError("Canceled by user in delilah console");
        else
            showWarningMessage( au::str("Not possible to cancel delilah process %lu." , _id ) );
    }

    std::string Delilah::getOutputForComponent( size_t _id )
    {
        au::TokenTaker tk( &token );
        
        DelilahComponent* component = components.findInMap( _id );
        
        if( !component )
            return au::str("Delilah component %lu not found" , _id);
        return component->output.str();
    }
    
    void Delilah::setBackgroundComponent( size_t _id )
    {
        au::TokenTaker tk( &token );
        
        DelilahComponent* component = components.findInMap( _id );
        
        if( component )
            component->set_print_output_at_finish();
        else
            showWarningMessage( au::str("Not possible to set delilah process %lu in background." , _id ) );
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
	
    std::string Delilah::getListOfComponents()
    {
        au::tables::Table table( "id|type,left|status,left|time,left|completion,left|concept,left" ); 
        table.setTitle("List of delilah processes");
		
		std::map<size_t,DelilahComponent*>::iterator iter;
		for (iter = components.begin() ; iter != components.end() ; iter++)
		{
            DelilahComponent* component = iter->second;
            
            if( component->hidden )
                continue;
            
            au::StringVector values;
            values.push_back( au::str("%s_%lu", au::code64_str( delilah_random_code ).c_str() , component->getId() ) );
            values.push_back( component->getTypeName() );
            values.push_back( component->getStatusDescription() );
            
            values.push_back( au::str_time( component->cronometer.getSecondRunnig() ) );
            values.push_back( au::str_percentage( component->progress ) );
            
            values.push_back( component->concept ); 
            
            //cronometer
            table.addRow( values );
		}
		return table.str( );
    }
    
    size_t Delilah::sendWorkerCommand( std::string command , engine::Buffer *buffer )
	{
		
		// Add a components for the reception
		WorkerCommandDelilahComponent *c = new WorkerCommandDelilahComponent( command , buffer );
        
		// Get the id of this operation
		size_t tmp_id = addComponent( c );
		
		// Send the packet to necessary workers
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
    
    
    
    int Delilah::_receive( Packet* packet )
    {
        if( packet->msgCode != Message::Alert )
            LM_V(("Unused packet %s" , packet->str().c_str()));
        
        return 0;
    }    
	
    
    // Get information for monitorization
    void Delilah::getInfo( std::ostringstream& output)
    {
        if (output != output)
            LM_E(("sorry, just wanted to avoid a 'strict' warning ..."));

        // Engine
        //engine::Engine::shared()->getInfo( output );

        // Engine system
        //samson::getInfoEngineSystem(output, network);
        
        // Modules manager
        //ModulesManager::shared()->getInfo( output );
        
        // Network
        //network->getInfo( output , "main" );
        
    }    
    
    std::string Delilah::getLsLocal( std::string pattern , bool only_queues )
    {
        au::tables::Table table( "Name,left|Type,left|Size|Format,left");
                                                         
        // first off, we need to create a pointer to a directory
        DIR *pdir = opendir ("."); // "." will refer to the current directory
        struct dirent *pent = NULL;
        if (pdir != NULL) // if pdir wasn't initialised correctly
        {
            while ((pent = readdir (pdir))) // while there is still something in the directory to list
                if (pent != NULL)
                {
                    
                    std::string fileName = pent->d_name;
                    
                    if( ( fileName != ".") && ( fileName != "..") )
                    {
                        
                        struct stat buf2;
                        stat( pent->d_name , &buf2 );
                        
                        if ( ::fnmatch( pattern.c_str() , pent->d_name , 0 ) != 0 )
                            continue;
                        
                        if( S_ISREG(buf2.st_mode) )
                        {
                            size_t size = buf2.st_size;
                            if (!only_queues)                            
                                table.addRow( au::StringVector( pent->d_name, "FILE" , au::str(size,"bytes") , "-" ) );
                        }
                        if( S_ISDIR(buf2.st_mode) )
                        {
                            
                            SamsonDataSet dataSet( pent->d_name );
                            
                            if( dataSet.error.isActivated() )
                            {
                                if (!only_queues)                            
                                    table.addRow( au::StringVector( pent->d_name, "DIR" , "" , ""  ) );
                            }
                            else
                                table.addRow( au::StringVector( pent->d_name, "SAMSON queue" , dataSet.strSize() , dataSet.strFormat() ) );
                            
                        }
                        
                    }
                    
                }
            // finally, let's close the directory
            closedir (pdir);						
        }
        
        if( only_queues )
            table.setTitle(au::str("Local queues ( %s )" , pattern.c_str()));
        else
            table.setTitle(au::str("Local files ( %s )" , pattern.c_str()));
        
        
        return table.str();
    }
    
/*    
    Status Delilah::stop_repeat( size_t id )
    {
        au::TokenTaker tt(&token);
        DelilahComponent* component = components.findInMap(id);
        if( !component )
            return Error;

        if( component->type != DelilahComponent::repeat )
            return Error;

        if ( component->isComponentFinished() )
            return Error;
        
        component->setComponentFinished();
        return OK;
    }
    
    Status Delilah::stop_all_repeat(  )
    {
        au::TokenTaker tt(&token);
		au::map<size_t , DelilahComponent>::iterator it_components;
        for( it_components = components.begin() ; it_components != components.end() ; it_components++ )
        {
            DelilahComponent * component = it_components->second;
            if( component->type == DelilahComponent::repeat )
                component->setComponentFinished();
        }
        return OK;
    }
 */

    DelilahComponent* Delilah::getComponent( size_t delilah_id )
    {
        return components.findInMap(delilah_id);
    }

    
    
}

