
#ifndef _H_DELILAH_COMPONENT
#define _H_DELILAH_COMPONENT

#include "au/ErrorManager.h"        // au::ErrorManager

#include <cstring>
#include "samson/network/Message.h"		// Message::MessageCode 
#include "samson/network/Packet.h"			// samson::Packet

namespace engine {
    class Buffer;
}

namespace samson {

	
	class Delilah;
	
	class DelilahComponent
	{

		bool component_finished;		// Flag to be removed when indicated by user
		
	public:
		
		typedef enum 
		{
			command,                // Command send to controller
			load,
			updater,
            push,
            pop,
            worker_command          // Command send to each worker
		}DelilaComponentType;
		
		DelilaComponentType type;
		
		size_t id;
		Delilah *delilah;

        au::ErrorManager error;         // Manager of the error in this operation
        
		DelilahComponent( DelilaComponentType _type);
		virtual ~DelilahComponent(){};	// Virtual destructor necessary in this class since subclasses are deleted using parent pointers
		
		void setId( Delilah * _delilah ,  size_t _id );
		virtual void receive(int fromId, Message::MessageCode msgCode, Packet* packet)=0;

        // General function to give one-line description
        virtual std::string getShortStatus()=0;
        
		// General function to give a long description status
		virtual std::string getStatus()=0;
	
        // Get main code name ( dependent on type )
        std::string getCodeName();
        
        // Description string to show messages
        std::string getDescription();

        // Check if the component is finished
        bool isComponentFinished();
        
    protected:
        
        void setComponentFinished();
        void setComponentFinishedWithError( std::string error_message );
        
	};
	
	
	/**
	 Simple component created when a command is send to the controller ( waiting for answeres )
	 */
	
	class CommandDelilahComponent : public DelilahComponent
	{
		std::string command;
		engine::Buffer *buffer;
        
	public:
		
		CommandDelilahComponent( std::string _command , engine::Buffer *buffer );
		
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);

		void run();
		
		std::string getStatus();
		std::string getShortStatus();
		
	};

	
	/**
	 Simple component created when a command is send to the controller ( waiting for answeres )
	 */
	
	class WorkerCommandDelilahComponent : public DelilahComponent
	{
		std::string command;
		engine::Buffer *buffer;
        
        int num_workers;
        int num_confirmed_workers;
        
        
	public:
		
		WorkerCommandDelilahComponent( std::string _command , engine::Buffer *buffer );
		
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);
        
		void run();
		
		std::string getStatus();
		std::string getShortStatus();
		
	};
    
	
	
}

#endif