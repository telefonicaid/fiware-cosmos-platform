#ifndef SAMSON_STARTER_H
#define SAMSON_STARTER_H

/* ****************************************************************************
*
* FILE                     SamsonStarter.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 04 2011
*
*/
#include "samson/common/status.h"
#include "samson/network/NetworkInterface.h"   // PacketReceiverInterface
#include "samson/network/Network2.h"           // Network2



namespace samson
{



/* ****************************************************************************
*
* SamsonStarter - 
*/
class SamsonStarter : public PacketReceiverInterface
{
public:
	SamsonStarter();
	~SamsonStarter();

	void            receive(Packet* packetP);

	void            procVecCreate(const char* controllerHost, int workers, const char* ip[]);
	Status          connect(void);
	Status          reset(void);               // Send a RESET to spawners
	Status          processList(void);
	Status          procVecSend();
	void            run();

	Network2*       networkP;
	int             spawners;
};

}

#endif
