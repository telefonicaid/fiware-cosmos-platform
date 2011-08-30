
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

#include "au/Format.h"                          // au::Format

#include "engine/Engine.h"                      // engine::Engine
#include "engine/MemoryManager.h"               // engine::MemoryManager
#include "engine/DiskManager.h"                 // engine::DiskManager
#include "engine/ProcessManager.h"              // engine::ProcessManager

#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup

#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/network/Network2.h"
#include "samson/network/EndpointManager.h"
#include "samson/network/Packet.h"

#include "samson/delilah/Delilah.h"             // samson::Delilah

#include "SamsonClient.h"                       // Own interface

#include "samson/delilah/TXTFileSet.h"                         // samson::DataSource

namespace samson {
    
    samson::EndpointManager* epMgr     = NULL;
    samson::Network2*        networkP  = NULL;
    samson::Delilah* delilah = NULL;    
    
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
        samson::SamsonSetup::init();    

        // Change the values for this parameters
        samson::SamsonSetup::shared()->setValueForParameter("general.memory", au::str("%lu",memory) );
        samson::SamsonSetup::shared()->setValueForParameter("load.buffer_size",  au::str("%lu",load_buffer_size) );
        
        engine::Engine::init();
        engine::DiskManager::init(1);
        engine::ProcessManager::init(samson::SamsonSetup::getInt("general.num_processess"));
        engine::MemoryManager::init(samson::SamsonSetup::getUInt64("general.memory"));
        
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
       
        return true;
        
    }
    
    size_t SamsonClient::push( std::string queue , char *data , size_t length )
    {
        // Do something
        
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
                printf("Waiting %lu: %s\n" , id , description.c_str() );
#else
                printf("Waiting %d: %s\n" , id , description.c_str() );
#endif
*/              
                sleep(1);
            }
        }
        
    }


    
    
}
