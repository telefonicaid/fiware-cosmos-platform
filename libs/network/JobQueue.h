#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

/* ****************************************************************************
*
* FILE                     JobQueue.h
*
* DESCRIPTION              Queues of pending jobs for unconnected endpoint
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 12 2011
*
*
* This class is used by the sender thread whenever its remote peer is not connected.
* The other 'local' side of the thread will be unaware about whether the peer is
* up or not, all messages meant for that peer will just be sent to the sender thread.
*/
#include "Message.h"            // Message::MessageCode, Message::MessageType



namespace ss
{



/* ****************************************************************************
*
* Forward declarations - or just to avoid an include ...
*/
class Packet;
class Endpoint2;



/* ****************************************************************************
*
* JobQueue - 
*/
class JobQueue
{
public:
	typedef struct Job
	{
		struct Job*           next;
		Endpoint2*            ep;
		Message::MessageCode  msgCode;
		Message::MessageType  msgType;
		void*                 dataP;
		int                   dataLen;
		Packet*               packetP;
	} Job;



private:
	Job*       head;
	int        jobs;
	long long  totalSize;

public:
	JobQueue();

	Job*       pop(void);   // flush the Job too ?  Or just take it out of the list ... ?
	void       push(Endpoint2* ep, Message::MessageCode code, Message::MessageType type, void* dataP, int dataLen, Packet* packetP);
	void       info(int* jobsP, long long* totalSizeP);

private:
	void       push(Job*);
};

}
#endif
