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
#include "samson/network/NetworkInterface.h"   // DataReceiverInterface
#include "samson/network/Network2.h"           // Network2
#include "samson/common/Process.h"            // Process



namespace samson
{



/* ****************************************************************************
*
* SamsonSpawner - 
*/
class SamsonSpawner : public DataReceiverInterface
{
public:
	SamsonSpawner();
	~SamsonSpawner();

	int               receive(int fromId, int nb, samson::Message::Header* headerP, void* dataP);
	void              init();
	void              init(samson::ProcessVector* pv);
	void              run(void);
	int               timeoutFunction(void);
	
private:
	samson::Network2*      networkP;
	bool               restartInProgress;

	void               reset(Endpoint2* ep);
	void               processVector(Endpoint2* ep, ProcessVector* procVec);
	void               spawn(Process* process);

	void               processesStart(ProcessVector* procVec);
	void               processesTest(ProcessVector* procVec);
	void               localProcessesKill(void);
};

}

#endif
