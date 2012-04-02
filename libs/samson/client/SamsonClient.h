
/* ****************************************************************************
 *
 * FILE            SamsonClient.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
 *
 * DATE            7/14/11
 *
 * DESCRIPTION
 *
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_Samson_SamsonClient
#define _H_Samson_SamsonClient

//We need to undef the macro "emit" because Qt library was setting it to empty string
#undef emit

#include <string>       // std::string
#include <vector>       // std::vector
#include <pthread.h>        /* pthread*/

#include "samson/common/ports.h"

#include "au/Cronometer.h"          // au::Cro
#include "au/Token.h"               // au::Token
#include "au/TokenTaker.h"          // au::TokenTaker
#include "au/list.h"               // au::list
#include "au/map.h"               // au::map
#include "au/Rate.h"               // au::rate::Rate

#include "engine/Object.h"          // engine::Object
#include "engine/Buffer.h"          // engine::Buffer
#include "engine/MemoryManager.h"   // engine::MemoryManager

#include "samson/common/coding.h"
#include "samson/network/DelilahNetwork.h"
#include "samson/delilah/Delilah.h"             // samson::Delilah
#include "samson/delilah/DelilahComponent.h"    // samson::DelilahComponent

#include "samson/module/ModulesManager.h"

#include "samson/client/BufferContainer.h"
#include "samson/client/SamsonClientBlockInterface.h"
#include "samson/client/SamsonClientBlock.h"


/*
 Main class to connect to a samson cluster
 */

namespace  samson {
    
    class SamsonClient : public DelilahLiveDataReceiverInterface
    {
                    
        std::string error_message;              // Error message if a particular operation fails
        
        std::vector<size_t> delilah_ids;        // Delilah operation to wait for...
        
        std::string connection_type;            // String to describe connection with SAMSON (pop, push, console, ...)
        
        samson::DelilahNetwork* networkP;       // Network interface for delilah client
        samson::Delilah* delilah;                // Delilah client   
        BufferContainer buffer_container;            // Blocks Container for live data
        
	public:

        au::rate::Rate push_rate;                    // Statistics about rate
        au::rate::Rate pop_rate;                    // Statistics about rate
        
        // Default constructor
        SamsonClient( std::string connection_type );
        
        static void general_init( size_t memory = 1000000000 , size_t load_buffer_size = 64000000 );
        
        // Init the connection with a SAMSON cluster
        bool initConnection( std::string samson_node 
                  , int port = SAMSON_WORKER_PORT 
                  , std::string user = "anonymous" 
                  , std::string password = "anonymous");
        
        // DelilahLiveDataReceiverInterface
        virtual void receive_buffer_from_queue(std::string queue , engine::Buffer* buffer);
        
        // Push content to a particular queue
        size_t push( std::string queue , char *data , size_t length );

        // Push a generic data source 
        size_t push( std::string queue , DataSource *data_source );

        // Get error message
        std::string getErrorMessage();
        
        // Wait until all operations are finished
        void waitUntilFinish();        
        
        // Check if all operations are finished
        bool areAllOperationsFinished();
        
        bool connection_ready();
        
        // Live data connection
        void connect_to_queue( std::string queue, bool flag_new , bool flag_remove );
        SamsonClientBlockInterface* getNextBlock( std::string queue );
        
        // Getting information about my connection
        std::string getInfoAboutPushConnections( bool print_verbose );
        std::string getInfoAboutDelilahComponents();
      
        std::string getStatisticsString()
        {
            return au::str("Pushed %s in %d blocs", au::str(push_rate.getTotalSize(),"B").c_str() , push_rate.getTotalNumberOfHits() );
        }
        
    };
    
}
#endif
