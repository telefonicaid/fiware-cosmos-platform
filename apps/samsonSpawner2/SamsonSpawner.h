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
#include "NetworkInterface.h"   // DataReceiverInterface
#include "Network2.h"           // Network2
#include "Process.h"            // Process



namespace ss
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

	int               receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);
	void              init();
	void              init(ss::ProcessVector* pv);
	void              run(void);
	int               timeoutFunction(void);
	
private:
	ss::Network2*      networkP;
	bool               restartInProgress;

	void               reset(Endpoint2* ep);
	Endpoint2::Status  processVector(Endpoint2* ep, ProcessVector* procVec);
	void               spawn(Process* process);

	void               processesStart(ProcessVector* procVec);
	void               localProcessesKill(void);
};

}

#endif
