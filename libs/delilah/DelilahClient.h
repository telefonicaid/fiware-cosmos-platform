#ifndef _H_DELILAH_CLIENT
#define _H_DELILAH_CLIENT

#include "Message.h"		// ss::Message

namespace ss
{
	
	class Packet;
	class DelilahLoadDataProcess;
	
	class DelilahClient
	{
	public:
		
		// Main run method to start
		virtual int run(int argc , const char * argv[])=0;
		
		// PacketReceiverInterface
		virtual int receive(int fromId, Message::MessageCode msgCode, Packet* packet)=0;

		// Quit 
		virtual void quit()=0;

		// Notifications for a loadData process
		virtual void loadDataConfirmation( DelilahLoadDataProcess *process)=0;
		
		
		// Write something on screen
		virtual void showMessage(std::string message)
		{
		}
		
	};
}

#endif
