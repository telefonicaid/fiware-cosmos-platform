#include <iconv.h>
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
#include "samson/common/MessagesOperations.h"

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
        
        // Listen this notification to send traces
        listen( notification_samson_worker_send_trace );
        
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
        
        // --------------------------------------------------------------------
        // pop messages
        // --------------------------------------------------------------------
             
        if( msgCode == Message::PopQueue )
        {
            
            size_t delilah_id = packet->from.id;
            size_t delilah_component_id = packet->message->delilah_component_id();
            
            streamManager->addPopQueue( packet->message->pop_queue() , delilah_id , delilah_component_id  );
            
            return;
        }
        
        // --------------------------------------------------------------------
        // Worker commands
        // --------------------------------------------------------------------
        
        if( msgCode == Message::WorkerCommand )
        {
            if( !packet->message->has_worker_command() )
            {
                LM_W(("Trying to run a WorkerCommand from a packet without that message"));
                return;
            }
            
            size_t delilah_id = packet->from.id;
            size_t delilah_component_id = packet->message->delilah_component_id();
            
            WorkerCommand *workerCommand = new WorkerCommand(  delilah_id 
                                                             , delilah_component_id 
                                                             , packet->message->worker_command() 
                                                             );
            
            // If it comes with a buffer, set the buffer property
            if( packet->buffer )
                workerCommand->setBuffer( packet->buffer );
            
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
        else if ( notification->isName( notification_samson_worker_send_trace ) )
        {
            std::string message = notification->environment.get("message","No message comming with trace-notification" );
            std::string context = notification->environment.get("context","?" );
            std::string type    = notification->environment.get("type","message" );
            
            sendTrace( type , context , message );
        }
        else
            LM_X(1, ("SamsonWorker received an unexpected notification %s", notification->getDescription().c_str()));
    }
    


/* ****************************************************************************
*
* toUTF8 - 
*/
static char* toUTF8(char* in, size_t* outLenP)
{
    static iconv_t  icDesc = (iconv_t) -1;

    if (icDesc == (iconv_t) -1)
    {
        icDesc   = iconv_open("UTF-8", "ISO-8859-1");
        if (icDesc == (iconv_t) -1)
        {
            LM_E(("error opening utf8 converter: %s", strerror(errno)));
            return strdup("error opening utf8 converter");
        }
    }


    int             outSize      = 4 * strlen(in);
    char*           out          = (char*) calloc(1, outSize);
    char*           outStart     = out;
    size_t          outbytesLeft = outSize;
    size_t          inbytesLeft  = strlen(in);
    int             nb           = 0;

    LM_T(LmtRest, ("Converting string of %d bytes to utf8", strlen(in)));
    nb = iconv(icDesc, (char**) &in, &inbytesLeft, (char**) &out, &outbytesLeft);
    if (nb == -1)
    {
        free((void*) outStart);
        LM_RE(NULL, ("iconv: %s", strerror(errno)));
    }

    LM_T(LmtRest, ("Got a utf8 string of %d bytes as result", outSize - outbytesLeft));
    nb = outSize - outbytesLeft;
    if (outLenP != NULL)
        *outLenP = nb;

    return outStart;
}



    //
    // SamsonWorker::getRESTInformation - 
    //
    std::string SamsonWorker::getRESTInformation( ::std::string in )
    {
        std::ostringstream header;
        std::ostringstream data;

        LM_T(LmtRest, ("Incoming REST request"));
        
        data << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
        data << "<!-- SAMSON Rest interface -->\r\n";
        
        data << "<samson>\r\n";
        
        // Get the path components
        std::vector<std::string> path_components = au::split( in , '/' );

        if ((path_components.size() < 2) || (path_components[0] != "samson"))
            au::xml_simple(data, "message", "Error. Only /samson/path requests are valid" );
        else if (path_components[1] == "version")
            au::xml_simple(data, "version", au::str("SAMSON v %s" , SAMSON_VERSION ) );
        else if (path_components[1] == "utftest")
        {
            char     in[32];

            int ix = 0;

            memset(in, 0, 32);
            in[ix++] = '-';
            in[ix++] = 0xA2;
            in[ix++] = 0xA3;
            in[ix++] = 0xA4;
            in[ix++] = '-';
            in[ix++] = 0;

            au::xml_simple(data, "utf8", au::str(in));
        }
        else if( path_components[1] == "state" )
        {
            if( path_components.size() < 4 )
                au::xml_simple(data, "message", au::str("Error: format /samson/state/queue/key" ) );
            else
                data << streamManager->getState( path_components[2] , path_components[3].c_str() );
            
        }
        else if( path_components[1] == "cluster" )
            network->getInfo( data , "cluster" );
        else if (path_components[1] == "traceSet")
        {
            lmTraceSet((char*) path_components[2].c_str());
            LM_M(("Changed tracelevels to '%s'", path_components[2].c_str()));
        }
        else if (path_components[1] == "traceGet")
        {
            char traceLevels[1024];
            lmTraceGet(traceLevels);
            au::xml_simple(data, "message", au::str("Tracelevels: '%s'", traceLevels));
        }
        else if (path_components[1] == "traceOff")
        {
            lmTraceSet(NULL);
            LM_M(("Turned all trace levels off"));
        }
        else if (path_components[1] == "traceAdd")
        {
            lmTraceAdd((char*) path_components[2].c_str());
            LM_M(("Added tracelevel '%s'", path_components[2].c_str()));
        }
        else if (path_components[1] == "traceDel")
        {
            lmTraceSub((char*) path_components[2].c_str());
            LM_M(("Removed tracelevel '%s'", path_components[2].c_str()));
        }
        else
            au::xml_simple(data, "message", au::str("Error: Unknown path component '%s'\n" , path_components[1].c_str() ) );

        data << "\r\n</samson>\r\n\r\n";
        
        int dataLen = data.str().length();

        header << "HTTP/1.1 200 OK\r\n";
        header << "Content-Type:   \"application/xml; charset=utf-8\"\r\n";
        header << "Content-Length: " << dataLen << "\r\n";
        header << "\r\n";

        std::ostringstream output;

        char* out = toUTF8((char*) data.str().c_str(), NULL);
        if (out == NULL)
        {
            out = strdup("error converting data to UTF8"); // free later ...
            LM_E((out));
        }

        output << header.str() << out;
        free(out);

        return output.str();
    }
    
    
    // Get information for monitoring
    void SamsonWorker::getInfo( std::ostringstream& output)
    {
        // Engine
        //engine::Engine::shared()->getInfo( output );
        
        // Engine system
        //samson::getInfoEngineSystem(output, network);
        
        // Modules manager
        //ModulesManager::shared()->getInfo( output );
                
        // Block manager
        //stream::BlockManager::shared()->getInfo( output );
        
        // Queues manager information
        streamManager->getInfo(output);

        
        // WorkerCommandManager
        //workerCommandManager->getInfo(output);
        
        // Network
        //network->getInfo( output , "main" );
        
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
    
    void SamsonWorker::sendTrace( std::string type , std::string context , std::string message  )
    {
        
        
        // Send message to all delilahs
        std::vector<size_t> delilahs = network->getDelilahIds();
        
        for ( size_t i = 0 ; i < delilahs.size() ; i++ )
        {
            
            Packet * p = new Packet( Message::Alert );
            
            network::Alert * alert = p->message->mutable_alert();
            alert->set_type(type);
            alert->set_context(context);
            alert->set_text( message );
                        
            p->message->set_delilah_component_id( (size_t)-1 ); // This message do not belong to the operation executing it
            
            // Direction of this paket
            p->to.node_type = DelilahNode;
            p->to.id = delilahs[i];
            
            // Send packet
            network->send( p );
        }            
        
    }
    
    network::Collection* SamsonWorker::getWorkerCollection( Visualization* visualization )
    {
        network::Collection* collection = new network::Collection();
        collection->set_name("workers");
        
        network::CollectionRecord* record = collection->add_record();            

        // Common type to joint queries ls_workers -group type
        ::samson::add( record , "Type" , "worker" , "different" );
        
        ::samson::add( record , "Mem used" , engine::MemoryManager::shared()->getUsedMemory() , "f=uint64,sum" );
        ::samson::add( record , "Mem total" , engine::MemoryManager::shared()->getMemory() , "f=uint64,sum" );

        ::samson::add( record , "Cores used" , engine::ProcessManager::shared()->getNumUsedCores() , "f=uint64,sum" );
        ::samson::add( record , "Cores total" , engine::ProcessManager::shared()->getNumCores() , "f=uint64,sum" );
        
        ::samson::add( record , "#Disk ops" , engine::DiskManager::shared()->getNumOperations() , "f=uint64,sum" );

        ::samson::add( record , "Disk in B/s" , engine::DiskManager::shared()->get_rate_in() , "f=uint64,sum" );
        ::samson::add( record , "Disk out B/s" , engine::DiskManager::shared()->get_rate_out() , "f=uint64,sum" );

        ::samson::add( record , "Net in B/s" , network->get_rate_in() , "f=uint64,sum" );
        ::samson::add( record , "Net out B/s" , network->get_rate_out() , "f=uint64,sum" );
        
        
        return collection;
    }

    
}
