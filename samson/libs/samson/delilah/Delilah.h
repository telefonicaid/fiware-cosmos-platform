#ifndef DELILAH_H
#define DELILAH_H

/* ****************************************************************************
 *
 * FILE                     Delilah.h
 *
 * DESCRIPTION			   Client application for Samson
 *
 */

#include <iostream>				// std::cout
#include <set>					// std::set

#include "logMsg/logMsg.h"             // lmInit, LM_*

#include "au/mutex/Token.h"				// au::Token
#include "au/mutex/TokenTaker.h"			// au::TokenTaker
#include "au/containers/map.h"				// au::map
#include "au/CommandLine.h"				// au::CommandLine
#include "au/Cronometer.h"      // au::Cronometer
#include "au/string.h"          // au::Table
#include "au/CounterCollection.h"           // au::CounterCollection

#include "au/tables/pugi.h"          // pugi::...

#include "engine/Object.h"          // engine::Object
#include "engine/Buffer.h"
#include "engine/MemoryManager.h"

#include "samson/common/samson.pb.h"			// samson::network::..
#include "samson/common/Macros.h"             // EXIT, ...
#include "samson/common/traces.h"				// TRACE_DALILAH

#include "samson/module/Environment.h"	// samson::Environment

#include "samson/network/NetworkInterface.h"			// NetworkInterface
#include "samson/network/Message.h"            // Message::MessageCode

#include "DelilahBase.h"                    // Monitorization information for delilah
#include "DelilahBaseConnection.h"

namespace  engine 
{
    class Buffer;
}

namespace samson 
{
    
    class Delilah;
	class DelilahClient;
	class DelilahComponent;
    class PushDelilahComponent;
    class PopDelilahComponent;
    class DataSource;
    
    // Interface to receive live data
    class DelilahLiveDataReceiverInterface
    {
    public:
        virtual void receive_buffer_from_queue(std::string queue , engine::Buffer* buffer )=0;
    };
    
	/**
	   Main class for the samson client element
	 */
    
	class Delilah :  public engine::Object, public DelilahBase , public DelilahBaseConnection
	{
		// Id counter of all internal DelilahComponents
		size_t id;												
		
		// Private token to protect the local list of components
		au::Token token;
        
        friend class SamsonClient;
        
    protected:
        
		// Map of components that intercept messages
		au::map<size_t , DelilahComponent> components;			
        
    public:

        // Flag to update automatically list of queues and workers
        bool automatic_update;
        
        // Interface to receive live data
        DelilahLiveDataReceiverInterface * data_receiver_interface;
        
	public:
		
		Environment environment;	// Environment properties to be sent in the next job
		
	public:
		
		Delilah( );
		~Delilah();

        // Notification system
        void notify( engine::Notification* notification );
        
		// PacketReceiverInterface
		void receive( Packet* packet );

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);

		// Add particular process that will take input parameters
        size_t addPushData( std::vector<std::string> fileNames , std::vector<std::string> queues );
        size_t addPushData( DataSource* dataSource , std::vector<std::string> queues );
        size_t addPushData( engine::Buffer* buffer , std::vector<std::string> queues );
        
        size_t addPopData( std::string queue_name , std::string fileName , bool force_flag  , bool show_flag);
		size_t sendWorkerCommand( std::string command , engine::Buffer *buffer );
		
		// Check a particular id
		bool isActive( size_t id );
        bool hasError( size_t id );
        std::string errorMessage( size_t id );
        std::string getDescription( size_t id );
        
	public:
				
		/** 
		 Methonds implemented by subclasses
		 */
		
		// Function to be implemented by sub-classes to process packets ( not handled by this class )
		virtual int _receive( Packet* packet );

        // Notification form a delilah component
        virtual void delilahComponentStartNotification( DelilahComponent *component)  { if (component == NULL) return; };
        virtual void delilahComponentFinishNotification( DelilahComponent *component) { if (component == NULL) return; };

		// Write something on screen
		virtual void showMessage( std::string message)          { LM_D(("not implemented (%s)", message.c_str())); };
		virtual void showWarningMessage( std::string message)   { LM_D(("not implemented (%s)", message.c_str())); };
		virtual void showErrorMessage( std::string message)     { LM_D(("not implemented (%s)", message.c_str())); };
        
		// Show traces  ( by default it does nothing )
		virtual void showTrace( std::string message)            { LM_D(("not implemented (%s)", message.c_str())); };
		
		// Callback to notify that a particular operation has finished
		virtual void notifyFinishOperation( size_t )
        {
        }
		
        virtual void receive_buffer_from_queue( std::string queue , engine::Buffer* buffer )
        {
            LM_W(("Buffer received from queue %s not used" , queue.c_str() ));
            buffer->release();
        }
        
        
		// Get info about the list of loads
        std::string getListOfComponents();
        
        // Recover a particular component
        DelilahComponent* getComponent( size_t delilah_id );
        
	public:
		
		void clearComponents();
        void clearAllComponents();  // Force all of them to be removed

        /*
        Status stop_repeat( size_t id );        
        Status stop_all_repeat(  );
         */
        
        // Get a list of local directory
        std::string getLsLocal( std::string pattern , bool only_queues );
        
        // Generate XML monitorization data
        void getInfo( std::ostringstream& output ); 

        bool checkXMLInfoUpdate();
        
        
        // Cancel a particuarl delilah_id
        void cancelComponent( size_t id );
        void setBackgroundComponent( size_t id );
        std::string getOutputForComponent( size_t id );
        
	protected:		
		
		size_t addComponent( DelilahComponent* component );
		
	};
}

#endif
