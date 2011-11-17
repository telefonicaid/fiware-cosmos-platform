
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
#include "samson/common/NotificationMessages.h" // notification_review_timeOut_SamsonPushBuffer

#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/network/Network2.h"
#include "samson/network/EndpointManager.h"
#include "samson/network/Packet.h"

#include "samson/delilah/Delilah.h"             // samson::Delilah

#include "SamsonClient.h"                       // Own interface

#include "samson/delilah/TXTFileSet.h"                         // samson::DataSource

namespace samson {

    
    SamsonPushBuffer::SamsonPushBuffer( SamsonClient *_client , std::string _queue , int _timeOut  ) : token("SamsonPushBuffer")
    {
        client = _client;
        queue = _queue;
        max_buffer_size = 64*1024*1024 - sizeof(KVHeader) ; // Perfect size for this ;)
        buffer = (char*) malloc( max_buffer_size );

        timeOut =  _timeOut;
        
        size = 0;
        
        // Received notification about this
        listen( notification_review_timeOut_SamsonPushBuffer );
        
    }
    
    SamsonPushBuffer::~SamsonPushBuffer()
    {
        if( buffer )
            free(buffer);
    }

    void SamsonPushBuffer::push( const char *data , size_t length )
    {
        // Keep accumulated size
        total_size += length;
        
        // If this is the first time we push data, just reset the cronometert to zero
        if ( size == 0)
            cronometer.reset();
        
        
        if( (size + length ) > max_buffer_size )
        {
            // Process buffer
            LM_V(("Pushing %s to queue %s. Total accumulated %s\n" , au::str(size,"B").c_str() , queue.c_str()  , au::str(total_size,"B").c_str() ));
            
            client->push(  queue , buffer, size );

            // Reset cronometer
            cronometer.reset();
            
            // Come back to "0"
            size = 0;
        }
        else
        {
            // Acumulate contents
            memcpy(buffer+size, data, length);
            size += length;

        }
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
            if( paVerbose )
                LM_M(("Pushing %s to queue %s\n" , au::str(size,"B").c_str() , queue.c_str()));
            client->push(  queue , buffer, size );
            
            // Come back to "0"
            size = 0;
           
        }
    }

    
    void SamsonPushBuffer::notify( engine::Notification* notification )
    {
        au::TokenTaker tt(&token);
        
        // We only expect notification about reviwing timeout
        
        if( timeOut > 0)
            if( ( size > 0 ) && (cronometer.diffTimeInSeconds() > timeOut) )
                _flush();
        
    }
    

    
#pragma mark
    
    samson::EndpointManager* epMgr     = NULL;
    samson::Network2*        networkP  = NULL;
    samson::Delilah* delilah = NULL;    
    
    BufferContainer buffer_container;   // Container for live data
    
    void delialh_client_delilah_process_stream_out_queue(std::string queue , engine::Buffer* buffer)
    {
        // Put inside the buffer to storage
        buffer_container.push( queue , buffer );
    }
    
    SamsonClient::SamsonClient()
    {
        memory = 1024*1024*1024;
        
        load_buffer_size =  64*1024*1024;
        
    }
 
    void SamsonClient::setMemory ( size_t _memory )
    {
        memory = _memory;
    }
    
    bool SamsonClient::init( std::string controller )
    {
                
        
        // Init the setup system 
        LM_TODO(("Add the possibility to set particular directories for this..."));
        samson::SamsonSetup::init( "/opt/samson/" , "/var/samson/" );    

        // Change the values for this parameters
        samson::SamsonSetup::shared()->setValueForParameter("general.memory", au::str("%lu",memory) );
        samson::SamsonSetup::shared()->setValueForParameter("load.buffer_size",  au::str("%lu",load_buffer_size) );
        
        engine::Engine::init();
        engine::DiskManager::init(1);
        engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));
        engine::MemoryManager::init(samson::SamsonSetup::shared()->getUInt64("general.memory"));
        
        samson::ModulesManager::init();         // Init the modules manager
        
        // Initialize the network element for delilah
        networkP  = new samson::Network2( samson::Endpoint2::Delilah, controller.c_str() );
        
        // Init the network connection in background
        networkP->runInBackground();
        
        //
        // What until the network is ready
        //
        //std::cout << "\nConnecting to SAMSOM controller " << controller << " ...";
        while (!networkP->ready())
            usleep(1000);
        //std::cout << " OK\n";
        
        //
        // Ask the Controller for the platform process list
        //
        // First, give controller some time for the interchange of Hello messages
        //
        samson::Packet*  packetP  = new samson::Packet(samson::Message::Msg, samson::Message::ProcessVector);
        
        LM_TODO(("I should probably go through NetworkInterface here ..."));
        networkP->epMgr->controller->send( packetP );
        
        //
        // What until the network is ready II
        //
        //std::cout << "Connecting to all workers ...";
        while (!networkP->ready(true))
            sleep(1);
        //std::cout << " OK\n";
        
        // Create a DelilahControler once network is ready
        delilah = new Delilah( networkP, true );
        
        // Set the funciton to process live stream data
        delilah->op_delilah_process_stream_out_queue = delialh_client_delilah_process_stream_out_queue;
        
        // Periodic notification to review timeout
        engine::Notification *notification = new engine::Notification( notification_review_timeOut_SamsonPushBuffer  );
        engine::Engine::shared()->notify( notification , 1);
        
        return true;
        
    }
    
    size_t SamsonClient::push( std::string queue , char *data , size_t length )
    {
        
        samson::BufferDataSource * ds = new samson::BufferDataSource( data , length );
        
        std::vector<std::string>queues;
        queues.push_back( queue );
        
        size_t id = delilah->addPushData( ds , queues );

        // Save the id to make sure it is finish before quiting...
        delilah_ids.push_back( id );
        
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
/*
#ifdef __LP64__
                printf("SamsonClient: Waiting %lu: %s\n" , id , description.c_str() );
#else
                printf("SamsonClient: Waiting %d: %s\n" , id , description.c_str() );
#endif
*/              
                sleep(1);
            }
        }
        
    }


    void SamsonClient::connect_to_queue( std::string queue )
    {
        std::string command = au::str("connect_to_queue %s" , queue.c_str() ); 
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
    
    
    
    
}
