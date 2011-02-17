
#ifndef _H_DELILAH_COMPONENT
#define _H_DELILAH_COMPONENT

#include <cstring>
#include "Message.h"		// Message::MessageCode 
#include "Packet.h"			// ss::Packet

namespace ss {

	
	class Delilah;
	
	class DelilahComponent
	{
		
	public:
		
		size_t id;
		Delilah *delilah;
		bool component_finished;		// Flag to be removed
		
		DelilahComponent();
		
		void setId( Delilah * _delilah ,  size_t _id );
		virtual void receive(int fromId, Message::MessageCode msgCode, Packet* packet)=0;
		
		virtual std::string getStatus()=0;
		
	};

	
	// Class to update local list of queues and operations for auto-completion
	
	void* DelilahUpdaterBackgroundThread( void* );
	
	class DelilahUpdater : public DelilahComponent
	{
	public:
		
		
		DelilahUpdater()
		{
			// Create a thread to send this message every secon
			pthread_t t;
			pthread_create(&t, NULL,DelilahUpdaterBackgroundThread , this);
		}
		
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);

		void run();
		
		virtual std::string getStatus()
		{
			return "Updater: local list of queues and operations";
		}
		
	};
	
	
}

#endif