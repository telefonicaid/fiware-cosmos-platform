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

#include "au/Token.h"				// au::Token
#include "au/TokenTaker.h"			// au::TokenTaker
#include "au/map.h"				// au::map
#include "au/CommandLine.h"				// au::CommandLine
#include "au/Cronometer.h"      // au::Cronometer
#include "au/string.h"          // au::Table
#include "au/CounterCollection.h"           // au::CounterCollection

#include "tables/pugi.h"          // pugi::...

#include "engine/Buffer.h"
#include "engine/MemoryManager.h"

#include "samson/module/Environment.h"	// samson::Environment
#include "samson/common/samson.pb.h"			// samson::network::..

#include "engine/Object.h" // engine::Object
#include "engine/Object.h"          // engine::Object

#include "samson/common/Macros.h"             // EXIT, ...
#include "samson/common/traces.h"				// TRACE_DALILAH

#include "samson/network/NetworkInterface.h"			// NetworkInterface
#include "samson/network/Message.h"            // Message::MessageCode

#include "DelilahBase.h"                    // Monitorization information for delilah

namespace  engine {
    class Buffer;
}

namespace samson
{
    class Delilah;
}

namespace samson {
    
    
	class DelilahClient;
	class DelilahComponent;
    class PushDelilahComponent;
    class PopDelilahComponent;
    class DataSource;
    
    // Interface to receive live data
    class DelilahLiveDataReceiverInterface
    {
        
    public:
        
        virtual void receive_buffer_from_queue(std::string queue , engine::Buffer* buffer)=0;
        
    };
    
	/**
	   Main class for the samson client element
	 */
    
	class Delilah : public NetworkInterfaceReceiver  , public engine::Object, public DelilahBase
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
		
		Environment environment;								// Environment properties to be sent in the next job
		bool              finish;								// Global flag used by all threads to detect to stop


        // Identifier of the next worker to send data
		int next_worker;
		
	public:
		
		Delilah( NetworkInterface *_network );
		~Delilah();
        
		void quit();

        // Notification system
        void notify( engine::Notification* notification );
        
		// PacketReceiverInterface
		void receive( Packet* packet );

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);

		// Add particular process that will take input parameters
        size_t addPushData( std::vector<std::string> fileNames , std::vector<std::string> queues );
        size_t addPushData( DataSource* dataSource , std::vector<std::string> queues );
        size_t addPopData( std::string queue_name , std::string fileName , bool force_flag  , bool show_flag);

		size_t sendWorkerCommand( std::string command , engine::Buffer *buffer );
		
		
		// Check a particular if
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
        virtual void delilahComponentStartNotification( DelilahComponent *component)  { if (component == NULL) return: };
        virtual void delilahComponentFinishNotification( DelilahComponent *component) { if (component == NULL) return: };

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
            engine::MemoryManager::shared()->destroyBuffer(buffer);
        }
        
        
		// Get info about the list of loads
        std::string getListOfComponents();
        
        // Get next worker_id to send data...
        size_t getNextWorkerId();
        
	protected:
		
		void clearComponents();
        void clearAllComponents();  // Force all of them to be removed
        
        Status stop_repeat( size_t id );        
        Status stop_all_repeat(  );
        
        // Get a list of local directory
        std::string getLsLocal( std::string pattern , bool only_queues );
        
        // Generate XML monitorization data
        void getInfo( std::ostringstream& output ); 

        bool checkXMLInfoUpdate();
        
	protected:		
		
		size_t addComponent( DelilahComponent* component );
		
	};
}

#endif
