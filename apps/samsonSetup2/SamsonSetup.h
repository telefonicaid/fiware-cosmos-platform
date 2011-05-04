#ifndef SAMSON_SETUP_H
#define SAMSON_SETUP_H

/* ****************************************************************************
*
* FILE                     SamsonSetup.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 04 2011
*
*/
#include "Network2.h"           // Network2


namespace ss
{



/* ****************************************************************************
*
* SamsonSetup - 
*/
class SamsonSetup
{
public:
	SamsonSetup();
	~SamsonSetup();

	void                procVecCreate(const char* controllerHost, int workers, const char* ip[]);
	Endpoint2::Status   connect(void);
	Endpoint2::Status   reset(void);               // Send a RESET to spawners
	Endpoint2::Status   processList(void);
	Endpoint2::Status   procVecSend();
	void                run();

	Network2*           networkP;

private:
	int                 spawners;
	ProcessVector*      procVec;
};

}

#endif
