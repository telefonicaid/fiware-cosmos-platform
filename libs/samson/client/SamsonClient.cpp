
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
#include "au/ThreadManager.h"

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


#include "SamsonClient.h"                       // Own interface

#include "samson/delilah/TXTFileSet.h"                         // samson::DataSource

extern size_t delilah_random_code;

namespace samson 
{
    
    SamsonClient::SamsonClient( std::string _connection_type )
    {
        connection_type = _connection_type;
        delilah = new Delilah();
        
        // No automatic update for this delilah
        delilah->automatic_update = false;
        
        // Set me as the receiver for live data packets
        delilah->data_receiver_interface = this;
    }
    
    SamsonClient::~SamsonClient()
    {
        // Remove delilah instance
        delete delilah;
        
    }
    
    void SamsonClient::set_receiver_interface( DelilahLiveDataReceiverInterface* interface )
    {
        delilah->data_receiver_interface = interface;
    }

    void SamsonClient::general_init( size_t memory , size_t load_buffer_size )
    {
        std::string samson_home    = SAMSON_HOME_DEFAULT;
        std::string samson_working = SAMSON_WORKING_DEFAULT;
        
        char *env_samson_working = getenv("SAMSON_WORKING");
        char *env_samson_home = getenv("SAMSON_HOME");
        
        if( env_samson_working )
            samson_working = env_samson_working;

        if( env_samson_home )
            samson_home = env_samson_home;
        
        // Init setup
        samson::SamsonSetup::init( samson_home , samson_working );    
        
        // Change the values for this parameters
        samson::SamsonSetup::shared()->setValueForParameter("general.memory", au::str("%lu",memory) );
        samson::SamsonSetup::shared()->setValueForParameter("load.buffer_size",  au::str("%lu",load_buffer_size) );

        // Init Engine, DiskManager, ProcessManager and Memory manager
        engine::Engine::init();
        engine::DiskManager::init(1);
        engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));
        engine::MemoryManager::init(samson::SamsonSetup::shared()->getUInt64("general.memory"));
        
        // Init the modules manager
        samson::ModulesManager::init();         
    }
    
    void SamsonClient::general_close( )
    {
        
        engine::Engine::stop();
        engine::DiskManager::stop();
        engine::ProcessManager::stop();

        // Wait all threads to finish
        au::ThreadManager::shared()->wait("SamsonClient");

        engine::MemoryManager::destroy();
        samson::ModulesManager::destroy();         
        samson::SamsonSetup::destroy( );    
        
        engine::DiskManager::destroy();
        engine::ProcessManager::destroy();
        engine::Engine::destroy();
        
    }
    
    void SamsonClient::initConnection( au::ErrorManager *error
                                      , std::string worker_host 
                                      , int port  
                                      , std::string user 
                                      , std::string password )
    {
 
        // Create a DelilahControler once network is ready
        delilah_random_code = au::code64_rand();
        delilah->delilah_connect(connection_type, worker_host, port, user, password, error);

        if( error->isActivated() )
        {
            LM_W(("Not possible to open connection with %s:%d (%s)" , worker_host.c_str() , port , error->getMessage().c_str() ));
            return;
        }
        
        // What until the network is ready
        LM_VV(("Waiting network connections to the all nodes in SAMSON cluster..."));
        while ( !delilah->isConnectionReady() )
            usleep(1000);
        LM_VV(("Connected to all workers"));
    }
    
    void SamsonClient::disconnect()
    {
        // Stop all connections of my delilah
        delilah->stop();
    }

    
    void SamsonClient::receive_buffer_from_queue( std::string queue , engine::Buffer* buffer )
    {
        // Accumulate buffers of data in this buffer container
        buffer_container.push( queue , buffer );
    }
    
    size_t SamsonClient::push( std::string queue , char *data , size_t length )
    {
        // Statistics
        push_rate.push( length );
        
        // Show some info if -v option is selected
        LM_VV(("Pushing %s to queue %s" , au::str(length,"B").c_str() , queue.c_str() ));
        LM_VV(("SamsonClient info: %s"  , push_rate.str().c_str() ));
        
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
    
    size_t SamsonClient::push( std::string queue , DataSource *ds )
    {
        // Statistics
        push_rate.push( ds->getTotalSize() );
        
        // Show some info if -v option is selected
        LM_VV(("Pushing a data source to queue %s" ,  queue.c_str() ));
        LM_VV(("SamsonClient info: %s"  , push_rate.str().c_str() ));
                
        std::vector<std::string>queues;
        queues.push_back( queue );
        
        size_t id = delilah->addPushData( ds , queues );
        
        // Save the id to make sure it is finish before quiting...
        delilah_ids.push_back( id );
                
        // Clean previous jobs ( if any )
        delilah->clearComponents();
        
        return id;
    }
    
    size_t SamsonClient::push( std::string queue , engine::Buffer * buffer )
    {
        if( !buffer )
            return 0;
        
        // Statistics
        push_rate.push( buffer->getSize() );
        
        // Show some info if -v option is selected
        LM_VV(("Pushing a data source to queue %s" ,  queue.c_str() ));
        LM_VV(("SamsonClient info: %s"  , push_rate.str().c_str() ));
        
        std::vector<std::string>queues;
        queues.push_back( queue );
        
        size_t id = delilah->addPushData( buffer , queues );
        
        // Save the id to make sure it is finish before quiting...
        delilah_ids.push_back( id );
        
        // Clean previous jobs ( if any )
        delilah->clearComponents();
        
        return id;
        
    }

    
    bool SamsonClient::areAllOperationsFinished()
    {
        for ( size_t i = 0 ; i < delilah_ids.size() ; i++)
        {
            size_t id = delilah_ids[i];
            
            if (delilah->isActive( id ) )
                return false;
        }
        return true;
    }
    
    void SamsonClient::waitUntilFinish()
    {
        
        for ( size_t i = 0 ; i < delilah_ids.size() ; i++)
        {
            size_t id = delilah_ids[i];
            
            while ( delilah->isActive( id ) )
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
        engine::BufferContainer tmp_buffer_container; // Temporal buffer container
        
        buffer_container.pop( queue , &tmp_buffer_container );
        
        engine::Buffer * buffer = tmp_buffer_container.getBuffer();
        
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
                    output << it_components->second->getId() << " ";
                    num_components++;
                }
            }
        }
        
        if( print_verbose && ( num_components > 0 ) )
            LM_VV(( "Push components %s" , output.str().c_str() ));
        
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
    
    bool SamsonClient::connection_ready()
    {
        return delilah->isConnectionReady();
    }

    size_t SamsonClient::getNumPendingOperations()
    {
        size_t total = 0;
        
        for ( size_t i = 0 ; i < delilah_ids.size() ; i++)
        {
            size_t id = delilah_ids[i];
            
            if ( delilah->isActive( id ) )
                total ++;
        }
        
        return total;

    }
    

}
