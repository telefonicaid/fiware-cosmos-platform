#include <iostream>                     // std::cout ...

#include "logMsg/logMsg.h"                     // lmInit, LM_*
#include "logMsg/traceLevels.h"                // Trace Levels
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>

#include "au/CommandLine.h"                // CommandLine
#include "au/string.h"						// au::Format
#include "au/time.h"                        // au::todayString

#include "engine/Notification.h"            // engine::Notification


#include "samson/common/SamsonSetup.h"                  // samson::SamsonSetup
#include "samson/common/Macros.h"                     // EXIT, ...
#include "samson/common/SamsonSetup.h"				// samson::SamsonSetup
#include "samson/common/Info.h"                     // samson::Info

#include "samson/network/Message.h"                    // Message
#include "samson/network/Packet.h"                     // samson::Packet
#include "samson/network/Network.h"                    // NetworkInterface
#include "samson/network/Endpoint.h"                   // Endpoint
#include "samson/worker/SamsonWorker.h"               // Own interfce

#include "engine/MemoryManager.h"				// samson::SharedMemory

#include "engine/Engine.h"						// engine::Engine
#include "engine/DiskOperation.h"				// samson::DiskOperation
#include "engine/DiskManager.h"                // Notifications
#include "engine/ProcessManager.h"             // engine::ProcessManager
#include "samson/isolated/SharedMemoryManager.h"        // engine::SharedMemoryManager

#include "samson/common/MemoryTags.h"                 // MemoryInput , MemoryOutputNetwork ,...

#include "samson/stream/Block.h"            // samson::stream::Block
#include "samson/stream/BlockList.h"            // samson::stream::BlockList
#include "samson/stream/BlockManager.h"     // samson::stream::BlockManager
#include "samson/stream/WorkerCommand.h"           // samson::stream::WorkerCommand

#include "samson/module/ModulesManager.h"   // samson::ModulesManager

#include "samson/network/NetworkInterface.h"    // samson::NetworkInterface

#define notification_worker_update_files    "notification_worker_update_files"


namespace samson {
    
    
    WorkerLog::WorkerLog( std::string _txt )
    {
        txt = _txt;
        time = au::todayString();
    }
    
    void WorkerLog::getInfo( std::ostringstream & output )
    {
        au::xml_open(output , "log" );
        au::xml_simple( output , "txt" , txt );
        au::xml_simple( output , "time" , time );
        au::xml_close(output , "log" );
        
    }
    
    
#pragma mark
    
    /* ****************************************************************************
     *
     * Constructor
     */
    SamsonWorker::SamsonWorker( NetworkInterface* _network ) :  taskManager(this) , loadDataManager(this)
    {
        network = _network;
        network->setNodeName("SamsonWorker");
        
        // Init the stream manager
        streamManager = new stream::StreamManager(this);
        
        // Get initial time
        gettimeofday(&init_time, NULL);
        
        // Description for the PacketReceiver
        packetReceiverDescription = "samsonWorker";
        
        // Set me as the packet receiver interface
        network->setPacketReceiver(this);
        
        srand((unsigned int) time(NULL));
        
        // Add SamsonWorker as listener of the update files
        listen(notification_worker_update_files );
        
        // Add samsonWorker as listere to send an update of the satatus
        listen(notification_samson_worker_send_status_update );
        
        // Add samsonWorker as lister to send traces to delilahs
        listen(notification_samson_worker_send_trace );
        
        // add to listen messages to send a packet to a worker
        listen( notification_send_to_worker );
        
        // Listen this notification to send packets
        listen( notification_samson_worker_send_packet );
        
        // Notification of the files
        {
            int worker_update_files_period = samson::SamsonSetup::getInt("worker.update_files_period");
            engine::Notification *notification = new engine::Notification(notification_worker_update_files);
            notification->environment.set("target", "SamsonWorker");
            notification->environment.setInt("worker", network->getWorkerId() );
            engine::Engine::shared()->notify( notification, worker_update_files_period );
        }
        
        // Notification to update state
        {
            int worker_update_files_period = samson::SamsonSetup::getInt("worker.update_status_period" );
            engine::Notification *notification = new engine::Notification(notification_samson_worker_send_status_update);
            notification->environment.set("target", "SamsonWorker");
            notification->environment.setInt("worker", network->getWorkerId() );
            engine::Engine::shared()->notify( notification, worker_update_files_period );
        }
        
        
        {
            int check_finish_tasks_period = samson::SamsonSetup::getInt("worker.period_check_finish_tasks" );
            engine::Notification *notification = new engine::Notification(notification_samson_worker_check_finish_tasks);
            engine::Engine::shared()->notify( notification, check_finish_tasks_period );
            
        }
        
        
        // Send a "hello" command message just to notify the controller about me
        Packet *p = new Packet( Message::Command );
        network::Command *command = p->message->mutable_command();
        command->set_command("hello");
        network->sendToController(p);
        
        
    }
    
    
    /* ****************************************************************************
     *
     * run -
     */
    
    void SamsonWorker::sendFilesMessage()
    {
        Packet*           p = new Packet(Message::Command);
        network::Command* c = p->message->mutable_command();
        
        // This message is not critical - to be thrown away if worker not connected
        p->disposable = true;
        
        c->set_command( "ls" );
        p->message->set_delilah_id( 0 ); // At the moment no sence at the controller
        //copyEnviroment( &environment , c->mutable_environment() );
        network->sendToController( p );
    }
    
    
    /* ****************************************************************************
     *
     * SamsonWorker::sendWorkerStatus -
     */
    void SamsonWorker::sendWorkerStatus(void)
    {
        
        Packet*                  p  = new Packet(Message::WorkerStatus);
        
        // This message is not critical - to be thrown away if worker not connected
        p->disposable = true;
        
        // Include generic information about this worker
        std::ostringstream info_str;
        getInfo( info_str );
        p->message->set_info(info_str.str() );
        
        // Send the message
        network->sendToController( p );
        
    }
    
    /* ****************************************************************************
     *
     * SamsonWorker::receive -
     */
    void SamsonWorker::receive( Packet* packet )
    {
        LM_T(LmtNodeMessages, ("SamsonWorker received %s from endpoint %d" , packet->str().c_str(), packet->fromId));
        
        int fromId = packet->fromId;
        Message::MessageCode msgCode = packet->msgCode;
        
        if( msgCode == Message::PushBlock )
        {
            if( !packet->message->has_push_block()  )
            {
                if ( packet->buffer )
                    engine::MemoryManager::shared()->destroyBuffer( packet->buffer );
                
                LM_W(("Internal error. Received a push block message without the push_object included"));
                
                return;
            }
            
            if ( !packet->buffer )
            {
                LM_W(("Internal error. Received a push block message without a buffer of data"));
                return;
            }
            
            
            // Tmp list with the created block
            stream::BlockList my_list( au::str("TmpList for PushBlock") );
            my_list.createBlock( packet->buffer );
            
            
            // Push the packet to a particular stream-queue
            for ( int i = 0 ; i < packet->message->push_block().queue_size() ; i++)
            {
                std::string queue = packet->message->push_block().queue(i);
                streamManager->addBlocks( queue , &my_list  );
            }
            
            // Send a message back if delilah_id is > 0
            
            if( packet->message->delilah_id() > 0)
            {
                Packet *p = new Packet( Message::PushBlockResponse );
                network::PushBlockResponse *pbr = p->message->mutable_push_block_response();
                pbr->mutable_request()->CopyFrom( packet->message->push_block() );
                
                p->message->set_delilah_id( packet->message->delilah_id()  );
                
                network->send( packet->fromId , p );
                
            }
            return;
        }
        
        if (msgCode == Message::WorkerTask)
        {
            // A packet with a particular command is received (controller expect to send a confirmation message)
            LM_T(LmtTask, ("Got a WorkerTask message"));
            
            // add task to the task manager
            taskManager.addTask( packet->message->worker_task() );
            return;
        }
        
        if (msgCode == Message::WorkerTaskKill)
        {
            // A packet with a particular command is received (controller expect to send a confirmation message)
            LM_T(LmtTask, ("Got a WorkerTaskKill message"));
            
            // add task to the task manager
            taskManager.killTask( packet->message->worker_task_kill() );
            return;
        }
        
        // List of local file ( remove unnecessary files )
        if (msgCode == Message::CommandResponse)
        {
            processListOfFiles( packet->message->command_response().queue_list() );
            return;
        }
        
        
        // Load data files to be latter confirmed to controller
        if (msgCode == Message::UploadDataFile)
        {
            loadDataManager.addUploadItem(fromId, packet->message->upload_data_file(), packet->message->delilah_id() , packet->buffer );
            return;
        }
        
        // Download data files
        if (msgCode == Message::DownloadDataFile)
        {
            loadDataManager.addDownloadItem(fromId, packet->message->download_data_file() , packet->message->delilah_id() );
            return;
        }
        
        /**
		 Data packets go directly to the DataBuffer to be joined and flushed to disk
		 DataManager is notifyed when created a new file or finish everything
         */
        
        if( msgCode == Message::WorkerDataExchange )
        {
            // New data packet for a particular queue inside a particular task environment
            
            size_t task_id = packet->message->data().task_id();
            network::WorkerDataExchange data = packet->message->data();
            taskManager.addBuffer( task_id , data, packet->buffer );
            
            return;
        }
        
        /**
		 Data Close message is sent to notify that no more data will be generated
		 We have to wait for "N" messages ( one per worker )
         */
        
        if( msgCode == Message::WorkerDataExchangeClose )
        {
            
            size_t task_id = packet->message->data_close().task_id();
            int worker_from = network->getWorkerFromIdentifier( fromId );
            
            taskManager.finishWorker( worker_from , task_id );
            
            return;
        }
        
        if( msgCode == Message::PopQueue )
        {
            
            size_t delilah_id = packet->message->delilah_id();
            streamManager->addPopQueue( packet->message->pop_queue() , delilah_id , packet->fromId );
            
            return;
        }
        
        if( msgCode == Message::WorkerCommand )
        {
            if( !packet->message->has_worker_command() )
            {
                LM_W(("Trying to run a WorkerCommand from a packet without that message"));
                return;
            }
            
            stream::WorkerCommand *workerCommand = new stream::WorkerCommand(  packet->fromId , packet->message->delilah_id() , packet->message->worker_command() );
            
            streamManager->addWorkerCommand( workerCommand );
            return;
        }
        
        LM_W((" Received a message with type %s. Just ignoring...", messageCode( msgCode )  ));
        
    }
    
    // Receive notifications
    void SamsonWorker::notify( engine::Notification* notification )
    {
        if ( notification->isName(notification_worker_update_files) )
            sendFilesMessage();
        else if ( notification->isName(notification_samson_worker_send_status_update))
            sendWorkerStatus();
        else if ( notification->isName(notification_send_to_worker) )
        {
            Packet *packet = (Packet *)notification->extractObject();
            if( packet )
            {
                // No packet could mean that other samsonWorker ( in samsonLocal mode has send the packet )
                
                int outputWorker = notification->environment.getInt("outputWorker", -1);
                
                // If outputWorker is not present, send to myself
                if ( outputWorker == -1 )
                    outputWorker = network->getWorkerId();
                
                if ( (outputWorker >= network->getNumWorkers() ) )
                {
                    LM_W(("Notification to send a packet to a worker. Deleting packet..."));
                    delete packet;
                    return;
                }
                
                // Send packet to the indicated worker
                network->sendToWorker( outputWorker , packet);
            }
            
        }
        else if ( notification->isName(notification_samson_worker_send_trace))
        {
            if ( !notification->containsObject() )
            {
                //LM_W(("SamsonWorker: Send trace without an object"));
                return;
            }
            else
            {
                //LM_M(("SamsonWorking sending a trace to all delilahs..."));
                Packet *p = (Packet*) notification->extractObject();
                network->delilahSend( this , p );
            }
        }
        else if( notification->isName( notification_samson_worker_send_packet ) )
        {
            if ( !notification->containsObject() )
            {
                LM_W(("SamsonWorker: received a notification_samson_worker_send_packet without an object"));
                return;
            }
            else
            {
                Packet *packet = (Packet *) notification->extractObject();
                
                int endpointId = notification->environment.getInt( "toId" , -1 );
                if( endpointId == -1 )
                {
                    LM_W(("No endpoint specified. Ignoring notification..."));
                    delete packet;
                }
                else
                    network->send( endpointId , packet);
            }
        }
        else
            LM_X(1, ("SamsonWorker received an unexpected notification %s", notification->getDescription().c_str()));
    }
    
    
    
    /**
	 Process the list of files removing unnecessary files
     */
    
    void SamsonWorker::processListOfFiles( const ::samson::network::QueueList& ql)
    {
        LM_T(LmtDisk, ("Starts processListOfFiles()"));
        
        // Generate list of local files ( to not remove them )
        std::set<std::string> files;
        
        // List of load_id process currently running
        std::set<size_t> load_id;
        
        // Get the elements provided by the controller
        for (int q = 0 ; q < ql.queue_size() ; q++)
        {
            const network::FullQueue& queue = ql.queue(q);
            for (int f = 0 ; f < queue.file_size() ; f++)
            {
                const network::File &file =  queue.file(f);
                files.insert( file.name() );
                //LM_T(LmtDisk,("Add queue.file()((%s) to files not to be removed", file.name().c_str()));
            }
        }
        
        // Get the files from the active tasks to not remove them
        for (int t = 0 ; t < ql.tasks_size() ; t++)
            for (int f = 0 ; f < ql.tasks(t).filename_size() ; f++)
            {
                files.insert( ql.tasks(t).filename(f)  );
                //LM_T(LmtDisk,("Add ql.tasks(%d).filename(%d)((%s) to files not to be removed", t, f, ql.tasks(t).filename(f).c_str()));
            }
        
        
        // Get the list of files generated by running tasks
        taskManager.addCurrentGeneratedFiles( files );
        
        // Get the list of active load_ids to not remove temporal files of these upload operations
        for (int t = 0 ; t < ql.load_id_size() ; t++)
            load_id.insert(ql.load_id(t));
        
        // Show on screen for debuggin...
        /*
         LM_M(("Process list of files %lu" , files.size() ));
         std::set<std::string>::iterator f;
         for (f = files.begin() ; f!= files.end() ; f++)
         std::cout << "File: " << *f << "\n";
         */
        
        // Get the list of files to be removed
        
        //std::set< std::string > remove_files;
        
        DIR *dp;
        struct dirent *dirp;
        std::string dir_path = SamsonSetup::shared()->dataDirectory;
        if((dp  = opendir( dir_path.c_str() )) == NULL) {
            
            // LOG and error to indicate that data directory cannot be access
            return;
        }
        
        while ((dirp = readdir(dp)) != NULL) {
            
            std::string path = SamsonSetup::shared()->dataDirectory + "/" + dirp->d_name;
            
            struct ::stat info;
            stat(path.c_str(), &info);
            
            
            if( S_ISREG(info.st_mode) )
            {
                
                // Get modification date to see if it was just created
                time_t now;
                time (&now);
                double age = difftime ( now , info.st_mtime );
                
                if( age > 7200 ) // Get some time to avoid cross-message between controller and worker
                {
                    
                    // Get the task from the file name
                    std::string file_name = dirp->d_name;
                    size_t pos = file_name.find("_");
                    size_t task = 0;
                    if( pos != std::string::npos )
                        task = atoll( file_name.substr( 0 , pos ).c_str() );
                    
                    if( files.find( file_name ) == files.end() )
                    {
                        LM_T(LmtDisk,("Detected ancient (> 7200) file(%s) not present in active list", file_name.c_str()));
                        // If the file is not in the list remove this
                        std::string file_name_abs = dir_path + "/" + file_name;
                        
                        if( file_name.substr( 0 , 14 ) == "file_updaload_" )
                        {
                            // Exception: temporal upload files
                            size_t pos = file_name.find("_" , 14);
                            size_t _load_id = atoll( file_name.substr(14 , pos).c_str() );
                            if( load_id.find(_load_id ) == load_id.end() )
                            {
                                LM_T(LmtDisk,("Removing file %s since the id (%lu) is not in the list of active load operations (size:%d)", file_name.c_str() , _load_id , load_id.size() ));
                                //remove_files.insert( dirp->d_name );
                                
                                int c = ::unlink( file_name_abs.c_str() );
                                if( c != 0 )
                                {
                                    LM_E(("Error removing file '%s', errno:%d\n", file_name_abs.c_str(), errno));
                                }
                                else
                                {
                                    LM_T( LmtDisk , ("DiskManager: Removed file %s", file_name.c_str() ));
                                }
                            }
                        }
                        else
                        {
                            LM_T(LmtDisk,("Remove file %s since it is not in any queue", file_name.c_str()));
                            //remove_files.insert( dirp->d_name );
                            int c = ::unlink( file_name_abs.c_str() );
                            if( c != 0 )
                            {
                                LM_E(("Error removing file '%s', errno:%d\n", file_name_abs.c_str(), errno));
                            }
                            else
                            {
                                LM_T( LmtDisk , ("DiskManager: Removed file %s", file_name.c_str() ));
                            }
                        }
                    }
                }
            }
        }
        closedir(dp);
        
        
        
        // Remove the selected files
        //for ( std::set< std::string >::iterator f = remove_files.begin() ; f != remove_files.end() ; f++)
        //{
        // Add a remove opertion to the engine ( target 0 means no specific listener to be notified )
        // Remove the file
        //engine::DiskOperation * operation =  engine::DiskOperation::newRemoveOperation(  SamsonSetup::dataFile(*f) , 0 );
        //engine::DiskManager::shared()->add( operation );
        //}
        LM_T(LmtDisk, ("Ends processListOfFiles()"));
        
        
    }
    
    void SamsonWorker::logActivity( std::string log)
    {
        activityLog.push_back( WorkerLog(log) );
        
        while( activityLog.size() > 100 )
            activityLog.pop_front();
        
    }
    
    // Get information for monitorization
    void SamsonWorker::getInfo( std::ostringstream& output)
    {
        // Engine
        engine::Engine::shared()->getInfo( output );
        
        // Engine system
        engine::getInfo(output);
        
        // Modules manager
        ModulesManager::shared()->getInfo( output );
        
        // Worker task manager
        taskManager.getInfo(output);
        
        // Block manager
        stream::BlockManager::shared()->getInfo( output );
        
        // Queues manager information
        streamManager->getInfo(output);
        
        // Network
        network->getInfo( output );
        
        
        // Activity log
        au::xml_open(output, "activity");
        
        std::list < WorkerLog >::iterator it_activityLog;
        for ( it_activityLog = activityLog.begin() ; it_activityLog != activityLog.end() ; it_activityLog++)
            it_activityLog->getInfo( output );
        
        au::xml_close(output, "activity");
    }
    
    
    
}
