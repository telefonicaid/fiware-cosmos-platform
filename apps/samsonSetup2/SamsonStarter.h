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
#include "Network2.h"           // Network2



/* ****************************************************************************
*
* SamsonStarter - 
*/
class SamsonStarter
{
public:
	SamsonStarter();
	~SamsonStarter();

	void                    procVecCreate(const char* controllerHost, int workers, const char* ip[]);
	ss::Endpoint2::Status   connect(void);
	ss::Endpoint2::Status   reset(void);               // Send a RESET to spawners
	ss::Endpoint2::Status   processList(void);
	ss::Endpoint2::Status   procVecSend();
	void                    run();

	ss::Network2*           networkP;
	int                     spawners;
};

#endif
