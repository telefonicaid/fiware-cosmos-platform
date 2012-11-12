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
#ifndef _H_PUSH_DELILAH_COMPONENT
#define _H_PUSH_DELILAH_COMPONENT

#include <fstream>				// ifstream
#include <vector>				// std::vector
#include <set>				// std::vector
#include <iostream>			// std::cout
#include <sstream>			// std::ostringstream
#include <list>				// std::list

#include "au/string.h"			// au::Format
#include "au/ErrorManager.h"			// au::ErrorManager
#include "au/Cronometer.h"      // au::Cronometer

#include "engine/Engine.h"          // engine::Object
#include "engine/Object.h"          // engine::Object
#include "engine/DiskManager.h"     // engine::DiskManager

#include "samson/common/samson.pb.h"		// samson::network

#include "samson/delilah/Delilah.h"			// samson::Delilah
#include "DelilahComponent.h"			// samson::DelilahComponent

#include "TXTFileSet.h"		// samson::TXTFileSet


namespace samson {

	
	class Delilah;
	class Buffer;
	
	
	// All the information related with a load process
	class PushDelilahComponent : public DelilahComponent , engine::Object
	{
		
        std::set<std::string> queues;				// Name of the stream-queue we are uploading
        
		// Sumary information
        
		size_t totalSize;				// Total size to be uploaded ( all files )
		size_t processedSize;			// total size processed locally ( compressed and squeduled to the network )
		size_t uploadedSize;			// Total size of uploaded files

        bool finish_process;            // Flag to indicate that we have process all input data
        
        au::Cronometer cronometer;
		
        DataSource *dataSource;
        
        // Input txt files
        //TXTFileSet fileSet;				
        
	public:
		
		PushDelilahComponent( DataSource * _dataSource , std::string _queue  );		
        
        void addQueue( std::string  _queue );
		~PushDelilahComponent();
		
        // Function to start running
        void run();
        
        // Function to receive packets
		void receive( Packet* packet );

		// Function to get the status
		std::string getStatus();		
        
        // Notifications
        void notify( engine::Notification* notification );
        
        // Virtual in DelilahComponent
        std::string getShortDescription();
        
    private:
        
        void requestMemoryBuffer();

	};	
	
    
	// All the information related with a load process
	class BufferPushDelilahComponent : public DelilahComponent , engine::Object
	{
		
        engine::BufferContainer buffer_container;   // Internal container of the buffer while processing it
        std::set<std::string> queues;				// Name of the stream-queue we are uploading
        
	public:
		
		BufferPushDelilahComponent( engine::Buffer * buffer , std::string queue  );		
        void addQueue( std::string  _queue );
        		
        // Function to start running
        void run();
        
        // Function to receive packets
		void receive( Packet* packet );
        
		// Function to get the status
		std::string getStatus();		
        
        // Virtual in DelilahComponent
        std::string getShortDescription();
        
    private:
        
        void requestMemoryBuffer();
        
	};	
	
	
}

#endif
