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

#include "au/Token.h"				// au::Lock
#include "au/map.h"				// au::map
#include "au/Cronometer.h"      // au::Cronometer

#include "pugi/pugi.h"          // pugi::...

#include "samson/module/Environment.h"	// samson::Environment
#include "samson/common/samson.pb.h"			// samson::network::..

#include "engine/Object.h" // engine::Object
#include "engine/Object.h"          // engine::Object

#include "samson/common/Macros.h"             // EXIT, ...
#include "samson/common/traces.h"				// TRACE_DALILAH

#include "samson/network/Network.h"			// NetworkInterface
#include "samson/network/Message.h"            // Message::MessageCode
#include "samson/network/Endpoint.h"			// Endpoint



namespace  engine {
    class Buffer;
}

namespace samson {

    const int i_controller  = 1;    
    const int i_worker      = 1<<1;    
    const int i_delilah     = 1<<2;    
    const int i_no_title    = 1<<3;    
    
	typedef std::string(* node_to_string_function)(const pugi::xml_node&);		
    
	class DelilahClient;
	class DelilahComponent;
	class DelilahUploadComponent;
	class DelilahDownloadComponent;
    class PushDelilahComponent;
    class PopDelilahComponent;
    class DataSource;
    

    // Private token to protect the local list of components
    extern au::Token token_xml_info;
    // Global xml-based information from the system
    extern std::string xml_info;
    // General document with the content of xml_info
    extern pugi::xml_document doc;
    // Cronometer for xml_info update
    extern au::Cronometer cronometer_xml_info;
   
    
	/**
	   Main class for the samson client element
	 */

	class Delilah : public PacketReceiverInterface, public PacketSenderInterface , public engine::Object
	{
		// Id counter of the command - messages sent to controller ( commands / upload/ download )
		size_t id;												
		
		// Private token to protect the local list of components
		au::Token token;
        
    protected:
        
		// Map of components that intercept messages
		au::map<size_t , DelilahComponent> components;			
        
        bool trace_on;

        
	public:
		
		Environment environment;								// Environment properties to be sent in the next job

		NetworkInterface* network;								// Network interface
		bool              finish;								// Global flag used by all threads to detect to stop

		
		// Full information everything in samson controller ( updated with updateInfo() )
		
	public:
		
		Delilah( NetworkInterface *_network ,  bool automatic_update );
		~Delilah();
        
		void initController(std::string controller);
		void quit();

        // Notification system
        void notify( engine::Notification* notification );
        
		// PacketReceiverInterface
		void receive( Packet* packet );

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);

		// Add particular process that will take input parameters
		size_t addUploadData( std::vector<std::string> fileNames , std::string queue , bool compresion, int _max_num_threads);
		size_t addDownloadProcess( std::string queue , std::string fileName , bool show_on_screen );
        size_t addPushData( std::vector<std::string> fileNames , std::vector<std::string> queues );
        size_t addPushData( DataSource* dataSource , std::vector<std::string> queues );
        size_t addPopData( std::string queue_name , std::string fileName , bool force_flag );

		size_t sendCommand( std::string command , engine::Buffer *buffer );
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
		virtual int _receive(int fromId, Message::MessageCode msgCode, Packet* packet);

        // Notification form a delilah component
        virtual void delilahComponentStartNotification( DelilahComponent *component){};
        virtual void delilahComponentFinishNotification( DelilahComponent *component){};

		// Write something on screen
		virtual void showMessage( std::string message){};
		virtual void showWarningMessage( std::string message){};
		virtual void showErrorMessage( std::string message){};
        
		// Show traces  ( by default it does nothing )
		virtual void showTrace( std::string message){};
		
		// Callback to notify that a particular operation has finished
		virtual void notifyFinishOperation( size_t )
        {
        }
		
		// Get info about the list of loads
		std::string getListOfLoads();
        std::string getListOfComponents();
        
	protected:
		
		void clearComponents();
        void clearAllComponents();  // Force all of them to be removed
		
        
        int getUpdateSeconds();     // Get the update time
        
        void getInfo( std::ostringstream& output);

        // Generic function to get lists of informations
        std::string getStringInfo( std::string path , node_to_string_function _node_to_string_function  ,  int options );
        
        bool checkXMLInfoUpdate();
        
	private:		
		
		size_t addComponent( DelilahComponent* component );
		
	};
}

#endif
