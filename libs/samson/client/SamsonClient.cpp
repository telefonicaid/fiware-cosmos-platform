
/* ****************************************************************************
 *
 * FILE            SamsonClient .cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
 *
 * DATE            7/14/11
 *
 * DESCRIPTION
 *
 *  Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#include "logMsg/logMsg.h"
#include "parseArgs/paConfig.h"

#include "au/string.h"                          // au::Format

#include "engine/Engine.h"                      // engine::Engine
#include "engine/MemoryManager.h"               // engine::MemoryManager
#include "engine/DiskManager.h"                 // engine::DiskManager
#include "engine/ProcessManager.h"              // engine::ProcessManager
#include "engine/Notification.h"                // engine::Notification


#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "samson/common/samsonDirectories.h"    
#include "samson/common/NotificationMessages.h" // notification_review_timeOut_SamsonPushBuffer

#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/network/DelilahNetwork.h"
#include "samson/network/Packet.h"

#include "samson/delilah/Delilah.h"             // samson::Delilah
#include "samson/delilah/DelilahComponent.h"    // samson::DelilahComponent

#include "SamsonClient.h"                       // Own interface

#include "samson/delilah/TXTFileSet.h"                         // samson::DataSource

namespace samson {

    
    SamsonPushBuffer::SamsonPushBuffer( SamsonClient *_client , std::string _queue  ) : token("SamsonPushBuffer")
    {
        // Client and queue name to push data to
        client = _client;
        queue = _queue;

        
        // Init the simple buffer
        max_buffer_size = 64*1024*1024 - sizeof(KVHeader) ; // Perfect size for this ;)
        buffer = (char*) malloc( max_buffer_size );
        size = 0;
        
        
    }
    
    SamsonPushBuffer::~SamsonPushBuffer()
    {
        if( buffer )
            free(buffer);
    }

    void SamsonPushBuffer::push( const char *data , size_t length , bool flushing )
    {
        // Mutex protection
        au::TokenTaker tt(&token);
        
        // Statistics
        rate.push( length );
        
        if( (size + length ) > max_buffer_size )
        {
            // Push to the client
            client->push(  queue , buffer, size );

            // Come back to "0"
            size = 0;
        }

        // Copy new content to the upload buffer
        {
            // Acumulate contents
            memcpy(buffer+size, data, length);
            size += length;

        }
        
        if( flushing )
            _flush();
    }

    void SamsonPushBuffer::flush()
    {
        au::TokenTaker tt(&token);
        _flush();
    }

    void SamsonPushBuffer::_flush()
    {
        if ( size > 0 )
        {
            // Process buffer
            LM_V(("SamsonPushBuffer: Pushing %s to queue %s\n" , au::str(size,"B").c_str() , queue.c_str()));
            client->push(  queue , buffer, size );
            
            // Come back to "0"
            size = 0;
           
        }
    }

    
#pragma mark
    
    samson::DelilahNetwork* networkP  = NULL;
    samson::Delilah* delilah = NULL;    
    
    BufferContainer buffer_container;   // Container for live data
    
    void delialh_client_delilah_process_stream_out_queue(std::string queue , engine::Buffer* buffer)
    {
        // Put inside the buffer to storage
        buffer_container.push( queue , buffer );
    }
    
    SamsonClient::SamsonClient( std::string _connection_type )
    {
        memory = 1024*1024*1024;
        load_buffer_size =  64*1024*1024;
        
        connection_type = _connection_type;
    }
 
    void SamsonClient::setMemory ( size_t _memory )
    {
        memory = _memory;
    }
    
    bool SamsonClient::init( std::string worker_host , int port  , std::string user , std::string password )
    {
                
        // Init the setup system 
        LM_TODO(("Add the possibility to set particular directories for this..."));
        
        std::string samson_home =  SAMSON_HOME_DEFAULT;
        std::string samson_working = SAMSON_WORKING_DEFAULT;

        char *env_samson_working = getenv("SAMSON_WORKING");
        char *env_samson_home = getenv("SAMSON_HOME");

        if( env_samson_working )
        {
            samson_working = env_samson_working;
        }
        if( env_samson_home )
        {
            samson_home = env_samson_home;
        }
        
        samson::SamsonSetup::init( samson_home , samson_working );    

        // Change the values for this parameters
        samson::SamsonSetup::shared()->setValueForParameter("general.memory", au::str("%lu",memory) );
        samson::SamsonSetup::shared()->setValueForParameter("load.buffer_size",  au::str("%lu",load_buffer_size) );
        
        engine::Engine::init();
        engine::DiskManager::init(1);
        engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));
        engine::MemoryManager::init(samson::SamsonSetup::shared()->getUInt64("general.memory"));
        
        samson::ModulesManager::init();         // Init the modules manager
        
        // Initialize the network element for delilah
        networkP  = new samson::DelilahNetwork( connection_type );

        // Create a DelilahControler once network is ready
        delilah = new Delilah( networkP );

        // No automatic update
        delilah->automatic_update = false;
        
        // Init network connection
        Status s = networkP->addMainDelilahConnection( worker_host , port , user , password );
        
        if( s != OK )
            LM_X(1, ("Not possible to open connection with %s:%d (%s)" , worker_host.c_str() , port , status(s) ));
        
        // Set the funciton to process live stream data
        delilah->op_delilah_process_stream_out_queue = delialh_client_delilah_process_stream_out_queue;
        
        // What until the network is ready
        LM_V(("Waiting network connections to the all nodes in SAMSON cluster..."));
        while ( !networkP->ready() )
            usleep(1000);
        LM_V(("Connected to all workers"));
        
        return true;
        
    }
    
    size_t SamsonClient::push( std::string queue , char *data , size_t length )
    {
        // Statistics
        rate.push( length );
        
        // Show some info if -v option is selected
        LM_V(("Pushing %s to queue %s" , au::str(length,"B").c_str() , queue.c_str() ));
        LM_V(("SamsonClient info: %s"  , rate.str().c_str() ));
        
        // Block this call if memory is not enougth
        double memory_usage = engine::MemoryManager::shared()->getMemoryUsage();
        while( memory_usage >= 0.8 )
        {
            LM_W(("Memory usage %s. Waiting until this goes below 80%..." , au::str_percentage( memory_usage ).c_str() )); 
            getInfoAboutPushConnections(true);
            sleep(1);
            memory_usage = engine::MemoryManager::shared()->getMemoryUsage();
        }
        
        samson::BufferDataSource * ds = new samson::BufferDataSource( data , length );
        
        std::vector<std::string>queues;
        queues.push_back( queue );
        
        size_t id = delilah->addPushData( ds , queues );

        // Save the id to make sure it is finish before quiting...
        delilah_ids.push_back( id );

        
        // Clean previous jobs ( if any )
        delilah->clearComponents();
        
        return id;
    }
    
    
    std::string SamsonClient::getErrorMessage()
    {
        return error_message;
    }
    
    void SamsonClient::waitUntilFinish()
    {
        
        for ( size_t i = 0 ; i < delilah_ids.size() ; i++)
        {
            size_t id = delilah_ids[i];
            
            while (delilah->isActive( id ) )
            {
                std::string description =  delilah->getDescription( id );                
                LM_V(("Waiting process %lu: %s", id , description.c_str()  ));
                sleep(1);
            }
        }
        
    }


    void SamsonClient::connect_to_queue( std::string queue , bool flag_new , bool flag_remove )
    {
        std::string command = au::str("connect_to_queue %s" , queue.c_str() ); 
        
        if( flag_new )
            command.append(" -new ");
        
        if( flag_remove )
            command.append(" -remove ");
        
        //LM_M(("Command to connect to queue '%s'", command.c_str() ));
        delilah->sendWorkerCommand( command , NULL );
    }
    
    SamsonClientBlockInterface* SamsonClient::getNextBlock( std::string queue )
    {
        engine::Buffer *buffer = buffer_container.pop( queue );
        
        if( buffer )
            return new SamsonClientBlock( buffer );
        else
            return NULL;
    }
    
    std::string SamsonClient::getInfoAboutPushConnections( bool print_verbose )
    {
        if( !delilah )
            return "Connection not established";
        
        int num_components = 0;
        
        // Clean previous jobs ( if any )
        delilah->clearComponents();
        
        std::ostringstream output;
        {
            au::TokenTaker tt(&delilah->token);

            au::map<size_t , DelilahComponent>::iterator it_components;	
            for( it_components = delilah->components.begin() ; it_components != delilah->components.end() ; it_components++)
            {
                if( it_components->second->type == DelilahComponent::push )
                {
                    output << it_components->second->getShortDescription() << " ";
                    num_components++;
                }
            }
        }
        
        if( print_verbose && ( num_components > 0 ) )
            LM_V(( "Push components %s" , output.str().c_str() ));
        
        return output.str();
        
    }
    
    std::string SamsonClient::getInfoAboutDelilahComponents()
    {
        // Clean previous jobs ( if any )
        delilah->clearComponents();
        
        std::ostringstream output;
        output << "[ ";
        for( size_t i = 0 ; i < delilah_ids.size() ; i++)
        {
            if( delilah->isActive( delilah_ids[i]  ) )
                output << delilah_ids[i] << " ";
        }
        output << "]";
        
        return output.str();
    }
    
    
}
