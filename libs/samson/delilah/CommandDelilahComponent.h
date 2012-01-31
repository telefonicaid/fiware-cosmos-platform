
#ifndef _H_COMMAND_DELILAH_COMPONENT
#define _H_COMMAND_DELILAH_COMPONENT

#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/Cronometer.h"          // au::CronometerSystem

#include <cstring>
#include "samson/network/Message.h"		// Message::MessageCode 
#include "samson/network/Packet.h"			// samson::Packet


namespace engine {
    class Buffer;
}

namespace samson {
	
	class Delilah;
	
	
	/**
	 Simple component created when a command is send to the controller ( waiting for answeres )
	 */

	/*
	class CommandDelilahComponent : public DelilahComponent
	{
		std::string command;
		engine::Buffer *buffer;
        
	public:
		
		CommandDelilahComponent( std::string _command , engine::Buffer *buffer );
		
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);

		void run();
		
		std::string getStatus();
		
	};
*/
	
	
	
}

#endif