/*
 * Delilah.cpp
 *
 *  Created on: Oct 14, 2010
 *      Author: ania
 */

#include "DelilahQt.h"		// Own interface
#include "Delilah.h"		// ss::Delilah
#include "DelilahQtApp.h"
#include "Packet.h"			// ss::Packet
#include "Message.h"		// ss::Message::MessageCode

int DelilahQt::run( int argc , const char * argv[] )
{
	app = new DelilahQtApp(argc, (char**) argv, dalilah);
	return app->exec();
}

int DelilahQt::receive(int fromId, ss::Message::MessageCode msgCode, ss::Packet* packet)
{
	// TODO: To be completed
	switch (msgCode)
	{
		case ss::Message::CommandResponse:
			app->receiveCommandResponse(fromId, packet);
			break;
		case ss::Message::HelpResponse:
			app->receiveData(packet);
			break;
		default:
			app->receiveUknownPacket(fromId, msgCode, packet);
			break;
	}
	return 0;	
}
