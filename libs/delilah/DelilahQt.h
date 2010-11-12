/*
 * DelilahQt.h
 *
 *  Created on: Oct 20, 2010
 *      Author: ania
 */

#ifndef DELILAHQT_H_
#define DELILAHQT_H_

#include <iostream>
#include "Message.h"			// ss::Message

//class MainWindow;
class DelilahQtApp;
namespace ss {
	class Delilah;
	class Packet;
	namespace Message {
		enum MessageCode;
	}
}

class DelilahQt
{
public:

	DelilahQt(  ss::Delilah* _dalilah  )
	{
		// Keep a pointer to main dalilah interface to send messages and stuff
		dalilah = _dalilah;
		
		// This is created at run command
		app = 0;
	}
	
	DelilahQtApp* app;		// Delilah Qt application
	ss::Delilah *dalilah;	// My dalilah interface to interact with SAMSON Platform

	/**
	 Main routine to launch app
	 This routine takes thread control to run the app
	 */
	
	int run( int argc , const char * argv[] );

	/**
	 Routine asynchronously executted whene a message is received
	 */
	
	int receive(int fromId, ss::Message::MessageCode msgCode, ss::Packet* packet);

	/**
	 Asynch method to quit interface
	 */
	
	void quit()
	{
		// TODO: To be completed
	}
	
	
};

#endif /* DELILAHQT_H_ */
