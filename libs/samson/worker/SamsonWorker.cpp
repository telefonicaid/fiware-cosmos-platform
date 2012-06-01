#include <iconv.h>
#include <iostream>                               // std::cout ...
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>

#include "json.h"

#include "logMsg/logMsg.h"                        // lmInit, LM_*
#include "logMsg/traceLevels.h"                   // Trace Levels

#include "au/CommandLine.h"                       // CommandLine
#include "au/string.h"                            // au::Format
#include "au/time.h"                              // au::todayString
#include "au/ThreadManager.h"
#include "au/tables/pugixml.hpp"                  // pugixml

#include "au/log/LogToServer.h"

#include "engine/Notification.h"                  // engine::Notification

#include "samson/common/SamsonSetup.h"            // samson::SamsonSetup
#include "samson/common/Macros.h"                 // EXIT, ...
#include "samson/common/SamsonSetup.h"            // samson::SamsonSetup
#include "samson/common/MessagesOperations.h"

#include "samson/module/samsonVersion.h"          // SAMSON_VERSION

#include "samson/network/Message.h"               // Message
#include "samson/network/Packet.h"                // samson::Packet
#include "samson/network/NetworkInterface.h"      // NetworkInterface

#include "engine/MemoryManager.h"                 // samson::SharedMemory

#include "engine/Engine.h"                        // engine::Engine
#include "engine/DiskOperation.h"                 // samson::DiskOperation
#include "engine/DiskManager.h"                   // Notifications
#include "engine/ProcessManager.h"                // engine::ProcessManager
#include "samson/isolated/SharedMemoryManager.h"  // engine::SharedMemoryManager

#include "samson/common/MemoryTags.h"             // MemoryInput , MemoryOutputNetwork ,...

#include "samson/stream/Block.h"                  // samson::stream::Block
#include "samson/stream/BlockList.h"              // samson::stream::BlockList
#include "samson/stream/BlockManager.h"           // samson::stream::BlockManager

#include "samson/module/ModulesManager.h"         // samson::ModulesManager

#include "samson/network/NetworkInterface.h"      // samson::NetworkInterface

#include "samson/delilah/WorkerCommandDelilahComponent.h"

#include "samson/worker/WorkerCommand.h"          // samson::stream::WorkerCommand
#include "samson/worker/SamsonWorker.h"           // Own interfce

#define notification_samson_worker_take_sample "notification_samson_worker_take_sample"

extern size_t delilah_random_code;     // Random code for the rest delilah client
extern int port;                       // Port where samsonWorker is started
extern int web_port;                   

namespace samson {
    
    
    /* ****************************************************************************
     *
     * Constructor
     */
    
    SamsonWorker::SamsonWorker( NetworkInterface* _network ) : samson_worker_samples( this )
    {
        network = _network;
        
        // Auto-client init in first REST connection
        delilah = NULL;
        
        // Init the stream manager
        streamManager = new stream::StreamManager(this);
        LM_T(LmtCleanup, ("Created streamManager: %p", streamManager));
        
        // Init worker command manager
        workerCommandManager = new WorkerCommandManager(this);
        LM_T(LmtCleanup, ("Created workerCommandManager: %p", workerCommandManager));
        
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

        // Take samples for the REST interface
        listen(notification_samson_worker_take_sample);
        {
            engine::Notification *notification = new engine::Notification(notification_samson_worker_take_sample);
            engine::Engine::shared()->notify( notification, 1 );
        }
        
        
        // Run REST interface
        rest_service = new au::network::RESTService( web_port  , this );
        rest_service->initRESTService();
        
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
                } else if( packet->message->network_notification().has_disconnected_worker_id() )
                {
                    size_t worker_id = packet->message->network_notification().disconnected_worker_id();
                    LM_M(( "Disconnected worker %lu\n", worker_id ));
                } else if( packet->message->network_notification().has_connected_delilah_id() )
                {
                    size_t delilah_id = packet->message->network_notification().connected_delilah_id();
                    LM_M(( "Connected delilah %lu\n", delilah_id ));
                } else if( packet->message->network_notification().has_disconnected_delilah_id() )
                {
                    size_t delilah_id = packet->message->network_notification().disconnected_delilah_id();
                    LM_M(( "Disconnected delilah %lu\n", delilah_id ));
                    streamManager->notify_delilah_disconnection( delilah_id );
                    
                }
                else
                    LM_W(("Notification from network without required information"));
                
            }
            else
                LM_W(("Notification from network without required information"));
            
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
                LM_W(("Internal error. Received a push block message without the push_object included"));
                
                return;
            }
            
            // Get a pointer to the buffer ( retained inside packet )
            engine::Buffer* buffer = packet->getBuffer();
            
            if ( !buffer )
            {
                LM_W(("Internal error. Received a push block message without a buffer of data"));
                return;
            }
            
            // Push the packet to a particular stream-queue
            for ( int i = 0 ; i < packet->message->push_block().queue_size() ; i++)
            {
                std::string queue = packet->message->push_block().queue(i);
                streamManager->addBuffer ( queue , buffer );
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
                
                p->release();
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
            
            std::string worker_command_id = au::str( "%s_%lu" , au::code64_str(delilah_id).c_str() , delilah_component_id );
            
            WorkerCommand *workerCommand = new WorkerCommand(  worker_command_id
                                                             , delilah_id 
                                                             , delilah_component_id 
                                                             , packet->message->worker_command() 
                                                             );
            
            // If it comes with a buffer, set the buffer property
            workerCommand->setBuffer( packet->getBuffer() );
            
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
                
                // Release created packet
                p->release();
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
                
                // Release packet
                p->release();
            }
            
            // Some ancient samson-0.6 useful Status information
            // Collect some information and print status...
            int num_processes = engine::ProcessManager::shared()->public_num_proccesses;
            int max_processes = engine::ProcessManager::shared()->public_max_proccesses;
            
            size_t used_memory = engine::MemoryManager::shared()->public_used_memory;
            size_t max_memory = engine::MemoryManager::shared()->public_max_memory;
            
            size_t disk_read_rate = (size_t) engine::DiskManager::shared()->get_rate_in();
            size_t disk_write_rate = (size_t) engine::DiskManager::shared()->get_rate_out();
            
            size_t network_read_rate = (size_t) network->get_rate_in();
            size_t network_write_rate = (size_t) network->get_rate_out();
            
            
            LM_M(("Status [ P %s M %s D_in %s D_out %s N_in %s N_out %s ]"
                  , au::str_percentage( num_processes, max_processes ).c_str()
                  , au::str_percentage(used_memory, max_memory).c_str()
                  , au::str( disk_read_rate , "Bs" ).c_str()
                  , au::str( disk_write_rate , "Bs" ).c_str()
                  , au::str( network_read_rate , "Bs" ).c_str()
                  , au::str( network_write_rate , "Bs" ).c_str()
                  ));
            
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
                Packet *packet = (Packet *) notification->getObject();
                network->send( packet );
            }
        }
        else if ( notification->isName( notification_samson_worker_send_trace ) )
        {
            std::string message = notification->environment.get("message","No message coming with trace-notification" );
            std::string context = notification->environment.get("context","?" );
            std::string type    = notification->environment.get("type","message" );
            
            sendTrace( type , context , message );
        }
        else if ( notification->isName( notification_samson_worker_take_sample ) )
        {
            // Take samples every second to check current status
            samson_worker_samples.take_samples();
        }
        else
            LM_X(1, ("SamsonWorker received an unexpected notification %s", notification->getDescription().c_str()));
    }
    
    std::string getFormatedElement( std::string name , std::string value , std::string& format )
    {
        std::ostringstream output;
        if (format == "xml")
            au::xml_simple(output, name, value );
        else if( format == "json" )
            au::json_simple(output, name, value );
        else if( format == "html" )
            output << "<h1>" << name << "</h1>" << value;
        else
            output << name << ":\n" << value;
        
        return output.str();
    }
    
    std::string getFormatedError( std::string message , std::string& format )
    {
        return  getFormatedElement( "error" , message , format );
    }
    
    void SamsonWorker::process_delilah_command( std::string delilah_command , au::network::RESTServiceCommand* command )
    {

        // Create client if not created
        if( !delilah )
        {
            delilah_random_code = au::code64_rand();
            delilah = new Delilah();
        }
        
        // Connect delilah if not connected
        if( !delilah->isConnected() )
        {
            au::ErrorManager error;
            delilah->delilah_connect("rest", "localhost", port, "anonymous", "anonymous", &error);
            
            if( error.isActivated() )
            {
                command->appendFormatedError(500, au::str("Error connecting to rest client (%s)" , error.getMessage().c_str() ) );
                LM_E(("Error connecting to rest client (%s)", error.getMessage().c_str()));
                return;
            }
        }
        
        // Wait for a fully connected client
        {
            au::Cronometer c;
            while( !delilah->isConnectionReady() )
            {
                usleep(10000);
                if( c.diffTimeInSeconds() > 1.0 )
                {
                    command->appendFormatedError(500, "Timeout connecting to REST client" );
                    LM_E(("Timeout connecting to REST client"));
                    return;
                }
            }
        }

        // Send the command
        LM_T(LmtDelilahCommand, ("Sending delilah command: '%s'", delilah_command.c_str()));
        size_t command_id = delilah->sendWorkerCommand( delilah_command , NULL );
        
        // Wait for the command to finish
        {
            au::Cronometer c;
            while( delilah->isActive(command_id) )
            {
                usleep(10000);
                if( c.diffTimeInSeconds() > 2 )
                {
                    command->appendFormatedError(500, au::str( "Timeout awaiting response from REST client (task %lu)" , command_id ));
                    LM_E(("Timeout awaiting response from REST client"));
                    return;
                }
            }
        }
        
        // Recover information
        WorkerCommandDelilahComponent* component = (WorkerCommandDelilahComponent*) delilah->getComponent( command_id );
        if( !component )
        {
            command->appendFormatedError(500, "Internal error recovering answer from REST client" );
            LM_E(("Internal error recovering answer from REST client"));
            return;
        }
        
        // Recover table from component
        au::tables::Table* table = component->getMainTable();
        
        if( !table )
        {
            command->appendFormatedError(500, "No content in answer from REST client" );
            LM_E(("No content in answer from REST client"));
            return;
        }
        
        std::string output;
        
        LM_T(LmtRest, ("appending delilah output to command: '%s'", table->str().c_str()));

        if( command->format == "xml" )
            command->append( table->str_xml() );
        else if( command->format == "json" )
            command->append( table->str_json() );
        else if( command->format == "html" )
            command->append( table->str_html() );
        else
            command->append( table->str() ); // Default non-format
        
        delete table;
        
    }
    
#define TF(b) ((b == true)? "true" : "false")
    void SamsonWorker::process_logging(au::network::RESTServiceCommand* command)
    {
        std::ostringstream  logdata;
        std::string         logCommand  = "";
        std::string         sub         = "";
        std::string         arg         = "";

        command->http_state = 200;

        if (command->path_components.size() > 2)
            logCommand = command->path_components[2];
        if (command->path_components.size() > 3)
            sub = command->path_components[3];
        if (command->path_components.size() > 4)
            arg = command->path_components[4];
        
        //
        // Treat all possible errors
        //
        
        if (logCommand == "")
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("no logging subcommand"));
        }
        else if ((logCommand != "reads") && (logCommand != "writes") && (logCommand != "traces") && (logCommand != "verbose") && (logCommand != "debug"))
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("badilogging command: '%s'", logCommand.c_str()));
        }
        else if (((logCommand == "reads") || (logCommand == "writes") || (logCommand == "debug")) && (sub != "on") && (sub != "off"))
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("bad ilogging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
        }
        else if ((logCommand == "verbose") && (sub != "get") && (sub != "set") && (sub != "off"))
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("bad logging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
        }
        else if ((logCommand == "verbose") && (sub == "set") && (arg != "0") && (arg != "1") && (arg != "2") && (arg != "3") && (arg != "4") && (arg != "5"))
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("bad logging argument for 'verbose': %s", arg.c_str()));
        }
        else if ((logCommand == "traces") && (sub != "get") && (sub != "set") && (sub != "add") && (sub != "remove") && (sub != "off") && (sub != ""))
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("bad logging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
        }
        else if ((logCommand == "traces") && ((sub != "set") || (sub != "add") || (sub != "remove")))
        {
            if (strspn(arg.c_str(), "0123456789-,") != strlen(arg.c_str()))
            {
                command->http_state = 400;
                command->appendFormatedElement("message", au::str("bad logging parameter '%s' for 'trace/%s'", arg.c_str(), sub.c_str()));
            }
        }
        

        //
        // Checking the VERB
        //
        std::string verb = command->command;
        std::string path = logCommand;

        if (sub != "") path += '/' + sub;

        if      ((path == "debug/on")      && (verb == "POST"));
        else if ((path == "debug/off")     && (verb == "POST"));

        else if ((path == "reads/on")      && (verb == "POST"));
        else if ((path == "reads/off")     && (verb == "POST"));

        else if ((path == "writes/on")     && (verb == "POST"));
        else if ((path == "writes/off")    && (verb == "POST"));

        else if ((path == "traces")        && (verb == "GET"));
        else if ((path == "traces/off")    && (verb == "POST"));
        else if ((path == "traces/set")    && (verb == "POST"));
        else if ((path == "traces/add")    && (verb == "POST"));
        else if ((path == "traces/remove") && (verb == "DELETE"));

        else if ((path == "verbose")       && (verb == "GET"));
        else if ((path == "verbose/off")   && (verb == "POST"));
        else if ((path == "verbose/set")   && (verb == "POST"));
        else
        {
            command->http_state = 404;
            command->appendFormatedElement("error", "BAD VERB");
            return;
        }

        if (command->http_state != 200)
            return;
        
        //
        // Treat the request
        //
        if (logCommand == "reads")
        {
            if (sub == "on")
            {
                process_delilah_command("wreads on", command);
                command->appendFormatedElement("reads", au::str("reads turned ON"));
            }
            else if (sub == "off")
            {
                process_delilah_command("wreads off", command);
                command->appendFormatedElement("reads", au::str("reads turned OFF"));
            }
        }
        else if (logCommand == "writes")
        {
            if (sub == "on")
            {
                process_delilah_command("wwrites on", command);
                command->appendFormatedElement("writes", au::str("writes turned ON"));
            }
            else if (sub == "off")
            {
                process_delilah_command("wwrites off", command);
                command->appendFormatedElement("writes", au::str("writes turned OFF"));
            }
        }
        else if (logCommand == "debug")
        {
            if (sub == "on")
            {
                process_delilah_command("wdebug on", command);
                command->appendFormatedElement("debug", au::str("debug turned ON"));
            }
            else if (sub == "off")
            {
                process_delilah_command("wdebug off", command);
                command->appendFormatedElement("debug", au::str("debug turned OFF"));
            }
        }
        else if (logCommand == "verbose")  // /samson/logging/verbose
        {
            if (sub == "")
                sub = "get";

            if ((sub == "set") && (arg == "0"))
                sub = "off";

            if (sub == "get")
            {
                process_delilah_command("wverbose get", command);
            }
            else if ((sub == "off") || (sub == "0"))
            {
                process_delilah_command("wverbose off", command);
                command->appendFormatedElement("verbose", au::str("verbose mode turned OFF"));
            }
            else
            {
                char delilahCommand[64];
                
                snprintf(delilahCommand, sizeof(delilahCommand), "wverbose %s", arg.c_str());
                process_delilah_command(delilahCommand, command);
                command->appendFormatedElement("verbose", au::str("verbose levels upto %s SET", arg.c_str()));
            }
        }
        else if (logCommand == "traces")
        {
            if (sub == "")
                sub = "get";

            if (sub == "set")
            {
                char delilahCommand[64];

                snprintf(delilahCommand, sizeof(delilahCommand), "wtrace set %s",  arg.c_str());
                process_delilah_command(delilahCommand, command);
                command->appendFormatedElement("trace", au::str("trace level: %s", arg.c_str()));
            }
            else if (sub == "get")    // /samson/logging/trace/get
            {
                char delilahCommand[64];

                snprintf(delilahCommand, sizeof(delilahCommand), "wtrace get");
                process_delilah_command(delilahCommand, command);
                // What is pushed back to the REST request?
            }
            else if (sub == "off")    // /samson/logging/trace/off
            {
                process_delilah_command("wtrace off", command);
                command->appendFormatedElement("trace", au::str("all trace levels turned off"));
            }
            else if (sub == "add")    // /samson/logging/trace/add
            {
                char delilahCommand[64];

                snprintf(delilahCommand, sizeof(delilahCommand), "wtrace add %s", arg.c_str());
                process_delilah_command(delilahCommand, command);
                command->appendFormatedElement("trace", au::str("added level(s) %s", arg.c_str()));
            }
            else if (sub == "remove")     // /samson/logging/trace/remove
            {
                char delilahCommand[64];

                snprintf(delilahCommand, sizeof(delilahCommand), "wtrace remove %s", arg.c_str());
                process_delilah_command(delilahCommand, command);
                command->appendFormatedElement("trace", au::str("removed level(s) %s", arg.c_str()));
            }
        }
    }

#if 0
    // ------------------------------------------------------------
    //
    // Future common check for path and verb in logging/ilogging
    //

    static bool restTraceCheck(au::network::RESTServiceCommand* command)
    {
        std::string arg = command->path_components[4];
        if (strspn(arg.c_str(), "0123456789-,") != strlen(arg.c_str()))
            return false;
        return true;
    }

    static bool restVerboseCheck(au::network::RESTServiceCommand* command)
    {
        std::string arg = command->path_components[4];

        if ((arg == "0") || (arg == "1") || (arg == "2") || (arg == "3") || (arg == "4") || (arg == "5"))
            return true;

        return false;
    }

    typedef bool (*RestCheckFunc)(au::network::RESTServiceCommand* command);
    typedef struct RestCheck
    {
        std::string    verb;
        std::string    path;
        int            components;   // Not counting '/samson/XXX' ,,,
        RestCheckFunc  checkFunc;
    } RestCheck;

    RestCheck restCheck[] = 
    {
        { "POST",   "debug/on",       2, NULL             },
        { "POST",   "debug/off",      2, NULL             },
        { "POST",   "reads/on",       2, NULL             },
        { "POST",   "reads/off",      2, NULL             },
        { "POST",   "writes/on",      2, NULL             },
        { "POST",   "writes/off",     2, NULL             },
        { "GET",    "traces",         1, NULL             },
        { "POST",   "traces/off",     2, NULL             },
        { "POST",   "traces/set",     3, restTraceCheck   },
        { "POST",   "traces/add",     3, restTraceCheck   },
        { "DELETE", "traces/remove",  3, restTraceCheck   },
        { "GET",    "verbose",        1, NULL             },
        { "POST",   "verbose/off",    2, NULL             },
        { "POST",   "verbose/set",    3, restVerboseCheck },
    };

    static bool process_logging_check(au::network::RESTServiceCommand* command)
    {
        std::string  verb = command->command;
        std::string  path = command->path_components[2];
        bool         ok   = false;

        if (command->path_components[3] != "")
            path += '/' + command->path_components[3];

        for (unsigned int ix = 0; ix < sizeof(restCheck) / sizeof(restCheck[0]); ix++)
        {
            if ((path == restCheck[ix].path) && (verb == restCheck[ix].verb) && (command->path_components.size() == restCheck[ix].components + 2))
            {
                // A match - now just lets see if the argument is OK also ...
                if (restCheck[ix].check != NULL)
                    return restCheck[ix].check(command);
                return true;
            }
        }

        return ok;
    }
#endif

    void SamsonWorker::process_ilogging(au::network::RESTServiceCommand* command)
    {
        std::ostringstream  logdata;
        std::string         logCommand  = "";
        std::string         sub         = "";
        std::string         arg         = "";

        command->http_state = 200;

        if (command->path_components.size() > 2)
            logCommand = command->path_components[2];
        if (command->path_components.size() > 3)
            sub = command->path_components[3];
        if (command->path_components.size() > 4)
            arg = command->path_components[4];
        
        //
        // Treat all possible errors
        //
        
        if (logCommand == "")
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("no ilogging subcommand"));
        }
        else if ((logCommand != "reads") && (logCommand != "writes") && (logCommand != "traces") && (logCommand != "verbose") && (logCommand != "debug"))
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("bad ilogging command: '%s'", logCommand.c_str()));
        }
        else if (((logCommand == "reads") || (logCommand == "writes") || (logCommand == "debug")) && (sub != "on") && (sub != "off"))
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("bad ilogging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
        }
        else if ((logCommand == "verbose") && (sub != "get") && (sub != "set") && (sub != "off") && (sub != ""))
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("bad ilogging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
        }
        else if ((logCommand == "verbose") && (sub == "set") && (arg != "0") && (arg != "1") && (arg != "2") && (arg != "3") && (arg != "4") && (arg != "5"))
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("bad ilogging argument for 'verbose': %s", arg.c_str()));
        }
        else if ((logCommand == "traces") && (sub != "get") && (sub != "set") && (sub != "add") && (sub != "remove") && (sub != "off") && (sub != ""))
        {
            command->http_state = 400;
            command->appendFormatedElement("message", au::str("bad ilogging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
        }
        else if ((logCommand == "traces") && ((sub != "set") || (sub != "add") || (sub != "remove")))
        {
            if (strspn(arg.c_str(), "0123456789-,") != strlen(arg.c_str()))
            {
                command->http_state = 400;
                command->appendFormatedElement("message", au::str("bad ilogging parameter '%s' for 'trace/%s'", arg.c_str(), sub.c_str()));
            }
        }
        

        //
        // Checking the VERB
        //
        std::string verb = command->command;
        std::string path = logCommand;

        if (sub != "") path += '/' + sub;

        if      ((path == "debug/on")      && (verb == "POST"));
        else if ((path == "debug/off")     && (verb == "POST"));

        else if ((path == "reads/on")      && (verb == "POST"));
        else if ((path == "reads/off")     && (verb == "POST"));

        else if ((path == "writes/on")     && (verb == "POST"));
        else if ((path == "writes/off")    && (verb == "POST"));

        else if ((path == "traces")        && (verb == "GET"));
        else if ((path == "traces/off")    && (verb == "POST"));
        else if ((path == "traces/set")    && (verb == "POST"));
        else if ((path == "traces/add")    && (verb == "POST"));
        else if ((path == "traces/remove") && (verb == "DELETE"));

        else if ((path == "verbose")       && (verb == "GET"));
        else if ((path == "verbose/off")   && (verb == "POST"));
        else if ((path == "verbose/set")   && (verb == "POST"));
        else
        {
            command->http_state = 404;
            command->appendFormatedElement("error", "BAD VERB");
            return;
        }

        if (command->http_state != 200)
            return;
        
        //
        // Treat the request
        //
        if (logCommand == "reads")
        {
            if (sub == "on")
            {
                lmReads  = true;
                command->appendFormatedElement("reads", au::str("reads turned ON"));
            }
            else if (sub == "off")
            {
                lmReads  = false;
                command->appendFormatedElement("reads", au::str("reads turned OFF"));
            }
        }
        else if (logCommand == "writes")
        {
            if (sub == "on")
            {
                lmWrites  = true;
                command->appendFormatedElement("writes", au::str("writes turned ON"));
            }
            else if (sub == "off")
            {
                lmWrites  = false;
                command->appendFormatedElement("writes", au::str("writes turned OFF"));
            }
        }
        else if (logCommand == "debug")
        {
            if (sub == "on")
            {
                lmDebug  = true;
                command->appendFormatedElement("debug", au::str("debug turned ON"));
            }
            else if (sub == "off")
            {
                lmDebug  = false;
                command->appendFormatedElement("debug", au::str("debug turned OFF"));
            }
        }
        else if (logCommand == "verbose")  // /samson/ilogging/verbose
        {
            if (sub == "")
                sub = "get";

            if ((sub == "set") && (arg == "0"))
                sub = "off";

            if (sub == "get")
            {
                int vLevel;
                
                if      (lmVerbose5 == true)  vLevel = 5;
                else if (lmVerbose4 == true)  vLevel = 4;
                else if (lmVerbose3 == true)  vLevel = 3;
                else if (lmVerbose2 == true)  vLevel = 2;
                else if (lmVerbose  == true)  vLevel = 1;
                else                          vLevel = 0;
                
                command->appendFormatedElement("verbose", au::str("verbosity level: %d", vLevel));
            }
            else
            {
                // Turn all verbose levels OFF
                lmVerbose  = false;
                lmVerbose2 = false;
                lmVerbose3 = false;
                lmVerbose4 = false;
                lmVerbose5 = false;
                
                if (sub == "off")
                    arg = "0";
                
                int verboseLevel = arg[0] - '0';
                
                // Turn on the desired verbose levels
                switch (verboseLevel)
                {
                    case 5: lmVerbose5 = true;
                    case 4: lmVerbose4 = true;
                    case 3: lmVerbose3 = true;
                    case 2: lmVerbose2 = true;
                    case 1: lmVerbose  = true;
                }
                
                command->appendFormatedElement("verbose", au::str("verbosity level: %d", verboseLevel));
            }
        }
        else if (logCommand == "traces")
        {
            if (sub == "")
                sub = "get";

            if (sub == "set")
            {
                lmTraceSet((char*) arg.c_str());
                command->appendFormatedElement("trace", au::str("trace level: %s", arg.c_str()));
            }
            else if (sub == "get")    // /samson/ilogging/trace/get
            {
                char traceLevels[1024];
                lmTraceGet(traceLevels);
                
                command->appendFormatedElement("trace", au::str("trace level: %s", traceLevels));
            }
            else if (sub == "off")    // /samson/ilogging/trace/off
            {
                lmTraceSet(NULL);
                command->appendFormatedElement("trace", au::str("all trace levels turned off"));
            }
            else if (sub == "add")    // /samson/ilogging/trace/add
            {
                lmTraceAdd((char*) arg.c_str());
                command->appendFormatedElement("trace", au::str("added level(s) %s", arg.c_str()));
            }
            else if (sub == "remove")     // /samson/ilogging/trace/remove
            {
                lmTraceSub((char*) arg.c_str());
                command->appendFormatedElement("trace", au::str("removed level(s) %s", arg.c_str()));
            }
        }
    }        



    //
    // SamsonWorker::getRESTInformation - 
    //
    void SamsonWorker::process( au::network::RESTServiceCommand* command )
    {
        // Default format
        if( command->format == "" )
            command->format = "xml";

        
        // Begin data for each format
        // ---------------------------------------------------
        if ( command->format == "xml" )
        {
            command->append( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
            command->append("<!-- SAMSON Rest interface -->\n" );
            command->append( "<samson>\n" );
        }
        else if ( command->format == "html" )
        {
            command->append("<html><body>");
        }

        // Internal process of the command
        process_intern(command);
        
        // Close data content
        // ---------------------------------------------------
        if (command->format == "xml")
            command->append("\n</samson>\n");
        else if( command->format == "json" )
            command->append("\n");
        else if (command->format == "html")
            command->append("</body></html>");
        
    }
    
    
/* ****************************************************************************
*
* jsonTypeName - 
*/
static const char* jsonTypeName(json_type type)
{
	switch (type)
	{
	case json_type_null:       return "json_type_null";
	case json_type_boolean:    return "json_type_boolean";
	case json_type_double:     return "json_type_double";
	case json_type_int:        return "json_type_int";
	case json_type_object:     return "json_type_object";
	case json_type_array:      return "json_type_array";
	case json_type_string:     return "json_type_string";
	}

	return "json_type_unknown";
}



/* ****************************************************************************
*
* jsonParse - 
*/
static int jsonParse(char* in, char* host, char* port)
{
	struct json_object* json;
	struct json_object* jsonObj;
	struct lh_table*    table;
	lh_entry*           entry;
	char*               key;

	json  = json_tokener_parse(in);
	table = json_object_get_object(json);
	entry = table->head;
	
	while (entry != NULL)
	{
		key     = (char*) entry->k;
		jsonObj = json_object_object_get(json, key);
		
		json_type type = json_object_get_type(jsonObj);

        LM_T(LmtRestData, ("got a '%s'", jsonTypeName(type)));
        if (type == json_type_object)
        {
            struct lh_table*    table2;
            lh_entry*           entry2;

            table2 = json_object_get_object(jsonObj);
            entry2 = table2->head;

            LM_T(LmtRestData, ("Starting while 2 (%p)", entry2));
            while (entry2 != NULL)
            {
                char*                key2     = (char*) entry2->k;
                struct json_object*  jsonObj2 = json_object_object_get(jsonObj, key2);
                json_type            type2    = json_object_get_type(jsonObj2);

                if (type2 == json_type_string)
                {
                    const char* s = json_object_get_string(jsonObj2);
                    LM_T(LmtRestData, ("json: '%s' : %s (%s)", key2, s, jsonTypeName(type2)));

                    if (strcmp(key2, "name") == 0)
                        strcpy(host, s);
                    else if (strcmp(key2, "port") == 0)
                        strcpy(port, s);
                }
                else if (type2 == json_type_int)
                {
                    int i = json_object_get_int(jsonObj2);
                    LM_T(LmtRestData, ("json: '%s' : %d (%s)", key2, i, jsonTypeName(type)));

                    if (strcmp(key2, "port") == 0)
                    {
                        LM_T(LmtRestData, ("found an integer port: %d", i));
                        sprintf(port, "%d", i);
                        LM_T(LmtRestData, ("found an integer port: '%s'", port));
                    }
                }

                entry2 = entry2->next;
            }
        }
		else if (type == json_type_boolean)
		{
			bool b = json_object_get_boolean(jsonObj);
			LM_T(LmtRestData, ("json: '%s' : %s (%s)", key, (b==true)? "true" : "false", jsonTypeName(type)));
		}
		else if (type == json_type_double)
		{
			double d = json_object_get_double(jsonObj);
			LM_T(LmtRestData, ("json: '%s' : %f (%s)", key, d, jsonTypeName(type)));
		}
		else if (type == json_type_int)
		{
			int i = json_object_get_int(jsonObj);
			LM_T(LmtRestData, ("json: '%s' : %d (%s)", key, i, jsonTypeName(type)));

            if (strcmp(key, "port") == 0)
                sprintf(port, "%d", i);
		}
		else if (type == json_type_string)
		{
			const char* s = json_object_get_string(jsonObj);
			LM_T(LmtRestData, ("json: '%s' : %s (%s)", key, s, jsonTypeName(type)));

            if (strcmp(key, "host") == 0)
                strcpy(host, s);
            else if (strcmp(key, "port") == 0)
                strcpy(port, s);
		}
			
		entry = entry->next;
	}

	return 0;
}



/* ****************************************************************************
*
* clusterNodeAdd - 
*/
void SamsonWorker::process_clusterNodeAdd(au::network::RESTServiceCommand* command)
{
    char delilahCommand[256];

    if (command->data == NULL)
    {
        command->appendFormatedError(404, au::str("no data"));
        return;
    }

    if (command->format == "xml")
    {
        char*                   commandData = (char*) malloc(command->data_size + 1);
        char                    delilahCommand[256];
        pugi::xml_document      doc;
        pugi::xml_parse_result  pugiResult;
        
        strncpy(commandData, command->data, command->data_size);
        commandData[command->data_size] = 0;

        LM_T(LmtRest, ("Adding a node - parse the data '%s' of size %d", commandData, command->data_size));
        pugiResult = doc.load(commandData);
        free(commandData);
        if (!pugiResult)
        {
            command->appendFormatedError(404, au::str("error in XML data: '%s'", pugiResult.description()));
            return;
        }

        pugi::xml_node  samson = doc.child("samson");
        pugi::xml_node  node   = samson.child("node");
        char*           host = (char*) node.child_value("name");
        char*           port = (char*) node.child_value("port");
                            
        snprintf(delilahCommand, sizeof(delilahCommand), "cluster add %s:%s", host, port);
        LM_T(LmtRest, ("processing delilah command '%s'", delilahCommand));
        process_delilah_command(delilahCommand, command);

        // How do I know that the 'cluster add' command worked ... ?
        // command->error ?

        // Now, as there is no output from this command, I need to 'reset' the return buffer
        command->output.str("");

        // And as the trailing '</samson>' is appended, I start the chain here ...
        command->output << "<samson>" << "\n" << "<result>OK</result>";
    }
    else if (command->format == "json")
    {
        char* commandData = (char*) malloc(command->data_size + 1);
        char  host[256];
        char  port[32];

        strncpy(commandData, command->data, command->data_size);
        commandData[command->data_size] = 0;
        LM_T(LmtRestData, ("commandData: '%s'", commandData));
        
        if (jsonParse(commandData, host, port) != 0)
        {
            command->appendFormatedError(404, au::str("JSON parse error"));
            return;
        }

        LM_T(LmtRestData, ("host: '%s'", host));
        LM_T(LmtRestData, ("port: '%s'", port));

        snprintf(delilahCommand, sizeof(delilahCommand), "cluster add %s %s", host, port);
        LM_T(LmtRest, ("processing delilah command '%s'", delilahCommand));
        process_delilah_command(delilahCommand, command);

        command->output.str("");
        command->output << "{ \"result\" : \"OK\" }";
    }
}



/* ****************************************************************************
*
* clusterNodeDelete - 
*/
void SamsonWorker::process_clusterNodeDelete(au::network::RESTServiceCommand* command)
{
    char delilahCommand[256];

    if (command->data == NULL)
    {
        command->appendFormatedError(404, au::str("no data"));
        return;
    }

    if (command->format == "xml")
    {
        char*                   commandData = (char*) malloc(command->data_size + 1);
        char                    delilahCommand[256];
        pugi::xml_document      doc;
        pugi::xml_parse_result  pugiResult;
                   
        strncpy(commandData, command->data, command->data_size);
        commandData[command->data_size] = 0;

        LM_T(LmtRest, ("Removing a node - parse the data '%s' of size %d", commandData, command->data_size));
        pugiResult = doc.load(commandData);
        free(commandData);
        if (!pugiResult)
        {
            command->appendFormatedError(404, au::str("error in XML data: '%s'", pugiResult.description()));
            return;
        }

        pugi::xml_node  samson = doc.child("samson");
        pugi::xml_node  node   = samson.child("node");
        char*           host = (char*) node.child_value("name");
        char*           port = (char*) node.child_value("port");
    
        snprintf(delilahCommand, sizeof(delilahCommand), "cluster remove %s:%s", host, port);
        LM_T(LmtRest, ("processing delilah command '%s'", delilahCommand));
        process_delilah_command(delilahCommand, command);

        // How do I know that the 'cluster add' command worked ... ?
        // command->error ?

        // Now, as there is no output from this command, I need to 'reset' the return buffer
        command->output.str("");

        // And as the trailing '</samson>' is appended, I start the chain here ...
        command->output << "<samson>" << "\n" << "<result>OK</result>";
    }
    else if (command->format == "json")
    {
        char* commandData = (char*) malloc(command->data_size + 1);
        char  host[256];
        char  port[32];

        strncpy(commandData, command->data, command->data_size);
        commandData[command->data_size] = 0;
        LM_T(LmtRestData, ("commandData: '%s'", commandData));

        if (jsonParse(commandData, host, port) != 0)
        {
            command->appendFormatedError(404, au::str("JSON parse error"));
            return;
        }

        LM_T(LmtRestData, ("host: '%s'", host));
        LM_T(LmtRestData, ("port: '%s'", port));

        snprintf(delilahCommand, sizeof(delilahCommand), "cluster remove %s %s", host, port);
        LM_T(LmtRest, ("processing delilah command '%s'", delilahCommand));
        process_delilah_command(delilahCommand, command);

        command->output.str("");
        command->output << "{ \"result\" : \"OK\" }";
    }
}

void SamsonWorker::process_intern( au::network::RESTServiceCommand* command )
{
    std::string         main_command  = command->path_components[1];
    std::string         path          = "";
    std::string         verb          = command->command;
    unsigned int        components    = command->path_components.size();

    for (unsigned int ix = 0; ix < components; ix++)
        path += std::string("/") + command->path_components[ix];
    LM_T(LmtRest, ("Incoming REST request: %s '%s'", verb.c_str(), path.c_str()));


    //
    // Quick sanity check
    //
    
    if (components < 2)
    {
        command->appendFormatedError(400 , "Only /samson/option paths are valid");
        return;
    }

    if (command->path_components[0] != "samson")
    {
        command->appendFormatedError(400 , "Only /samson/option paths are valid");
        return;
    }

    //
    // Treat the message
    //
    
    if ((path == "/samson/version") && (verb == "GET"))
    {
        command->appendFormatedElement("version", au::str("SAMSON v %s" , SAMSON_VERSION ));
    }
    else if ((path == "/samson/status") && (verb == "GET"))
    {
        process_delilah_command("ls_workers", command);
        // command->appendFormatedElement("status", "OK");
    }
    else if ((path == "/samson/cluster") && (verb == "GET"))
    {
        std::ostringstream data;
        network->getInfo(data, "cluster", command->format);
        command->append( data.str() );
    }
    else if ((path == "/samson/cluster/add_node") && (verb == "POST"))
    {
        process_clusterNodeAdd(command);
    }
    else if ((path == "/samson/cluster/remove_node") && (verb == "DELETE"))
    {
        process_clusterNodeDelete(command);
    }
    else if (main_command == "ilogging")
    {
        process_ilogging(command);
    }
    else if (main_command == "logging")
    {
        process_logging(command);
    }
    else if (main_command == "node")
    {
        process_node(command);
    }
    else if ((path == "/samson/queues") && (verb == "GET"))
    {
        process_delilah_command( "ls -group name -sort name", command);
    }
    else if ( main_command == "queues" )
    {
        char delilahCommand[256];

        if ((components == 3) && (verb == "GET"))
        {
            snprintf(delilahCommand, sizeof(delilahCommand), "ls %s", command->path_components[2].c_str());
            process_delilah_command(delilahCommand, command);
        }
        else if ((components == 4) && (command->path_components[3] == "delete") && (verb == "DELETE"))
        {
            snprintf(delilahCommand, sizeof(delilahCommand), "rm %s", command->path_components[2].c_str());
            process_delilah_command(delilahCommand, command);
        }
        else if ((components == 5) && (command->path_components[3] == "state") && (verb == "GET"))
            streamManager->process( command );             // Get this from the stream manager
        else
            command->appendFormatedError(404, au::str("Bad VERB or PATH"));
    }
    else if (main_command == "command" ) /* /samson/command */
    {
        std::string delilah_command = "";

        if (command->command != "GET")
        {
            command->appendFormatedError(404, au::str("bad VERB for command"));
            return;
        }

        if (components == 2)
            delilah_command = "ls"; // 'ls' is the default command
        else if (components == 3)
            delilah_command = command->path_components[2];
        
        if (delilah_command != "")
            process_delilah_command(delilah_command, command);
        else
            command->appendFormatedError(404, au::str("bad path for command"));
    }
    else if ((path == "/samson/modules") && (verb == "GET"))
    {
        process_delilah_command("ls_modules", command);
    }
    else if ((main_command == "modules") && (components == 3) && (verb == "GET"))
    {
        char delilahCommand[256];

        snprintf(delilahCommand, sizeof(delilahCommand), "ls_modules %s", command->path_components[2].c_str());
        process_delilah_command(delilahCommand, command);
    }
    else if (main_command == "operations" )  /* /samson/operations */
    {
        char delilahCommand[256];
        
        if ((command->command == "GET") && (components == 2))
        {
            snprintf(delilahCommand, sizeof(delilahCommand), "ls_operations");
            process_delilah_command(delilahCommand, command);
        }
        else if ((command->command == "GET") && (components == 3))
        {
            snprintf(delilahCommand, sizeof(delilahCommand), "ls_operations %s", command->path_components[2].c_str());
            process_delilah_command(delilahCommand, command);
        }
        else if ((command->command == "PUT") && (components == 3))
        {
            // Need to parse the XML here ...
            command->appendFormatedError(400, au::str("Not Implemented"));
        }
        else if ((command->command == "DELETE") && (components == 4) && (command->path_components[2] == "delete"))
        {
            // snprintf(delilahCommand, sizeof(delilahCommand), "rm_stream_operation %s", command->path_components[3].c_str());
            // process_delilah_command(delilahCommand, command);
            command->appendFormatedError(400, au::str("Not Implemented"));
        }
        else
            command->appendFormatedError(404, au::str("bad path/verb"));
    }
#if 0
    else if( ( main_command == "state" ) || ( main_command == "queue" ) ) 
    {
        /* /samson/state/queue/key */
        if ( components < 4 )
            command->appendFormatedError(400 , "Only /samson/state/queue/key paths are valid");
        else
            streamManager->process( command );             // Get this from the stream manager
    }
    else if( main_command == "data_test" )
    {
        command->appendFormatedElement("data_size", au::str("%lu" , command->data_size));
            
        if( command->data_size == 0 )
            command->appendFormatedElement("Data", "No data provided in the REST request");
        else
        {
            // Return with provided data
            std::string data;
            data.append( command->data , command->data_size );
            command->appendFormatedElement("data", data );
        }
    }
#endif
    else
    {
        command->appendFormatedError(404, au::str("Bad VERB or PATH"));
    }
}
    


    void SamsonWorker::process_node(  au::network::RESTServiceCommand* command  )
    {
        
        if( command->format != "json" )
        {
            command->appendFormatedError(400, "Only json format is supported in samson/node/*");
            return;
        }
        
        if( command->path_components.size() <= 2 )
        {
            command->appendFormatedError(400, "Bad path. Supported: samson/node/general.json");
            return;
        }
        
        //  /samson/status/
        std::string sub_command = command->path_components[2];
        
        if( sub_command == "general" )
        {
            command->append( samson_worker_samples.getJson() );
            return;
        }
        else
        {
            command->appendFormatedError(400, "Bad path. Supported: samson/node/general.json");
            return;
        }
        
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
    
    void SamsonWorker::autoComplete( au::ConsoleAutoComplete* info )
    {
        if( info->completingFirstWord() )
        {
            info->add( "quit" );
            info->add( "exit" );
            info->add( "threads" );
            info->add( "cluster" );
            info->add( "show_logs" );
            info->add( "hide_logs" );
            info->add( "show_engine_statistics" );
            info->add( "show_engine_last_items" );
            info->add( "show_engine_elements" );
            info->add( "show_engine_current_element" );
        }
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

        if ( main_command == "exit" )
            quitConsole();
        
        if ( main_command == "threads" )
            writeOnConsole( au::ThreadManager::shared()->str() );
        
        if ( main_command == "cluster" )
            writeOnConsole( network->cluster_command(command) );
        
        if( main_command == "show_logs" )
        {
            au::add_log_plugin( this );
            writeOnConsole("OK\n");
        }
        if( main_command == "hide_logs" )
        {
            au::remove_log_plugin( this );
            writeOnConsole("OK\n");
        }

        if( main_command == "show_engine_current_element" )
        {
            writeOnConsole( engine::Engine::shared()->get_activity_monitor()->getCurrentActivity() + "\n" );
            return;
        }
        
        if( main_command == "show_engine_statistics" )
        {
            writeOnConsole( engine::Engine::shared()->get_activity_monitor()->str_elements() + "\n" );
            return;
        }
        
        if( main_command == "show_engine_last_items" )
        {
            writeOnConsole( engine::Engine::shared()->get_activity_monitor()->str_last_items() + "\n" );
            return;
        }
        
        if( main_command == "show_engine_elements" )
        {
            writeOnConsole( engine::Engine::shared()->getTableOfEngineElements() + "\n" );
            return;
        }
        
        
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
            
            //  Release created packet
            p->release();
        }            
        
    }
    
    network::Collection* SamsonWorker::getWorkerCollection( Visualization* visualization )
    {
        network::Collection* collection = new network::Collection();
        collection->set_name("workers");
        
        network::CollectionRecord* record = collection->add_record();            
        
        // Common type to joint queries ls_workers -group type
        ::samson::add( record , "Type" , "worker" , "different" );
        
        if( visualization->options == engine )
        {
            
            size_t num_elements= engine::Engine::shared()->getNumElementsInEngineStack();
            double waiting_time = engine::Engine::shared()->getMaxWaitingTimeInEngineStack();
            ::samson::add( record , "#elements in engine" , num_elements , "f=uint64,sum" );
            ::samson::add( record , "Max waiting time" , waiting_time , "f=double,different" );
            
        }
        else if( visualization->options == disk )
        {
            // Disk activiry
            
            ::samson::add( record , "Disk in B/s" , engine::DiskManager::shared()->get_rate_in() , "f=uint64,sum" );
            ::samson::add( record , "Disk out B/s" , engine::DiskManager::shared()->get_rate_out() , "f=uint64,sum" );
            
            double op_in = engine::DiskManager::shared()->get_rate_operations_in();
            double op_out = engine::DiskManager::shared()->get_rate_operations_out();
            
            ::samson::add( record , "Disk in Ops/s" , op_in  , "f=double , sum" );
            ::samson::add( record , "Disk out Ops/s" , op_out , "f=double,sum" );
            
            
            double on_time = engine::DiskManager::shared()->on_off_monitor.get_on_time(); 
            double off_time = engine::DiskManager::shared()->on_off_monitor.get_off_time(); 
            ::samson::add( record , "On time" , on_time , "f=double,differet" );
            ::samson::add( record , "Off time" , off_time , "f=double,differet" );
            
            ::samson::add( record , "BM writing" ,  stream::BlockManager::shared()-> get_scheduled_write_size() , "f=uint64,sum" );
            ::samson::add( record , "BM reading" ,  stream::BlockManager::shared()->get_scheduled_read_size() , "f=uint64,sum" );
            
            double usage =  engine::DiskManager::shared()->get_on_off_activity();
            ::samson::add( record , "Disk usage" , au::str_percentage(usage) , "differet" );
            
        }
        else
        {
            
            ::samson::add( record , "Mem used" , engine::MemoryManager::shared()->getUsedMemory() , "f=uint64,sum" );
            ::samson::add( record , "Mem total" , engine::MemoryManager::shared()->getMemory() , "f=uint64,sum" );
            
            ::samson::add( record , "Cores used" , engine::ProcessManager::shared()->getNumUsedCores() , "f=uint64,sum" );
            ::samson::add( record , "Cores total" , engine::ProcessManager::shared()->getNumCores() , "f=uint64,sum" );
            
            ::samson::add( record , "#Disk ops" , engine::DiskManager::shared()->getNumOperations() , "f=uint64,sum" );
            
            ::samson::add( record , "Disk in B/s" , engine::DiskManager::shared()->get_rate_in() , "f=uint64,sum" );
            ::samson::add( record , "Disk out B/s" , engine::DiskManager::shared()->get_rate_out() , "f=uint64,sum" );
            
            ::samson::add( record , "Net in B/s" , network->get_rate_in() , "f=uint64,sum" );
            ::samson::add( record , "Net out B/s" , network->get_rate_out() , "f=uint64,sum" );
        }
        
        if (visualization == NULL)
            return collection;
        
        return collection;
    }
    
    
    void SamsonWorkerSamples::take_samples()
    {
        

        int num_processes = engine::ProcessManager::shared()->public_num_proccesses;
        int max_processes = engine::ProcessManager::shared()->public_max_proccesses;
        
        size_t used_memory = engine::MemoryManager::shared()->public_used_memory;
        size_t max_memory = engine::MemoryManager::shared()->public_max_memory;
        
        size_t disk_read_rate = (size_t) engine::DiskManager::shared()->get_rate_in();
        size_t disk_write_rate = (size_t) engine::DiskManager::shared()->get_rate_out();
        
        size_t network_read_rate = (size_t) samsonWorker_->network->get_rate_in();
        size_t network_write_rate = (size_t) samsonWorker_->network->get_rate_out();

        cpu.push( 100.0 *  (double)  num_processes / (double)  max_processes );
        memory.push( 100.0 * (double) used_memory / (double)  max_memory );

        disk_in.push( disk_read_rate / 1000000  );
		disk_out.push( disk_write_rate / 1000000  );

        net_in.push( network_read_rate / 1000000  );
		net_out.push( network_write_rate / 1000000 );
                    
    }

}
