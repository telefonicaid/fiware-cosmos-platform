#ifndef SAMSON_SPAWNER_H
#define SAMSON_SPAWNER_H

/* ****************************************************************************
*
* FILE                     SamsonSpawner.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 29 2011
*
*/
#include "samson/network/NetworkInterface.h"   // PacketReceiverInterface
#include "samson/network/Network2.h"           // Network2
#include "samson/common/Process.h"             // Process



namespace samson
{



/* ****************************************************************************
*
* SamsonSpawner - 
*/
class SamsonSpawner : public PacketReceiverInterface
{
public:
	SamsonSpawner();
	~SamsonSpawner();

	void         receive(Packet* packetP);
	void         init();
	void         init(ProcessVector* pv);
	void         run(void);
	int          timeoutFunction(void);
	
private:
	Network2*     networkP;
	bool          restartInProgress;

	void          reset(Endpoint2* ep);
	void          processVector(Endpoint2* ep, ProcessVector* procVec);
	void          spawn(Process* process);
	void          localProcVecGenerate(void);

	void          processesStart(ProcessVector* procVec);
	void          processesTest(ProcessVector* procVec);
	void          localProcessesKill(void);
};

}

#endif
