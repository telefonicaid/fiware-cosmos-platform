/* ****************************************************************************
*
* FILE                     SamsonSetup.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 04 2011
*
*/
#include "Network2.h"           // Network2
#include "SamsonSetup.h"        // Own interface



namespace ss
{


/* ****************************************************************************
*
* SamsonSetup constructor - 
*/
SamsonSetup::SamsonSetup()
{
	EndpointManager* epMgr = new EndpointManager(Endpoint2::Setup);
	networkP               = new Network2(epMgr);
}



/* ****************************************************************************
*
* procVecCreate - 
*/
void SamsonSetup::procVecCreate(const char* controllerHost, int workers, const char* ip[])
{
	
}



/* ****************************************************************************
*
* connect - 
*/
Endpoint2::Status SamsonSetup::connect(void)
{
	return Endpoint2::Error;
}



/* ****************************************************************************
*
* reset - 
*/
Endpoint2::Status SamsonSetup::reset(void)
{
	return Endpoint2::Error;
}



/* ****************************************************************************
*
* processList - 
*/
Endpoint2::Status SamsonSetup::processList(void)
{
	return Endpoint2::Error;
}



/* ****************************************************************************
*
* procVecSend - 
*/
Endpoint2::Status SamsonSetup::procVecSend()
{
	return Endpoint2::Error;
}



/* ****************************************************************************
*
* run - 
*/
void SamsonSetup::run(void)
{
}

}
