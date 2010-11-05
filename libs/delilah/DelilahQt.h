/*
 * DelilahQt.h
 *
 *  Created on: Oct 20, 2010
 *      Author: ania
 */

#ifndef DELILAHQT_H_
#define DELILAHQT_H_

#include <iostream>

//class MainWindow;
class DelilahQtApp;
namespace ss {
	class Delilah;
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
	 Routine asynchronously executted whene a message from the controller is received
	 */
	void receivedMessage( size_t id , bool error , bool finished , std::string message )
	{
		// TODO: To be completed
	}

	/**
	 Asynch method to quit interface
	 */
	
	void quit()
	{
		// TODO: To be completed
	}
	
	
};

#endif /* DELILAHQT_H_ */
