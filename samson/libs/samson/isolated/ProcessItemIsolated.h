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
#ifndef _H_PROCESS_ITEM_ISOLATED
#define _H_PROCESS_ITEM_ISOLATED

#include "engine/ProcessItem.h"             // samson::ProcessItem
#include "samson/module/Tracer.h"           // samson::Tracer
#include "samson/module/OperationController.h"		// samson::Tracer
#include "engine/ProcessItem.h"
#include "SharedMemoryItem.h"               // engine::SharedMemoryItem
#include "samson/common/samson.pb.h"        // samson::network


namespace samson {
	
    
    
	class ProcessItemIsolated : public  engine::ProcessItem, public Tracer , public OperationController
	{
        
		// Pipes used between two process
		int pipeFdPair1[2];
		int pipeFdPair2[2];
        
    protected:
        
        std::string processItemIsolated_description;

        
	public:
		
		void run();

        static bool isolated_process_as_tread;
		
    private:
        
        void runExchangeMessages();
        
	public:
		
		
		// Constructor
		ProcessItemIsolated( std::string _description );
		
        // Destructor ( free the shared memory segment used internally )
        ~ProcessItemIsolated();
        
		// Function to be implemented ( running on a different process )
        virtual bool initProcessItemIsolated()=0;
		virtual void runIsolated() = 0;
        virtual void finishProcessItemIsolated()=0;

		// Function executed at this process side when a code is sent from the background process
		// The returned value is the code send to the isolated process back ( contunue or kill )
		virtual void runCode( int c ) = 0;

		// Function used inside runIsolated to send a code to the main process
		void sendCode( int c );
		
		// Function used indide the runIsaled to send a trace to the main process
		void trace(LogLineData *logData);

		// Function used inside the runIsolated to send progress to the main process
		void reportProgress( double p );
		void reportProgress( double p , std::string status );
		
		// Set the error and finish the task
		void setUserError( std::string message ); 

		// Generic function to send messages from process to platform
        void sendMessageProcessPlatform( samson::network::MessageProcessPlatform *message );

		
        //virtual void sendAlert( samson::network::Alert& alert )=0;
        
	public:		

		// Internal function to be executed by a separate process or thread
		void runBackgroundProcessRun();

		
        // Process a message from the background process
        // Return true if it necessary to finish
        bool processProcessPlatformMessage( samson::network::MessageProcessPlatform * message );

	};

	
};

#endif