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
#ifndef SAMSON_WORKER_H
#define SAMSON_WORKER_H

/* ****************************************************************************
*
* FILE                     SamsonWorker.h
*
* DESCRIPTION			   Main class for the worker element
*
*/

#include <iostream>				                // std::cout
#include <iomanip>                              // setiosflags()

#include "logMsg/logMsg.h"				        // 

#include "au/console/Console.h"
#include "au/network/RESTService.h"

#include "engine/EngineElement.h"               // samson::EngineElement

#include "samson/stream/StreamManager.h"        // samson::stream::StreamManager
#include "samson/stream/QueueTaskManager.h"     // samson::stream::QueueTaskManager

#include "samson/common/traces.h"				// Trace levels
#include "samson/common/Macros.h"				// exit(.)
#include "samson/common/samsonDirectories.h"    // SAMSON_WORKER_DEFAULT_PORT
#include "samson/common/samson.pb.h"			// samson::network::
#include "samson/common/NotificationMessages.h"

#include "samson/network/NetworkInterface.h"
#include "samson/delilah/Delilah.h"
#include "samson/worker/WorkerCommandManager.h"


namespace samson {
	
    class NetworkInterface;
    class Info;
    
    class SampleVector
    {
        // Vector of numbers
        double *values_;
        int max_num_values_;
        
    public:
        
        SampleVector( int max_num_values )
        {
            max_num_values_ = max_num_values;
            values_ = new double[max_num_values_];
        }
        
        ~SampleVector()
        {
            free( values_ );
        }        

        void push_to_sample_vector( double value )
        {
            for ( int i = 1 ; i < max_num_values_ ; i++ )
                values_[ i-1 ] = values_[ i ];
            values_[ max_num_values_ - 1 ] = value;
        }
        
        int getVector( double * v , int max_v_length )
        {
            if( max_v_length < max_num_values_ )
                return 0; // Not possible to fit here
            
            for ( int i = 0 ; i < max_num_values_ ; i++ )
                v[i] = values_[i];
            return max_num_values_;
        }
        
        int get_num_values()
        {
            return max_num_values_;
        }

    };
    
    class AccumulatedSampleVector : public SampleVector
    {
        
        // Number of samples to average to generate a new one
        int num_averge_samples_;
        
        // Accumulated number
        double accumulated_value_;   // Acumulated sum
        int num_values_;             // Number of accumulated values
        
    public:
        
        AccumulatedSampleVector( int num_averge_samples , int num_values ) : SampleVector( num_values )
        {
            num_averge_samples_ = num_averge_samples;

            // Accumulated version
            accumulated_value_ = 0;
            num_values_ = 0;
        }
        
        void push( double value )
        {
            accumulated_value_ += value;
            num_values_++;
            
            if( num_values_ >= num_averge_samples_ )
            {
                push_to_sample_vector( accumulated_value_ / (double) num_values_ );
                
                // Init again
                accumulated_value_ = 0;
                num_values_ = 0;
            }
        }
    };
    
    
    class FullSampleVector
    {
        
        AccumulatedSampleVector second;
        AccumulatedSampleVector minute;
        //AccumulatedSampleVector hour;
        //AccumulatedSampleVector day;

    public:
        
        FullSampleVector() : second( 1,60 ) , minute( 60 , 60 )// , hour( 60*60 , 24 ) , day( 60*60*24 , 30 )
        {
        }
        
        void push( double value )
        {
            second.push( value );
            minute.push( value );
            //hour.push( value );
            //day.push( value );
        }
        
        int get_num_values()
        {
            int total = 0;
            total += second.get_num_values();
            total += minute.get_num_values();
            //total += hour.get_num_values();
            //total += day.get_num_values();
            
            return total;
        }
        
        // Get the vector of values
        int getVector( double * v , int max_v_length )
        {
            
            if( max_v_length < get_num_values() )
            {
                LM_W(("Not enougth length for FullSampleVector::getVector"));
                return 0;
            }
            
            int pos = 0;
            //pos += day.getVector( &v[pos] , max_v_length - pos );
            //pos += hour.getVector( &v[pos] , max_v_length - pos );
            pos += minute.getVector( &v[pos] , max_v_length - pos );
            pos += second.getVector( &v[pos] , max_v_length - pos );
            return pos;
        }
        
        
        std::string getJson( std::string label )
        {
            /*
            {
                "label": "CPU",
                "data": [[-100, values], [-99, value]... ]
            }            
             */
            
            int num_values = get_num_values();
            double values[ num_values  ];
            int n = getVector( values , num_values );

            
            std::ostringstream output;
            output << "{";
            output << "\"label\":\"" << label << "\",";
            
            output << "\"data\":";
            output << "[";
            for ( int i = 0 ; i < n ; i++ )
            {
			   output << "[" << (i-n+1) << "," << std::setiosflags(std::ios::fixed) << std::setprecision(2) << values[i] << "]";
                if( i != (n-1) )
                    output << ",";
            }
            output << "]";
            output << "}";
            
            return output.str();
        }
    };

    
    class SamsonWorker;
    class SamsonWorkerSamples
    {
        
        SamsonWorker * samsonWorker_;
        
        FullSampleVector cpu;
        FullSampleVector memory;
        FullSampleVector disk_in;
        FullSampleVector disk_out;
        FullSampleVector net_in;
        FullSampleVector net_out;
        
    public:
        
        SamsonWorkerSamples( SamsonWorker * samsonWorker )
        {
            samsonWorker_ = samsonWorker;
        }
        
        // Take necessary samples for cpu, memory,...
        void take_samples();
        
        std::string getJson()
        {
            std::ostringstream output;
            output << "[";
            output << cpu.getJson("cpu");
            output << ",";
            output << memory.getJson("memory");
            output << ",";
            output << disk_in.getJson("disk in");
            output << ",";
            output << disk_out.getJson("disk out");
            output << ",";
            output << net_in.getJson("net in");
            output << ",";
            output << net_out.getJson("net out");
            output << "]";
            return output.str();
        }
    };
    
    
	class SamsonWorker : 
        public NetworkInterfaceReceiver, 
        public engine::Object,
        public au::Console,
        public au::network::RESTServiceInterface
	{

        // Initial time stamp for this worker
        au::Cronometer cronometer; 
        
        // Auto-client for REST interface
        Delilah* delilah;

        // REST Service
        au::network::RESTService *rest_service;

        // Sampler for REST interface
        SamsonWorkerSamples samson_worker_samples;
        
	public: 
		
		SamsonWorker( NetworkInterface* network );
        ~SamsonWorker()
        {
            LM_T(LmtCleanup, ("Deleting workerCommandManager: %p", workerCommandManager));
            delete workerCommandManager;

            LM_T(LmtCleanup, ("Deleting streamManager: %p", streamManager));
            delete streamManager;
            
            if( rest_service )
                delete rest_service;
        }

        friend class StreamManager;                     // Friend to be able to log to dataManager
        
	public:

        NetworkInterface *network;                      // Network interface to communicate with the rest of the system
				
        stream::StreamManager* streamManager;           // Manager of all the stream-processing queues in the system

        WorkerCommandManager* workerCommandManager;      // Manager of the "Worker commands"

        
	public:

		// Interface to receive Packets ( NetworkInterfaceReceiver )
		void receive( Packet* packet );
		
        // Notification from the engine about finished tasks
        void notify( engine::Notification* notification );

        // Get information for monitoring
        void getInfo( std::ostringstream& output);
        
        // au::Console ( debug mode with fg )
        void autoComplete( au::ConsoleAutoComplete* info );
        void evalCommand( std::string command );
        std::string getPrompt();

        // Send a trace to all delilahs
        void sendTrace( std::string type , std::string context , std::string message );

        // Get a collection with a single record with information for this worker...
        network::Collection* getWorkerCollection( Visualization* visualization );

        // RESTServiceInterface
        void process( au::network::RESTServiceCommand* command );
        void process_intern( au::network::RESTServiceCommand* command );
        void process_clusterNodeAdd( au::network::RESTServiceCommand* command );
        void process_clusterNodeDelete( au::network::RESTServiceCommand* command );
        void process_delilah_command( std::string delilah_command , au::network::RESTServiceCommand* command );
        void process_node( au::network::RESTServiceCommand* command );
        void process_ilogging( au::network::RESTServiceCommand* command );
        void process_logging( au::network::RESTServiceCommand* command );

        
        void stop()
        {
            // Stop the rest waiting for all connections to finish ( even the thread for the listener )
            rest_service->stop( true );
        }
        
    private:

        // Internal REST methods

        
        
	};
	
}

#endif
