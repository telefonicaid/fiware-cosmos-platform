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
                
        // Add samsonWorker as lister to send traces to delilahs
        listen(notification_samson_worker_send_trace );
        
        // add to listen messages to send a packet to a worker
        listen( notification_send_to_worker );
        
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
        
        
        /*
        // Send a "hello" command message just to notify the controller about me
        Packet *p = new Packet( Message::Command );
        network::Command *command = p->message->mutable_command();
        command->set_command("hello");
        network->sendToController(p);
        */
         
        
    }
    
    

    
    /* ****************************************************************************
     *
     * SamsonWorker::receive -
     */
    void SamsonWorker::receive( Packet* packet )
    {
        LM_T(LmtNodeMessages, ("SamsonWorker received %s from endpoint %d" , packet->str().c_str(), packet->fromId));
        
        //int fromId = packet->fromId;
        
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
        if ( notification->isName(notification_update_status))
        {
            Packet* p  = new Packet(Message::StatusReport);
            
            // This message is not critical - to be thrown away if worker not connected
            p->disposable = true;
            
            // Include generic information about this worker
            std::ostringstream info_str;
            getInfo( info_str );
            p->message->set_info(info_str.str() );
            
            // Send this message to all delilahs connnected
            network->delilahSend( this , p );

            // Collect some information an print status...
            int num_processes = engine::ProcessManager::shared()->public_num_proccesses;
            int max_processes = engine::ProcessManager::shared()->public_max_proccesses;

            size_t used_memory = engine::MemoryManager::shared()->public_used_memory;
            size_t max_memory = engine::MemoryManager::shared()->public_max_memory;

            size_t disk_read_rate = (size_t) engine::DiskManager::shared()->diskStatistics.item_read.rate.getRate();
            size_t disk_write_rate = (size_t) engine::DiskManager::shared()->diskStatistics.item_write.rate.getRate();
            
            LM_M(("Status [ P %s M %s D_in %s D_out %s ]"
                  , au::percentage_string( num_processes, max_processes ).c_str()
                  , au::percentage_string(used_memory, max_memory).c_str()
                  , au::str( disk_read_rate , "Bs" ).c_str()
                  , au::str( disk_write_rate , "Bs" ).c_str()
                  ));
            
        }
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
