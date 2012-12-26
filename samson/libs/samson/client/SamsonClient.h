/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

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
#include "au/mutex/Token.h"               // au::Token
#include "au/mutex/TokenTaker.h"          // au::TokenTaker
#include "au/containers/list.h"               // au::list
#include "au/containers/map.h"               // au::map
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
        std::string connection_type;            // String to describe connection with SAMSON (pop, push, console, ...)
        
        std::vector<size_t> delilah_ids;        // Delilah operation to wait for... ( mainly push opertions )
        
        samson::Delilah* delilah;               // Delilah client   
        
        BufferContainer buffer_container;       // Blocks of data received so far ( live data )
        
	public:
        
        au::rate::Rate push_rate;               // Statistics about rate
        au::rate::Rate pop_rate;                // Statistics about rate
        
        // Default constructor
        SamsonClient( std::string connection_type );
        virtual ~SamsonClient();
        
        // General init ( Init engine )
        static void general_init( size_t memory = 1000000000 , size_t load_buffer_size = 64000000 );
        static void general_close( );
        
        // Init the connection with a SAMSON cluster
        void initConnection( au::ErrorManager * error
                            , std::string samson_node 
                            , int port = SAMSON_WORKER_PORT 
                            , std::string user = "anonymous" 
                            , std::string password = "anonymous"
                            );
        
        void disconnect();
        
        // DelilahLiveDataReceiverInterface
        void receive_buffer_from_queue( std::string queue , engine::Buffer* buffer );
        
        // Push content to a particular queue
        size_t push( std::string queue , char *data , size_t length );
        size_t push( std::string queue , DataSource *data_source );
        size_t push( std::string queue , engine::Buffer * buffer );

        // Wait until all operations are finished
        void waitUntilFinish();        
        
        // Check if all operations are finished
        bool areAllOperationsFinished();
        
        // Check if connection is ready
        bool connection_ready();
        
        // Live data connection
        void connect_to_queue( std::string queue, bool flag_new , bool flag_remove );
        SamsonClientBlockInterface* getNextBlock( std::string queue );
        
        // Getting information about my connection
        std::string getInfoAboutPushConnections( bool print_verbose );
        std::string getInfoAboutDelilahComponents();
      
        // Change the interface to receive live data from SAMSON
        void set_receiver_interface( DelilahLiveDataReceiverInterface* interface );

        // Get number of operations we are waiting for...
        size_t getNumPendingOperations();
        
        std::string getStatisticsString()
        {
            return au::str("Pushed %s in %d blocs", 
                           au::str(push_rate.getTotalSize(),"B").c_str() , 
                           push_rate.getTotalNumberOfHits() );
        }
        
    };
    
}
#endif
