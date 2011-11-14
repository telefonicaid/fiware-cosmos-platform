#include <iostream>				// std::cout ...
#include <iomanip>
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	

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
#include "samson/common/NotificationMessages.h" // notification_network_diconnected

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
#include "samson/delilah/SamsonDataSet.h"       // samson::SamsonDataSet
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
        
        // Listen notification about netowrk disconnection
        listen( notification_network_diconnected );
        
        // Emit a periodic notification
        int delilah_automatic_update_period = samson::SamsonSetup::shared()->getInt( "delilah.automatic_update_period" );
        engine::Engine::shared()->notify( new engine::Notification( notification_delilah_automatic_update ) , delilah_automatic_update_period );

        // Global pointer to access delilah
        global_delilah = this;
     
        // No next worker decided
        next_worker = -1;
        
        // No operation to deal with live data from queues by default ( used in samsonClient library )
        op_delilah_process_stream_out_queue = NULL;
    }
    
    
    Delilah::~Delilah()
    {
        clearAllComponents();
    }

    
    int Delilah::getNextWorker()
    {
        int num_workers = network->getNumWorkers();
        
        if( next_worker == -1 )
            next_worker = rand()%num_workers;
        
        next_worker++;
        if( next_worker == num_workers )
            next_worker = 0;
        
        return next_worker;
    }
    
    void Delilah::notify( engine::Notification* notification )
    {
        
        if( notification->isName( notification_network_diconnected ) )
        {
            int id = notification->environment.getInt("id",-1);
            
            if( network->controllerGetIdentifier() == id )
                showWarningMessage(au::str("Controller  got disconnected"));
            else if( network->getWorkerFromIdentifier(id) != -1 )
                showWarningMessage(au::str("Worker %d got disconnected" , network->getWorkerFromIdentifier(id) ));
            else
                showWarningMessage(au::str("Some unknown network element got disconnected" , network->getWorkerFromIdentifier(id) ));
            
            return;
        }
        
        
        if( notification->isName(notification_disk_operation_request_response) )
        {
            // Nothing to do..
            return;
        }        
        
        // Send a message to the controller to receive an update of the information
        if( notification->isName(notification_delilah_automatic_update) )
        {
            if( network->isConnected( network->controllerGetIdentifier()  ) )
            {
                // Message to update the worker status list
                Packet*           p = new Packet(Message::StatusRequest);
                p->message->set_delilah_id( 1 );    // Spetial id for global update
                network->sendToController( p );
            }
            
            return;
        }
        
        LM_X(1,("Delilah received an unexpected notification"));
        
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
            {
                component->receive( fromId, msgCode, packet );
                return; // If process by component, not process anywhere else
            }
            
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
        
        
        if( msgCode == Message::StreamOutQueue )
        {
            std::string queue = packet->message->stream_output_queue().queue();
            
            if( !packet->buffer )
            {
                LM_W(("StreamOutQueue message whitout a buffer"));
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
        
        
        
        
        // Forward the reception of this message to the client
        _receive( fromId , msgCode , packet );
        
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
    
    

    
    
#define LS_QUEUES_FIELDS "/name,t=name,left /block_info/kv_info/kvs,t=#kvs,format=uint64 /block_info/kv_info/size,t=size,format=uint64 /block_info/format/key_format,t=key /block_info/format/value_format,t=value,left"

#define LS_QUEUES_FIELDS_VERBOSE "/name,t=name,left  /block_info/kv_info/kvs,t=#kvs,format=uint64 /block_info/kv_info/size,t=size,format=uint64 /rate_kvs/rate,t=#kvs/s,format=uint64 /rate_size/rate,t=Bytes/s,format=uint64 /status,t=status,left"
    
#define LS_QUEUES_FIELDS_VERBOSE_VERBOSE "/name,t=name,left /block_info/num_blocks,t=#Blocks,format=uint64 /block_info/size,title=size,format=uint64 /block_info/size_on_memory^/block_info/size,format=per,t=on_memory /block_info/size_on_disk^/block_info/size,format=per,t=on_disk /block_info/size_locked^/block_info/size,format=per,t=locked  /block_info/min_time_diff,f=time,t=oldest /block_info/max_time_diff,f=time,t=earliest /num_divisions,t=#div,uint64 /block_info/num_divisions,t=#div"
    
#define ENGINE_SHOW_COMMAND "info_command -delilah -worker -controller //engine_system /process_manager/num_running_processes^/process_manager/num_processes,t=process,format=per /memory_manager/used_memory^/memory_manager/memory,t=memory,format=per  /disk_manager/num_pending_operations+/disk_manager/num_running_operations,t=disk,f=uint64"


#define LS_STREAM_OPERATIONS "info_command -worker //stream_operation /name,t=name,left /inputs,t=inputs /outputs,t=outputs /operation,t=operation,left "

#define LS_STREAM_OPERATIONS_VERBOSE "info_command -worker //stream_operation /name,t=name,left /input_str,t=input /running_tasks,t=running_tasks /history_str,t=history "

#define LS_STREAM_OPERATIONS_VERBOSE_VERBOSE "info_command -worker //stream_operation /name,t=name,left /state,t=state /last_review,t=last_review"

    
    std::string Delilah::info( std::string command )
    {
        au::CommandLine cmd;
        cmd.set_flag_boolean("vv");
        cmd.set_flag_boolean("v");
        
        cmd.parse( command );
        
        if( cmd.get_num_arguments() == 0)
            return "No command to display information about SAMSON platform.";
        
        std::string main_command = cmd.get_argument(0);
        
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
            
            return infoCommand(command.str());
        }
        
        if( main_command == "ls_queues" )
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

            command << "info_command -worker //queue" << filter << " ";
            
            if( cmd.get_flag_bool("vv") )
                command << LS_QUEUES_FIELDS_VERBOSE_VERBOSE;
            else if( cmd.get_flag_bool("v") )
                command << LS_QUEUES_FIELDS_VERBOSE;
            else
                command << LS_QUEUES_FIELDS;
            
            
            return infoCommand(command.str());
            
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
        
        if( main_command == "engine_show" )
            return infoCommand(ENGINE_SHOW_COMMAND);
        
        
        if( main_command == "ls_block_manager" )
            return getStringInfo( "//block_manager" , getBlockManagerInfo, i_worker ); 
        
        
        if( main_command == "ls_operation_rates" )
            return getStringInfo( "//process_manager//rates//simple_rate_collection" , getOperationRatesInfo, i_worker ); 
        
        if( main_command == "ls_modules" )
        {
            
            std::string command;
            if( cmd.get_num_arguments() > 1 )
            {
                std::string argument =  cmd.get_argument(1);
                command = au::str("//modules_manager//module[starts-with(name,'%s')]" , argument.c_str() );
            }
            else
                command = "//modules_manager//module";
            
            std::string txt = getStringInfo( command , getModuleInfo, i_controller | i_worker | i_delilah ); 
            return txt;
            
        }
        
        
        if( main_command == "ls_operations" )
        {
            std::string command;
            if( cmd.get_num_arguments() > 1 )
            {
                std::string argument =  cmd.get_argument(1);
                command = au::str("/modules_manager//operation[starts-with(name,'%s')]" , argument.c_str() );
            }
            else
                command = "/modules_manager//operation";
            
            return getStringInfo( command , getOperationInfo, i_controller ); 
        }
        
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
        
        if( main_command == "ls_local" )
        {
            return getLsLocal( ); 
        }
        
        
        return au::str("Command %s unkown" , main_command.c_str() );
        
    }

    std::string Delilah::getLsLocal()
    {
        std::ostringstream output;
        
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
                        
                        if( S_ISREG(buf2.st_mode) )
                        {
                            size_t size = buf2.st_size;
                            output << "\t FILE      " << std::setw(20) << std::left <<  pent->d_name << " " << au::str(size,"bytes") << "\n";
                        }
                        if( S_ISDIR(buf2.st_mode) )
                        {
                            
                            SamsonDataSet dataSet( pent->d_name );
                            
                            if( dataSet.error.isActivated() )
                                output << "\t DIR       " << std::setw(20) << std::left << pent->d_name << "\n";
                            else
                                output << "\t DATA-SET  " << std::setw(20) << std::left << pent->d_name << " " << dataSet.str() << "\n";
                            
                        }
                        
                    }
                    
                }
            // finally, let's close the directory
            closedir (pdir);						
        }
        
        
        return output.str();
    }
    
}

