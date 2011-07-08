
#ifndef _H_DELILAH_COMPONENT
#define _H_DELILAH_COMPONENT

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
		
	public:
		
		typedef enum 
		{
			command,
			load,
			updater,
            push,
            pop,
            
		}DelilaComponentType;
		
		DelilaComponentType type;
		
		size_t id;
		Delilah *delilah;
		bool component_finished;		// Flag to be removed when indicated by user
		
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
	
	
	
}

#endif