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
class SamsonSpawner : public ss::DataReceiverInterface, public ss::TimeoutReceiverInterface
{
public:
	SamsonSpawner();
	~SamsonSpawner();

	void  init();
	void  run(void);

	int   receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);
	//int   endpointUpdate(ss::Endpoint* ep, ss::Endpoint2::UpdateReason reason, const char* reasonText, void* info = NULL);
	int   timeoutFunction(void);
	void  init(ss::ProcessVector* procVec);

private:
	ss::Network2*  networkP;
	void           processesStart(ProcessVector* procVec);
	void           spawnersConnect(ProcessVector* procVec, bool force = false);
	void           localProcessesKill(void);
	int            procVecTreat(Endpoint2* ep);
	void           spawnerForward(Message::MessageCode code, void* dataP = NULL, int dataLen = 0);
	void           spawn(Process* process);
};

}

#endif
