#include <iostream>				// std::cout ...
#include <iomanip>
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	
#include <fnmatch.h>

#include "logMsg/logMsg.h"             // lmInit, LM_*

#include "au/CommandLine.h"		// CommandLine
#include "au/Cronometer.h"      // au::Cronometer
#include "au/TokenTaker.h"                          // au::TokenTake

#include "tables/Table.h"

#include "engine/Buffer.h"      // engine::Buffer
#include "engine/Notification.h"    // engine::Notificaiton

#include "samson/common/Info.h"                 // samson::Info
#include "samson/common/Macros.h"             // EXIT, ...
#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "samson/common/EnvironmentOperations.h"
#include "samson/common/NotificationMessages.h" // notification_network_diconnected

#include "samson/network/Message.h"            // Message::MessageCode, ...
#include "samson/network/Packet.h"				// samson::Packet
#include "samson/network/NetworkInterface.h"			// NetworkInterface

#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/network/Packet.h"				// samson::Packet
#include "UploadDelilahComponent.h"           // samson::DelilahLoadDataProcess
#include "DownloadDelilahComponent.h"         // samson::DelilahLoadDataProcess
#include "PushDelilahComponent.h"               // samson::PushDelilahComponent
#include "PopDelilahComponent.h"                // samson::PopDelilahComponent
#include "CommandDelilahComponent.h"            // samson::CommandDelilahComponent
#include "WorkerCommandDelilahComponent.h"      // samson::WorkerCommandDelilahComponent


#include "samson/delilah/DelilahUtils.h"        // node_to_string_function
#include "samson/delilah/SamsonDataSet.h"       // samson::SamsonDataSet
#include "samson/delilah/Delilah.h"             // Own interfce
#include "samson/delilah/DelilahConsole.h"      // samson::DelilahConsole

#define notification_delilah_automatic_update "notification_delilah_automatic_update"

samson::Delilah* global_delilah = NULL;

namespace samson {
    
    
    /* ****************************************************************************
     *
     * Delilah::Delilah
     */
    Delilah::Delilah( NetworkInterface* _network ) : DelilahBase( ) , token("Delilah_token")
    {
        
        network = _network;		// Keep a pointer to our network interface element
        network->setReceiver(this);
		
        id = 2;	// we start with process 2 because 0 is no process & 1 is global_update messages
		
        finish = false;				// Global flag to finish threads
        
        // By default, no traces
        trace_on =  false;
        
        // Listen notification about netowrk disconnection
        listen( notification_network_diconnected );

        // Global pointer to access delilah
        global_delilah = this;
     
        // No next worker decided
        next_worker = -1;
        
        // No operation to deal with live data from queues by default ( used in samsonClient library )
        op_delilah_process_stream_out_queue = NULL;
        
        // Bu default is is not updated automatically
        automatic_update =  false;
        
        // Notification to update state
        listen( notification_update_status );
        {
            int update_period = samson::SamsonSetup::shared()->getInt("general.update_status_period" );
            engine::Notification *notification = new engine::Notification(notification_update_status);
            engine::Engine::shared()->notify( notification, update_period );
        }        

        // Notification to update local data bases ( queues, operations, etc...) when required
        listen( notification_delilah_automatic_update );
        {
            // Notification every second but only used if some flags are tru
            engine::Notification *notification = new engine::Notification(notification_delilah_automatic_update);
            engine::Engine::shared()->notify( notification, 2 ); // automatic update every 2 seconds
        }        
        
    }
    
    
    Delilah::~Delilah()
    {
        clearAllComponents();
    }

    
    size_t Delilah::getNextWorkerId()
    {
        
        std::vector<size_t> workers = network->getWorkerIds();
                
        if( next_worker == -1 )
        {
            int max = workers.size();
            int r = rand();
            next_worker = r%max;
            
            LM_V(("Next worker %d ( r %d max %d)" , next_worker , r , max ));
            
        }
        
        next_worker++;
        if( next_worker == (int)workers.size() )
            next_worker = 0;
        
        return workers[ next_worker ];
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
            // Nothing to do..
            return;
        }        
        
        if ( notification->isName(notification_update_status))
        {
            // Create a xml version of monitorization ( common to all delilahs )
            std::ostringstream info_str;
            getInfo( info_str );
            
            // Get vector of all workers
            std::vector<size_t> workers = network->getWorkerIds();
            
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
                network->send( p );
            }
            return;
        }        
        
        if ( notification->isName( notification_delilah_automatic_update ) )
        {
            // Add a hidden command to update something
            if( automatic_update )
            {
                // Update local list of queus automatically
                sendWorkerCommand("ls_queues -a -hidden -save" , NULL);
                
                
            }
            
            return;
        }
        
        LM_X(1,("Delilah received an unexpected notification %s" , notification->getName() ));
        
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
                    
                }

                if( packet->message->network_notification().has_disconnected_worker_id() )
                {
                    size_t worker_id = packet->message->network_notification().disconnected_worker_id();
                    showWarningMessage( au::str("Disconnected worker %lu\n", worker_id) );
                }
                
            }
            
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
        
        
        if( msgCode == Message::StreamOutQueue )
        {
            std::string queue = packet->message->stream_output_queue().queue();
            
            if( !packet->buffer )
            {
                LM_W(("StreamOutQueue message without a buffer"));
                return;
            }
            
            if( op_delilah_process_stream_out_queue )
                op_delilah_process_stream_out_queue( queue , packet->buffer );
            else
            {
                
                size_t counter = stream_out_queue_counters.getCounterFor( queue );
                
                mkdir( "stream_out_queues" , 0755 );    // Setup this directory ?
                
                std::string fileName = au::str( "stream_out_queues/queue_%s_%l05u" , queue.c_str() , counter );
                size_t size = packet->buffer->getSize();
                
                showMessage( au::str("Received stream data for queue %s (%s). Stored at file %s" , queue.c_str() , au::str( size ).c_str() , fileName.c_str() ));
                
                // Disk operation....
                engine::DiskOperation* operation = engine::DiskOperation::newWriteOperation( packet->buffer ,  fileName , getEngineId()  );
                engine::DiskManager::shared()->add( operation );
                
            }
            
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
        au::tables::Table table( au::StringVector( "id" ,"type" , "status" , "concept" ) , au::StringVector( "" ,"left" , "left" , "concept" ) );
		
		std::map<size_t,DelilahComponent*>::iterator iter;
		for (iter = components.begin() ; iter != components.end() ; iter++)
		{
            DelilahComponent* component = iter->second;
            table.addRow( au::StringVector( component->getIdStr() ,  component->getTypeName() ,  component->getStatusDescription() , component->concept ) );
		}
		return table.str( "List of delilah processes" );
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
        LM_W(("Unused packet %s" , packet->str().c_str()));
        
        if( packet->buffer )
            engine::MemoryManager::shared()->destroyBuffer( packet->buffer );

        return 0;
    }    
	
    
    // Get information for monitorization
    void Delilah::getInfo( std::ostringstream& output)
    {
        // Engine
        engine::Engine::shared()->getInfo( output );

        // Engine system
        samson::getInfoEngineSystem(output, network);
        
        // Modules manager
        ModulesManager::shared()->getInfo( output );
        
        // Network
        network->getInfo( output , "main" );
        
    }    
    
    // Generic function to get a tabular information scaning the xml document

// --------------------------------------------------------------------------------------------------------------------------------
// LS_QUEUES
// --------------------------------------------------------------------------------------------------------------------------------
    
#define LS_QUEUES_NAME               " name,t=name,left "    
#define LS_QUEUES_KVS                " block_info/kv_info/kvs,t=#kvs,format=uint64,sum "
#define LS_QUEUES_SIZE               " block_info/kv_info/size,t=size,format=uint64,sum "
#define LS_QUEUES_KEY                " block_info/format/key_format,t=key "
#define LS_QUEUES_VALUE              " block_info/format/value_format,t=value,left "
#define LS_QUEUES_RATE_KVS           " rate_kvs/rate,t=#kvs/s,format=uint64,sum "
#define LS_QUEUES_RATE_SIZE          " rate_size/rate,t=Bytes/s,format=uint64,sum "   
#define LS_QUEUES_STATUS             " status,t=status,left "    
#define LS_QUEUES_BLOCKS_NUM         " block_info/num_blocks,t=#Blocks,format=uint64,sum "     
#define LS_QUEUES_BLOCKS_SIZE        " block_info/size,title=size,format=uint64,sum "
#define LS_QUEUES_BLOCKS_MEMORY      " block_info/size_on_memory,format=uint64,t=on_memory,sum "   
#define LS_QUEUES_BLOCKS_DISK        " block_info/size_on_disk,format=uint64,t=on_disk,sum "
#define LS_QUEUES_BLOCKS_LOCKED      " block_info/size_locked,format=uint64,t=locked,sum " 
#define LS_QUEUES_BLOCKS_TIME_BEGIN  " block_info/min_time_diff,f=time,t=oldest,vector "
#define LS_QUEUES_BLOCKS_TIME_END    " block_info/max_time_diff,f=time,t=earliest,vector " 
    
#define LS_QUEUES_FIELDS                  LS_QUEUES_NAME LS_QUEUES_KVS LS_QUEUES_SIZE LS_QUEUES_KEY LS_QUEUES_VALUE
#define LS_QUEUES_FIELDS_VERBOSE          LS_QUEUES_NAME LS_QUEUES_KVS LS_QUEUES_SIZE LS_QUEUES_RATE_KVS LS_QUEUES_RATE_SIZE LS_QUEUES_STATUS    
#define LS_QUEUES_FIELDS_VERBOSE_VERBOSE  LS_QUEUES_NAME LS_QUEUES_BLOCKS_NUM LS_QUEUES_BLOCKS_SIZE LS_QUEUES_BLOCKS_MEMORY LS_QUEUES_BLOCKS_DISK LS_QUEUES_BLOCKS_LOCKED LS_QUEUES_BLOCKS_TIME_BEGIN LS_QUEUES_BLOCKS_TIME_END

    
// --------------------------------------------------------------------------------------------------------------------------------    
    
#define LS_ENGINES_FIELDS " uptime,title=up_time,f=time,vector,min_max "                  \
                          " process_manager/num_running_processes,t=#cores,sum "          \
                          " process_manager/num_processes,t=#max_cores,sum "              \
                          " memory_manager/used_memory,t=used_memory,format=uint64,sum "  \
                          " memory_manager/memory,t=memory,format=uint64,sum "            \
                          " disk_manager/num_pending_operations,t=disk_ops,f=uint64,sum " \

// --------------------------------------------------------------------------------------------------------------------------------    

#define LS_STREAM_OPERATIONS_FIELDS " worker_id,vector,t=Workers "          \
                                    " name,t=name,left"                     \
                                    " inputs,t=inputs "                     \
                                    " outputs,t=outputs "                   \
                                    " operation,t=operation,left "          \

#define LS_STREAM_OPERATIONS_FIELDS_VERBOSE "name,t=name,left"                     \
                                            "/input_str,t=input"                   \
                                            "/running_tasks,t=running_tasks"       \
                                            "/history_str,t=history "              \

#define LS_STREAM_OPERATIONS_FIELDS_VERBOSE_VERBOSE  "name,t=name,left"            \
                                                     "/last_review,t=last_review"  \

// --------------------------------------------------------------------------------------------------------------------------------    
    
    
#define LS_BLOCKS_FIELDS " id,t=id "                                              \
                         " size,t=size,format=uint64 "                            \
                         " state,t=state /tasks_str,t=tasks "                     \
                         " kv_header/format/key_format,t=key "                    \
                         " kv_header/format/value_format,t=value,left "           \
                         " kv_header/kv_info/kvs,t=#kvs,format=uint64 "           \
                         " kv_header/kv_info/size,t=size,format=uint64 "          

#define LS_BLOCKS_FIELDS_VERBOSE " id,t=id "                           \
                                 " size,t=size,format=uint64 "         \
                                 " state,t=state /tasks_str,t=tasks "  \
                                 " lists_str,t=lists,left "            \

// --------------------------------------------------------------------------------------------------------------------------------    
    
#define PS_STREAM_FIELDS " id,t=id "                     \
                         " state,t=state "               \
                         " operation,t=operation,left "  \
                         " input_0,t=input_0 "           \
                         " input_1,t=input_1 "           \
    
    
    std::string Delilah::info( std::string external_command )
    {
        // Parse external command
        au::CommandLine cmd;
        cmd.set_flag_boolean("vv");
        cmd.set_flag_boolean("v");
        cmd.set_flag_boolean("w");
        cmd.set_flag_boolean("first");
        cmd.set_flag_int("limit" , 0);
        cmd.set_flag_boolean("show_command");
        cmd.parse( external_command );

        // Get values for all options
        int limit = cmd.get_flag_int("limit");

        // Check
        if( cmd.get_num_arguments() == 0)
            return "No command to display information about SAMSON platform.";
        
        // Command to be executed
        std::string command;
        
        std::string main_command = cmd.get_argument(0);
        
        if( main_command == "ls_queues" )
        {

            command.append( "print_table queues " );

            if( cmd.get_flag_bool("w") )
                command.append(" worker_id ");
            
            if( cmd.get_flag_bool("vv") )
                command.append( LS_QUEUES_FIELDS_VERBOSE_VERBOSE );
            else if( cmd.get_flag_bool("v") )
                command.append( LS_QUEUES_FIELDS_VERBOSE );
            else
                command.append( LS_QUEUES_FIELDS );
            
            if( cmd.get_flag_bool("w") )
                command.append(" -divide worker_id -sort name ");
            else
                command.append(" -group name ");
            
            
            if( cmd.get_num_arguments() > 1 )
                command.append( au::str( " -where name=%s*" , cmd.get_argument(1).c_str() ) );

            // Limit
            command.append( au::str(" -limit %d " , limit ) );
            
        } 
        else if ( main_command == "ps_stream" )
        {
            command.append( "print_table tasks " );
            
            if( cmd.get_flag_bool("w") )
                command.append(" worker_id ");
            
            command.append( PS_STREAM_FIELDS );
            
            command.append(" -divide worker_id ");
            
            // Limit
            command.append( au::str(" -limit %d " , limit ) );            
            
            
            
        }
        else if ( main_command == "ls_engines" )
        {
            command.append( "print_table engines " );
            
            if( cmd.get_flag_bool("w") )
                command.append(" worker_id ");
            
            command.append( LS_ENGINES_FIELDS );
            
            if( cmd.get_flag_bool("w") )
                command.append(" -divide worker_id ");
            else
                command.append(" -group name ");
            
            
            if( cmd.get_num_arguments() > 1 )
                command.append( au::str( " -where name=%s*" , cmd.get_argument(1).c_str() ) );
            
        } 
        else if ( main_command == "ls_engine_delilah" )
        {
            command.append( "print_table engine_delilah " );
            command.append( LS_ENGINES_FIELDS );
        }
        else if( main_command == "ls_stream_operations" )
        {
            command.append( "print_table stream_operations " );
            
            if( cmd.get_flag_bool("w") )
                command.append(" worker_id ");
            
            if( cmd.get_flag_bool("vv") )
                command.append( LS_STREAM_OPERATIONS_FIELDS_VERBOSE_VERBOSE );
            else if( cmd.get_flag_bool("v") )
                command.append( LS_STREAM_OPERATIONS_FIELDS_VERBOSE );
            else
                command.append( LS_STREAM_OPERATIONS_FIELDS );
            
            if( cmd.get_flag_bool("w") )
                command.append(" -divide worker_id -sort name ");
            else
                command.append(" -group name ");
            
            
            if( cmd.get_num_arguments() > 1 )
                command.append( au::str( " -where name=%s*" , cmd.get_argument(1).c_str() ) );
            
            // Limit
            command.append( au::str(" -limit %d " , limit ) );
            
        }        
        else if( main_command == "ls_blocks" )
        {
            
            // In this case we have to select a worker to show
            
            if ( cmd.get_num_arguments() < 2 )
                return au::str( au::red , "usage: ls_blocks worker_id" );
            
            command.append( "print_table blocks " );
            
            if( cmd.get_flag_bool("v") )
                command.append( LS_BLOCKS_FIELDS_VERBOSE );
            else
                command.append( LS_BLOCKS_FIELDS );
            
            // Condition of the worker
            command.append( au::str(" -where worker_id=%s" , cmd.get_argument(1).c_str() ) );
            
        }
        else if( main_command == "ls_operations" )
        {
            command.append( "print_table operations " );
            
            if( cmd.get_flag_bool("w") )
                command.append(" worker_id ");
            
            if( cmd.get_flag_bool("v") )
                command.append( " name type input_description,t=Inputs output_description,t=Outputs ");            
            else
                command.append( " name type help,left ");            
            
            if( cmd.get_flag_bool("w") )
                command.append(" -divide worker_id -sort name ");
            else
                command.append(" -group name ");
            
            if( cmd.get_num_arguments() > 1 )
                command.append( au::str(" -where name=%s*", cmd.get_argument(1).c_str() ) );
            
            if ( cmd.get_flag_bool("first") )
                command.append(" -first ");
            
        }
        else if( main_command == "ls_modules" )
        {
            command.append( "print_table modules " );
            
            if( cmd.get_flag_bool("w") )
                command.append( " worker_id ");
            
            // Add fields
            command.append(" name version,different num_operations,t=#Ops num_datas,t=#Datas author,left ");            
            
            if( cmd.get_flag_bool("w") )
                command.append(" -divide worker_id ");
            else
                command.append(" -group name ");
                       
            if( cmd.get_num_arguments() > 1 )
                command.append( au::str(" -where name=%s*", cmd.get_argument(1).c_str() ) );
        }
        else
        {
            return au::str("Command %s not implemented" , main_command.c_str() );
        }

        // Option to show command on screen
        if( cmd.get_flag_bool("show_command") )
            return au::str( au::purple , "%s" , command.c_str() );
        
        // Limit to the size of console for easy visualitzation
        std::string result = runDatabaseCommand( command );
        return au::strToConsole( result );
        
        
        
/*        
        if( main_command == "ls" )
        {
            std::string filter = "";
            
            if( cmd.get_num_arguments() > 1 )
            {
                std::string filter_parameter = cmd.get_argument(1);
                
                if( filter_parameter.substr( filter_parameter.length()-1 , 1) == "*" )
                    filter_parameter = filter_parameter.substr( 0 , filter_parameter.length() - 1);
                
                filter = au::str("[starts-with(name,'%s')]" , filter_parameter.c_str() );
            }
            
            
            std::ostringstream command;
            
            command << "info_command -controller //queue" << filter;
            command << " /name,title=Name,l /num_files,t=#files /kv_info/kvs,t=#kvs,f=uint64 /kv_info/size,t=size,f=uint64 /format/key_format,t=key /format/value_format,t=value,left -no_title";
            
            return infoCommand( command.str() );
        }
        

        
        if( main_command == "ls_stream_activity" )
        {
            return infoCommand( "info_command //activity/log -worker /time,t=time /txt,t=log,left" );
        }
        
        
        if( main_command == "ls_stream_operations" )
        {
            if( cmd.get_flag_bool("vv")  )
                return infoCommand(LS_STREAM_OPERATIONS_VERBOSE_VERBOSE);
            else if( cmd.get_flag_bool("v")  )
                return infoCommand(LS_STREAM_OPERATIONS_VERBOSE);
            else
                return infoCommand(LS_STREAM_OPERATIONS);
            
        }
        
        if( main_command == "ls_blocks" )
        {
            std::string command;
            
            if( cmd.get_flag_bool("v")  )
                command.append( LS_BLOCKS_VERBOSE );
            else
                command.append( LS_BLOCKS );
            
            int limit = cmd.get_flag_int("limit");

            if( limit > 0)
                command.append( au::str(" -limit %d " , limit) );
            
            return infoCommand( command );
            
        }
        
        if( main_command == "engine_show" )
            return infoCommand(ENGINE_SHOW_COMMAND);
        
        
        if( main_command == "ls_block_manager" )
            return getStringInfo( "//block_manager" , getBlockManagerInfo, i_worker ); 
        
        
        if( main_command == "ls_operation_rates" )
            return getStringInfo( "//process_manager//rates//simple_rate_collection" , getOperationRatesInfo, i_worker ); 
        
        if( main_command == "ls_datas" )
        {
            std::string command;
            if( cmd.get_num_arguments() > 1 )
            {
                std::string argument =  cmd.get_argument(1);
                command = au::str("/modules_manager//data[starts-with(name,'%s')]" , argument.c_str() );
            }
            else
                command = "/modules_manager//data";
            
            return getStringInfo( command , getDataInfo, i_controller ); 
        }
        
        if( main_command == "ps_network" )
            return getStringInfo("/network", getNetworkInfo, i_controller | i_worker | i_delilah ); 
        
        if( main_command == "ps_jobs" )
            return getStringInfo("/job_manager//job", getJobInfo,i_controller  ); 
        
        if( main_command == "ps_tasks" )
        {
            
            std::string txt = getStringInfo("/controller_task_manager//controller_task", getTaskInfo, i_controller  ); 
            std::string txt2 = getStringInfo("/worker_task_manager//worker_task", getWorkerTaskInfo, i_worker  ); 
            return  txt + txt2;
        }
        
        if( main_command == "ps_stream" )
        {
            std::string info_command ="info_command //queue_task  -worker /id,t=id /state,t=state /operation,t=operation,left /input_0,t=input_0 /input_1,t=input_1";
            return infoCommand( info_command );
            
            //return getStringInfo("/stream_manager//queue_task", getQueueTaskInfo, i_worker ); 
        }
        
        
        return au::str("Command %s Unknown" , main_command.c_str() );
        
        */
        
    }

    std::string Delilah::getLsLocal( std::string pattern )
    {
        au::tables::Table* table = new au::tables::Table( au::StringVector( "Name" , "Type" , "Description" ) , au::StringVector( "left" , "left" , "left" ) );
                                                         
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
                            
                            table->addRow( au::StringVector( pent->d_name, "FILE" , au::str(size,"bytes")   ) );
                        }
                        if( S_ISDIR(buf2.st_mode) )
                        {
                            
                            SamsonDataSet dataSet( pent->d_name );
                            
                            if( dataSet.error.isActivated() )
                            {
                                table->addRow( au::StringVector( pent->d_name, "DIR" , ""   ) );
                            }
                            else
                                table->addRow( au::StringVector( pent->d_name, "SAMSON Dataset" , dataSet.str()   ) );
                            
                        }
                        
                    }
                    
                }
            // finally, let's close the directory
            closedir (pdir);						
        }
        
        return table->str(au::str("Local files ( %s )" , pattern.c_str()));
    }
    
}

