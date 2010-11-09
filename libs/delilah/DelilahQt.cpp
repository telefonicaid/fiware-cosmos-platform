/*
 * Delilah.cpp
 *
 *  Created on: Oct 14, 2010
 *      Author: ania
 */

#include "DelilahQt.h"		// Own interface
#include "Delilah.h"		// ss::Delilah
#include "DelilahQtApp.h"
# include "Packet.h"		// ss::Packet

int DelilahQt::run( int argc , const char * argv[] )
{
	app = new DelilahQtApp(argc, (char**) argv);
	return app->exec();
}

int DelilahQt::receive(int fromId, ss::Message::MessageCode msgCode, ss::Packet* packet)
{
	// TODO: To be completed
	return 0;	
}
