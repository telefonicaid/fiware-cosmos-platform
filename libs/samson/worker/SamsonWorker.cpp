#include <iconv.h>
#include <iostream>                               // std::cout ...
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>

#include "logMsg/logMsg.h"                        // lmInit, LM_*
#include "logMsg/traceLevels.h"                   // Trace Levels

#include "au/CommandLine.h"                       // CommandLine
#include "au/string.h"                            // au::Format
#include "au/time.h"                              // au::todayString
#include "au/ThreadManager.h"

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

#include "samson/worker/WorkerCommand.h"          // samson::stream::WorkerCommand
#include "samson/worker/SamsonWorker.h"           // Own interfce

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
            
            std::string worker_command_id = au::str( "%s_%lu" , au::code64_str(delilah_id).c_str() , delilah_component_id );
            
            WorkerCommand *workerCommand = new WorkerCommand(  worker_command_id
                                                             , delilah_id 
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
            
            // Some ancient samson-0.6 useful Status information
            // Collect some information and print status...
            int num_processes = engine::ProcessManager::shared()->public_num_proccesses;
            int max_processes = engine::ProcessManager::shared()->public_max_proccesses;

            size_t used_memory = engine::MemoryManager::shared()->public_used_memory;
            size_t max_memory = engine::MemoryManager::shared()->public_max_memory;

            size_t disk_read_rate = (size_t) engine::DiskManager::shared()->get_rate_in();
            size_t disk_write_rate = (size_t) engine::DiskManager::shared()->get_rate_out();

            LM_M(("Status [ P %s M %s D_in %s D_out %s ]"
                  , au::str_percentage( num_processes, max_processes ).c_str()
                  , au::str_percentage(used_memory, max_memory).c_str()
                  , au::str( disk_read_rate , "Bs" ).c_str()
                  , au::str( disk_write_rate , "Bs" ).c_str()
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
                Packet *packet = (Packet *) notification->extractObject();
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
std::string SamsonWorker::getRESTInformation(::std::string in)
{
    unsigned short int  http_state = 200;  // be optimistic and assume all is ok :)
    std::ostringstream  header;
    std::ostringstream  data;
    std::string         jsonSuffix = ".json";
    std::string         xmlSuffix  = ".xml";
    std::string         format     = "xml"; // Default value


    LM_T(LmtRest, ("Incoming REST request: '%s'", in.c_str()));

    if (in.substr(in.length() - jsonSuffix.length()) == jsonSuffix)
    {
        format = "json";
        in     = in.substr(0, in.length() - jsonSuffix.length());
    }
    else if (in.substr(in.length() - xmlSuffix.length()) == xmlSuffix)
    {
        format = "xml";
        in     = in.substr(0, in.length() - xmlSuffix.length());
    }
    
    if (format == "xml")
    {
        data << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
        data << "<!-- SAMSON Rest interface -->\r\n";
        data << "<samson>\r\n";
    }
    else
    {
        data << "{\r\n";
    }
    

    // Get the path components
    std::vector<std::string> path_components = au::split( in , '/' );
    
    if ((path_components.size() < 2) || (path_components[0] != "samson"))
    {
        http_state = 400;

        if (format == "xml")
            au::xml_simple(data, "message", "Error. Only /samson/path requests are valid");
        else
            data << "  \"error\" : \"Only /samson/path requests are valid\"";
    }
    else if (path_components[1] == "version")
    {
        if (format == "xml")
            au::xml_simple(data, "version", au::str("SAMSON v %s" , SAMSON_VERSION ) );
        else
            data << "  \"version\" : \"" << "SAMSON v " << SAMSON_VERSION << "\"\r\n";
    }
    else if (path_components[1] == "die")
    {
        LM_X(1, ("Got a DIE request over REST interface ... I die!"));
    }
    else if (path_components[1] == "utftest")
    {
        char  test[32];
        int   ix = 0;

        memset(test, 0, 32);
        test[ix++] = '-';
        test[ix++] = 0xA2;
        test[ix++] = 0xA3;
        test[ix++] = 0xA4;
        test[ix++] = '-';
        test[ix++] = 0;
        
        if (format == "xml")
            au::xml_simple(data, "utf8", au::str(test));
        else
            data << "  \"utf8\" : \"" << test << "\"";
    }
    else if( path_components[1] == "state" )  /* /samson/state/queue/key */
    {
        char redirect[512];
        
        redirect[0] = 0;
        if (path_components.size() < 4)
        {
            http_state = 400;

            if (format == "xml")
                au::xml_simple(data, "format_error", au::str("correct format: /samson/state/queue/key"));
            else
                data << "  \"format_error\" : \"correct format: /samson/state/queue/key\"";
        }
        else
        {
            bool         ok     = true;
            std::string  result = streamManager->getState(path_components[2], path_components[3].c_str(), redirect, sizeof(redirect), &ok);

            if (ok == false)
            {
                if (format == "xml")
                    au::xml_simple(data, "error", result);
                else
                    data << "  \"error\" : \"" << result << "\"\r\n";
            }
        }
        
        if (redirect[0] != 0)
        {
            LM_T(LmtRest, ("redirecting to '%s'", redirect));
            
            header << "HTTP/1.1 302 Found\r\n";
            header << "Location:   " << redirect << "/samson/state/" << path_components[2] << "/" << path_components[3].c_str() << "\r\n";
            header << "Content-Type:   application/txt; charset=utf-8\r\n";
            header << "Content-Length: " << 0 << "\r\n";
            header << "\r\n";
            header << "\r\n";
            
            std::ostringstream output;
            output << header.str();
            return output.str();
        }
    }
    else if( path_components[1] == "queue" ) 
    {
        char redirect[512];
            
        redirect[0] = 0;

        if (path_components.size() < 4)
        {
            http_state = 400;

            if (format == "xml")
                au::xml_simple(data, "format_error", au::str("correct format: /samson/queue/<queue_name>/<key>" ) );
            else
                data << "  \"format_error\" : \"correct format: /samson/queue/<queue_name>/<key>\"\r\n";
        }
        else
        {
            bool         ok     = true;
            std::string  result = streamManager->getState(path_components[2] , path_components[3].c_str(), redirect, sizeof(redirect), &ok);

            if (ok == false)
            {
                if (format == "xml")
                    au::xml_simple(data, "message", result);
                else
                    data << "  \"error\" : \"" << result << "\"\r\n";
            }
        }

        if (redirect[0] != 0)
        {
            LM_T(LmtRest, ("redirecting to '%s'", redirect));
                    
            header << "HTTP/1.1 302 Found\r\n";
            header << "Location:   " << redirect << "/samson/queue/" << path_components[2] << "/" << path_components[3].c_str() << "\r\n";
            header << "Content-Type:   application/txt; charset=utf-8\r\n";
            header << "Content-Length: " << 0 << "\r\n";
            header << "\r\n";
            header << "\r\n";

            std::ostringstream output;
            output << header.str();
            return output.str();
        }
    }
    else if (path_components[1] == "cluster" ) /* /samson/logging */
    {
        network->getInfo(data, "cluster", format);
    }
    else if (path_components[1] == "logging" )  
    {
        std::string logCommand = "";
        std::string sub        = "";
        std::string arg        = "";
        
        if (path_components.size() > 2)
            logCommand = path_components[2];
        if (path_components.size() > 3)
            sub = path_components[3];
        if (path_components.size() > 4)
            arg = path_components[4];
        
        
        //
        // Treat all possible errors
        //
        if (logCommand == "")
        {
            http_state = 400;

            if (format == "xml")
                au::xml_simple(data, "message", au::str("no logging subcommand"));
            else
                data << "  \"error\" : \"" << au::str("no logging subcommand") << "\"\r\n";
        }
        else if ((logCommand != "reads") && (logCommand != "writes") && (logCommand != "trace") && (logCommand != "verbose") && (logCommand != "debug"))
        {
            http_state = 400;

            if (format == "xml")
                au::xml_simple(data, "message", au::str("bad logging command: '%s'", logCommand.c_str()));
            else
                data << "  \"error\" : \"" << au::str("bad logging command: '%s'", logCommand.c_str()) << "\"\r\n";
        }
        else if (((logCommand == "reads") || (logCommand == "writes") || (logCommand == "debug")) && (sub != "on") && (sub != "off"))
        {
            http_state = 400;

            if (format == "xml")
                au::xml_simple(data, "message", au::str("bad logging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
            else
                data << "  \"error\" : \"" << au::str("bad logging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()) << "\"\r\n";
        }
        else if ((logCommand == "verbose") && (sub != "get") && (sub != "set") && (sub != "off"))
        {
            http_state = 400;

            if (format == "xml")
                au::xml_simple(data, "message", au::str("bad logging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
            else
                data << "  \"error\" : \"" << au::str("bad logging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()) << "\"\r\n";
        }
        else if ((logCommand == "verbose") && (sub == "set") && (arg != "1") && (arg != "2") && (arg != "3") && (arg != "4") && (arg != "5"))
        {
            http_state = 400;

            if (format == "xml")
                au::xml_simple(data, "message", au::str("bad logging argument for 'trace/set': %s", arg.c_str()));
            else
                data << "  \"error\" : \"" << au::str("bad logging argument for 'trace/set': %s", arg.c_str())  << "\"\r\n";
        }
        else if ((logCommand == "trace") && (sub != "get") && (sub != "set") && (sub != "add") && (sub != "remove") && (sub != "off"))
        {
            http_state = 400;

            if (format == "xml")
                au::xml_simple(data, "message", au::str("bad logging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
            else
                data << "  \"error\" : \"" << au::str("bad logging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()) << "\"\r\n";
        }
        else if (sub == "")
        {
            http_state = 400;

            if (format == "xml")
                au::xml_simple(data, "message", au::str("logging subcommand for '%s' missing", logCommand.c_str()));
            else
                data << "  \"error\" : \"" << au::str("logging subcommand for '%s' missing", logCommand.c_str()) << "\"\r\n";
        }
        else if ((logCommand == "trace") && ((sub != "set") || (sub != "add") || (sub != "remove")))
        {
            if (strspn(arg.c_str(), "0123456789-,") != strlen(arg.c_str()))
            {
                http_state = 400;

                if (format == "xml")
                    au::xml_simple(data, "message", au::str("bad logging parameter '%s' for 'trace/%s'", arg.c_str(), sub.c_str()));
                else
                    data << "  \"error\" : \"" << au::str("bad logging parameter '%s' for 'trace/%s'", arg.c_str(), sub.c_str()) << "\"\r\n";
            }
        }
        
        if (http_state != 200)
            goto afterTreatment;
        

        //
        // Treat the request
        //
        if (logCommand == "reads")
        {
            if (sub == "on")
            {
                lmReads  = true;

                if (format == "xml")
                    au::xml_simple(data, "reads", au::str("reads turned ON"));
                else
                    data << "  \"reads\" : \"reads turned ON\"\r\n";
            }
            else if (sub == "off")
            {
                lmReads  = false;
                if (format == "xml")
                    au::xml_simple(data, "reads", au::str("reads turned OFF"));
                else
                    data << "  \"reads\" : \"reads turned OFF\"\r\n";
            }
        }
        else if (logCommand == "writes")
        {
            if (sub == "on")
            {
                lmWrites  = true;

                if (format == "xml")
                    au::xml_simple(data, "writes", au::str("writes turned ON"));
                else
                    data << "  \"writes\" : \"writes turned ON\"\r\n";
            }
            else if (sub == "off")
            {
                lmWrites  = false;
                if (format == "xml")
                    au::xml_simple(data, "writes", au::str("writes turned OFF"));
                else
                    data << "  \"writes\" : \"writes turned OFF\"\r\n";
            }
        }
        else if (logCommand == "debug")
        {
            if (sub == "on")
            {
                lmDebug  = true;

                if (format == "xml")
                    au::xml_simple(data, "debug", au::str("debug turned ON"));
                else
                    data << "  \"debug\" : \"debug turned ON\"\r\n";
            }
            else if (sub == "off")
            {
                lmDebug  = false;
                if (format == "xml")
                    au::xml_simple(data, "debug", au::str("debug turned OFF"));
                else
                    data << "  \"debug\" : \"debug turned OFF\"\r\n";
            }
        }
        else if (logCommand == "verbose")  // /samson/logging/verbose
        {
            if (sub == "get")
            {
                int vLevel;
                
                if      (lmVerbose5 == true)  vLevel = 5;
                else if (lmVerbose4 == true)  vLevel = 4;
                else if (lmVerbose3 == true)  vLevel = 3;
                else if (lmVerbose2 == true)  vLevel = 2;
                else if (lmVerbose  == true)  vLevel = 1;
                else                          vLevel = 0;
                
                if (format == "xml")
                    au::xml_simple(data, "verbose", au::str("verbosity level: %d", vLevel));
                else
                    data << "  \"verbose\" : \"verbosity level: " << vLevel << "\"\r\n";
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

                if (format == "xml")
                {
                    if (sub == "0")
                        au::xml_simple(data, "verbose", au::str("verbose levels OFF", sub.c_str()));
                    else
                        au::xml_simple(data, "verbose", au::str("verbose levels upto %s SET", sub.c_str()));
                }
                else
                {
                    if (arg == "0")
                        data << "  \"verbose\" : \"verbose levels OFF\"\r\n";
                    else
                        data << "  \"verbose\" : \"verbose levels upto " << arg.c_str() << " SET\"\r\n";
                }
            }
        }
        else if (logCommand == "trace")
        {
            if (sub == "set")
            {
                lmTraceSet((char*) arg.c_str());

                if (format == "xml")
                    au::xml_simple(data, "traceLevels", au::str(arg.c_str()));
                else
                    data << "  \"traceLevels\" : \"" << arg.c_str() << "\"\r\n";
            }
            else if (sub == "get")    // /samson/logging/trace/get
            {
                char traceLevels[1024];
                lmTraceGet(traceLevels);

                if (format == "xml")
                    au::xml_simple(data, "message", au::str("Tracelevels: '%s'", traceLevels));
                else
                    data << "  \"traceLevels\" : \"" << traceLevels << "\"\r\n";
            }
            else if (sub == "off")    // /samson/logging/trace/off
            {
                lmTraceSet(NULL);

                if (format == "xml")
                    au::xml_simple(data, "traceLevels", au::str("all trace levels turned off"));
                else
                    data << "  \"traceLevels\" : \"all trace levels turned off\"\r\n";
            }
            else if (sub == "add")    // /samson/logging/trace/add
            {
                lmTraceAdd((char*) arg.c_str());

                if (format == "xml")
                    au::xml_simple(data, "traceLevels", au::str("added level(s) %s",  arg.c_str()));
                else
                    data << "  \"traceLevels\" : \"added level(s) " << arg.c_str() << "\"\r\n";
            }
            else if (sub == "remove")     // /samson/logging/trace/remove
            {
                lmTraceSub((char*) arg.c_str());

                if (format == "xml")
                    au::xml_simple(data, "traceLevels", au::str("removed level(s) %s",  arg.c_str()));
                else
                    data << "  \"traceLevels\" : \"removed level(s) " << arg.c_str() << "\"\r\n";
            }
        }
    }
    else
    {
        http_state = 404;

        if (format == "xml")
            au::xml_simple(data, "error", au::str("unknown path component '%s'\n" , path_components[1].c_str() ) );
        else
            data << "  \"error\" : \"" << au::str("unknown path component '%s'\"\n" , path_components[1].c_str());
    }

afterTreatment:

    if (format == "xml")
    {
        data << "\r\n</samson>\r\n";
    }
    else
    {
        data << "}\r\n";
    }
    
        
    int dataLen = data.str().length();

    // Send the correct HTTP status code
    switch (http_state)
    {
    case 200:
        header << "HTTP/1.1 200 OK\r\n";
        break;
        
    case 400:
        header << "HTTP/1.1 400 Bad Request\r\n";
        break;
        
    case 404:
        header << "HTTP/1.1 404 Not Found\r\n";
        break;
        
    default:
        header << "HTTP/1.1 Bad Request \r\n"; 
        break;
    }
    
    if (format == "xml")
        header << "Content-Type:   \"application/xml; charset=utf-8\"\r\n";
    else
        header << "Content-Type:   \"application/json; charset=utf-8\"\r\n";

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

    
}
