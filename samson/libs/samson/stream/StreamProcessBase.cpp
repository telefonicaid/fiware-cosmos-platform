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


#include "logMsg/logMsg.h"                 // LM_X

#include "au/CommandLine.h"				// au::CommandLine

#include "engine/Object.h"              // engine::Object
#include "engine/Notification.h"      // engine::Notification

#include "samson/common/coding.h"					// All data definitions
#include "samson/network/Packet.h"					// samson::Packet
#include "samson/common/EnvironmentOperations.h"	// copyEnviroment
#include "samson/worker/SamsonWorker.h"			// samson::SamsonWorker
#include "samson/isolated/SharedMemoryItem.h"       // samson::SharedMemoryItem
#include "samson/common/MemoryTags.h"             // MemoryOutputNetwork

#include "PopQueue.h"                   // samson::stream::PopQueue
#include "StreamOperation.h"            // samson::stream::StreamOperation
#include "StreamProcessBase.h"			// Own interface


namespace samson {
    
    namespace stream {
        
#pragma mark ProcessItemKVGenerator
        
        StreamProcessBase::StreamProcessBase( size_t _task_id , StreamOperationBase* _streamOperation ) 
             : ProcessIsolated( _streamOperation->operation , ProcessIsolated::key_value )
        {
            
            // Get the task_id
            task_id = _task_id;
            
            // Set the order of the task
            task_order = 0;
            
            
            // Copy queue information for this task
            streamOperation = new StreamOperationBase( _streamOperation );

            // Copy the entire environment to the operation..
            std::map<std::string,std::string>::iterator it;
            for (it = streamOperation->environment.environment.begin() ; it != streamOperation->environment.environment.end() ; it++)
                operation_environment.set( it->first , it->second );
            
            /*
            LM_M(("StreamOperation %s %s --> %d %d ( workers %d ) " , streamOperation->name.c_str() , streamOperation->operation.c_str() ,
                  (int) streamOperation->input_queues.size() , (int) streamOperation->output_queues.size() , streamOperation->num_workers ));
            */
            
            // Set information about information is distributed
            setDistributionInformation( _streamOperation->distribution_information );
                
        }

        StreamProcessBase::~StreamProcessBase()
        {
            if( streamOperation )
                delete streamOperation;
        }
        

        void StreamProcessBase::processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish )
        {
            finish = false;
            // Special case for logging
            LM_T(LmtIsolatedOutputs, ("Checking output(%d) with output_queues.size(%d) to send logging for worker:%d", output, (int) streamOperation->output_queues.size(), outputWorker));
            if( output == (int) streamOperation->output_queues.size() )
            {
            	LM_M(("Sending buffer to log_queue for stream_operation(%s)", streamOperation->name.c_str()));
                sendBufferToQueue( buffer , outputWorker , au::str("log_%s", streamOperation->name.c_str() ) );
                return;
            }
            
            LM_T(LmtIsolatedOutputs,("[%s] Processing buffer %s" , streamOperation->operation.c_str(), au::str(buffer->getSize()).c_str() ));
            sendBufferToQueue( buffer , outputWorker , streamOperation->output_queues[output] );
        }
        
        void StreamProcessBase::sendBufferToQueue( engine::Buffer *buffer , int outputWorker , std::string queue_name  )
        {
            if(!buffer)
                return;
            
            if( queue_name == "null")
                return;

            // Select the target worker_id
            // ------------------------------------------------------------------------------------
            size_t target_worker_id;
            if( outputWorker == -1 )
            {
            	LM_M(("Buffer sent to myself"));
                // To my self
                target_worker_id = distribution_information.get_my_worker_id();
            }
            else
            {
            
                if ( ( outputWorker < 0 ) || ( outputWorker > (int)distribution_information.workers.size() ) )
                    LM_X(1, ("Non valid worker %d (#workers %lu) when seding buffer to queue %s" , outputWorker , distribution_information.workers.size() , queue_name.c_str() ) );
                target_worker_id = distribution_information.workers[ outputWorker ];
            }
            
            // ------------------------------------------------------------------------------------
            // Sent the packet
            // ------------------------------------------------------------------------------------
            
            Packet* packet = new Packet( Message::PushBlock );
            packet->setBuffer( buffer );    // Set the buffer of data
            packet->message->set_delilah_component_id( 0 );
            
            network::PushBlock* pb =  packet->message->mutable_push_block();
            pb->set_size( buffer->getSize() );
            
            std::vector<std::string> queue_names = au::split( queue_name , ',' );
            for ( size_t i = 0 ; i < queue_names.size() ; i++)
            {
                pb->add_queue( queue_names[i] );
            }
            
            
            // Direction to send packet
            packet->to.node_type = WorkerNode;
            packet->to.id = target_worker_id;
            
            // Send packet
            distribution_information.network->send( packet );
            
            
            // Release created packet
            packet->release();
            
            
        }
        
        
        
        void StreamProcessBase::processOutputTXTBuffer( engine::Buffer *buffer , bool finish )
        {
            finish = false;

            // LM_M(("[%s] Processing buffer %s" , streamOperation->operation.c_str(), au::str(buffer->getSize()).c_str() ));

            int output = 0;
            int outputWorker = -1;  // Send always to myself ( txt is not distributed )
            
            sendBufferToQueue( buffer , outputWorker , streamOperation->output_queues[output] );
            
        }
        
        
    }
}
