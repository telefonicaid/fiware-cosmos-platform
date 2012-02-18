#include <iostream>                     // std::cout ...

#include "logMsg/logMsg.h"                     // lmInit, LM_*
#include "logMsg/traceLevels.h"                // Trace Levels
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>

#include "au/CommandLine.h"                // CommandLine
#include "au/string.h"						// au::Format
#include "au/time.h"                        // au::todayString
#include "au/ThreadManager.h"

#include "engine/Notification.h"            // engine::Notification

#include "samson/common/SamsonSetup.h"                  // samson::SamsonSetup
#include "samson/common/Macros.h"                     // EXIT, ...
#include "samson/common/SamsonSetup.h"				// samson::SamsonSetup
#include "samson/common/Info.h"                     // samson::Info

#include "samson/module/samsonVersion.h"  // SAMSON_VERSION

#include "samson/network/Message.h"                    // Message
#include "samson/network/Packet.h"                     // samson::Packet
#include "samson/network/NetworkInterface.h"                    // NetworkInterface
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

#include "samson/module/ModulesManager.h"   // samson::ModulesManager

#include "samson/network/NetworkInterface.h"    // samson::NetworkInterface

#include "samson/worker/WorkerCommand.h"           // samson::stream::WorkerCommand

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
    SamsonWorker::SamsonWorker( NetworkInterface* _network )
    {
        network = _network;
        
        // Init the stream manager
        streamManager = new stream::StreamManager(this);

        // Init worker command manager
        workerCommandManager = new WorkerCommandManager(this);
        
        // Get initial time
        gettimeofday(&init_time, NULL);
                
        // Set me as the packet receiver interface
        network->setReceiver(this);

        // Random initialization
        srand( time(NULL));
                
        // Listen this notification to send packets
        listen( notification_samson_worker_send_packet );
        
        // Notification to update state
        listen( notification_update_status );
        {
            int update_period = samson::SamsonSetup::shared()->getInt("general.update_status_period" );
            engine::Notification *notification = new engine::Notification(notification_update_status);
            engine::Engine::shared()->notify( notification, update_period );
        }        
        
        {
            int check_finish_tasks_period = samson::SamsonSetup::shared()->getInt("worker.period_check_finish_tasks" );
            engine::Notification *notification = new engine::Notification(notification_samson_worker_check_finish_tasks);
            engine::Engine::shared()->notify( notification, check_finish_tasks_period );
            
        }
        
        
    }
    
    
    void SamsonWorker::reset_worker( size_t worker_id )
    {
        // reset BlockManager ans assign the new worker id
        stream::BlockManager::shared()->resetBlockManager( worker_id );
        
    }
    
    /* ****************************************************************************
     *
     * SamsonWorker::receive -
     */
    void SamsonWorker::receive( Packet* packet )
    {
        LM_T(LmtNetworkNodeMessages, ("SamsonWorker received %s " , packet->str().c_str()));
        
        //int fromId = packet->fromId;
        
        Message::MessageCode msgCode = packet->msgCode;
        
        
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
                    LM_M(( "Connected worker %lu\n", worker_id ));
                }
                
                if( packet->message->network_notification().has_disconnected_worker_id() )
                {
                    size_t worker_id = packet->message->network_notification().disconnected_worker_id();
                    LM_M(( "Disconnected worker %lu\n", worker_id ));
                }
                
                if( packet->message->network_notification().has_connected_delilah_id() )
                {
                    size_t worker_id = packet->message->network_notification().connected_delilah_id();
                    LM_M(( "Connected delilah %lu\n", worker_id ));
                }
                
                if( packet->message->network_notification().has_disconnected_delilah_id() )
                {
                    size_t worker_id = packet->message->network_notification().disconnected_delilah_id();
                    LM_M(( "Disconnected delilah %lu\n", worker_id ));
                }

            }
            
            return;
        }        
        
        
        // --------------------------------------------------------------------
        // StatusReport
        // --------------------------------------------------------------------
        
        if( msgCode == Message::StatusReport )
        {
            //LM_M(("Recieved status report message from %s" , packet->from.str().c_str() ));
            return;
        }
        
        
        // --------------------------------------------------------------------
        // push messages
        // --------------------------------------------------------------------
        
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
            
            if( packet->message->delilah_component_id() > 0)
            {
                Packet *p = new Packet( Message::PushBlockResponse );
                network::PushBlockResponse *pbr = p->message->mutable_push_block_response();
                pbr->mutable_request()->CopyFrom( packet->message->push_block() );
                
                p->message->set_delilah_component_id( packet->message->delilah_component_id()  );
                
                // Direction of the message ( answer to who send you the message )
                p->to = packet->from;
                
                network->send( p );
            }
            return;
        }
             
        if( msgCode == Message::PopQueue )
        {
            
            size_t delilah_id = packet->from.id;
            size_t delilah_component_id = packet->message->delilah_component_id();
            
            streamManager->addPopQueue( packet->message->pop_queue() , delilah_id , delilah_component_id  );
            
            return;
        }
        
        if( msgCode == Message::WorkerCommand )
        {
            if( !packet->message->has_worker_command() )
            {
                LM_W(("Trying to run a WorkerCommand from a packet without that message"));
                return;
            }
            
            size_t delilah_id = packet->from.id;
            size_t delilah_component_id = packet->message->delilah_component_id();
            
            WorkerCommand *workerCommand = new WorkerCommand(  delilah_id , delilah_component_id , packet->message->worker_command() );
            workerCommandManager->addWorkerCommand( workerCommand );
            return;
        }
        
        LM_W((" Received a message with type %s. Just ignoring...", messageCode( msgCode )  ));
        
    }
    
    // Receive notifications
    void SamsonWorker::notify( engine::Notification* notification )
    {
        if ( notification->isName(notification_update_status))
        {
            // Create a xml version of monitorization ( common to all delilahs )
            std::ostringstream info_str;
            getInfo( info_str );
            
            // Get vector of connected delilahs
            std::vector<size_t> delilahs = network->getDelilahIds();
            std::vector<size_t> workers  = network->getWorkerIds();

            // Send this message to all delilahs
            for ( size_t i = 0 ; i < delilahs.size() ; i++ )
            {
                Packet* p  = new Packet( Message::StatusReport );
                
                // This message is not critical - to be thrown away if worker not connected
                p->disposable = true;
                
                // Include generic information about this worker
                p->message->set_info( info_str.str() );
                
                // Packet direction
                p->to.node_type = DelilahNode;
                p->to.id = delilahs[i];
                
                // Send this message to all delilahs connected
                network->send( p );
            }
            
            // Send this message to all workers ( even me ;) )
            for ( size_t i = 0 ; i < workers.size() ; i++ )
            {
                Packet* p  = new Packet( Message::StatusReport );
                
                // This message is not critical - to be thrown away if worker not connected
                p->disposable = true;
                
                // Include generic information about this worker
                p->message->set_info( info_str.str() );
                
                // Packet direction
                p->to.node_type = WorkerNode;
                p->to.id = workers[i];
                
                // Send this message to all delilahs connected
                network->send( p );
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
                network->send( packet );
            }
        }
        else
            LM_X(1, ("SamsonWorker received an unexpected notification %s", notification->getDescription().c_str()));
    }
    
    std::string SamsonWorker::getRESTInformation( ::std::string in )
    {
        std::ostringstream output;
        output << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
        output << "<!-- SAMSON Rest interface -->\n";
        
        output << "<samson>\n";
        
        // Get the path components
        std::vector<std::string> path_components = au::split( in , '/' );

        if ( ( path_components.size() < 2 ) || (path_components[0] != "samson") )
            au::xml_simple(output, "message", "Error. Only /samson/path requests are valid" );
        
        else if( path_components[1] == "version" )
            au::xml_simple(output, "version", au::str("SAMSON v %s" , SAMSON_VERSION ) );
        else if( path_components[1] == "state" )
        {
            if( path_components.size() < 4 )
                au::xml_simple(output, "message", au::str("Error: format /samson/state/queue/key" ) );
            else
                output<< streamManager->getState( path_components[2] , path_components[3].c_str() );
            
        }
        else if( path_components[1] == "cluster" )
            network->getInfo( output , "cluster" );
        else
            au::xml_simple(output, "message", au::str("Error: Unkown path component '%s'\n" , path_components[1].c_str() ) );

        output << "\n</samson>\n";
        
        return output.str();
        
    }
    
    void SamsonWorker::logActivity( std::string log)
    {
        activityLog.push_back( WorkerLog(log) );
        
        while( activityLog.size() > 100 )
            activityLog.pop_front();
        
    }
    
    // Get information for monitoring
    void SamsonWorker::getInfo( std::ostringstream& output)
    {
        // Engine
        engine::Engine::shared()->getInfo( output );
        
        // Engine system
        samson::getInfoEngineSystem(output, network);
        
        // Modules manager
        ModulesManager::shared()->getInfo( output );
                
        // Block manager
        stream::BlockManager::shared()->getInfo( output );
        
        // Queues manager information
        streamManager->getInfo(output);

        
        // WorkerCommandManager
        workerCommandManager->getInfo(output);
        
        // Network
        network->getInfo( output , "main" );
        
        
        // Activity log
        au::xml_open(output, "activity");
        
        std::list < WorkerLog >::iterator it_activityLog;
        for ( it_activityLog = activityLog.begin() ; it_activityLog != activityLog.end() ; it_activityLog++)
            it_activityLog->getInfo( output );
        
        au::xml_close(output, "activity");
    }
    
    void SamsonWorker::evalCommand( std::string command )
    {
        au::CommandLine cmdLine;
        cmdLine.parse(command);

        if( cmdLine.get_num_arguments() == 0 )
            return;
        
        std::string main_command = cmdLine.get_argument(0);
        
        if ( main_command == "quit" )
            quitConsole();

        if ( main_command == "threads" )
            writeOnConsole( au::ThreadManager::shared()->str() );
        
        if ( main_command == "cluster" )
            writeOnConsole( network->cluster_command(command) );
        
        // More command to check what is going on inside a worker
        
    }

    std::string SamsonWorker::getPrompt()
    {
        return "SamsonWorker> ";
    }
    
    
}
