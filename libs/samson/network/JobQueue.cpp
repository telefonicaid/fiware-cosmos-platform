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
#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Lmt*

#include "samson/network/Packet.h"             // Packet
#include "JobQueue.h"           // Own interface



namespace samson
{



/* ****************************************************************************
*
* Constructor
*/
JobQueue::JobQueue(void)
{
	LM_T(LmtJob, ("Creating jobQueue"));
	head      = NULL;
	jobs      = 0;
	totalSize = 0;
}



/* ****************************************************************************
*
* JobQueue::pop - 
*/
JobQueue::Job* JobQueue::pop(void)
{
	Job* last  = head;
	Job* prev  = NULL;
	Job* jobP;

	LM_T(LmtJob, ("Trying to pop a Job (last: %p)", last));

    if (head == NULL) // List is empty
		return NULL;

	LM_T(LmtJob, ("Popping a job from queue"));

	while (last != NULL)
	{
		prev = last;
		last = last->next;
	}
	
	if (prev != NULL)
		prev->next = NULL;

	if (prev == head) // List contained only ONE item
		head = NULL;

	jobP = last;



	//
	// Statistics
	//
	jobs -= 1;

	LM_TODO(("I get a SIGABRT here - fix this!"));
#if 0

	//
	// output in gdb:
	//   samsonWorker2: Popping a job from queue
	//   pure virtual method called
	//   terminate called without an active exception
	//
	//   Program received signal SIGABRT, Aborted.
	//

	if (jobP->packetP->message != NULL)
		totalSize  -= jobP->packetP->message->ByteSize();
#endif

	if (jobP->packetP == NULL)
	{
		LM_W(("NULL packet pointer"));
		return last;
	}

	if (jobP->packetP->dataP != NULL)
		totalSize  -= jobP->packetP->dataLen;

	if (jobP->packetP->buffer != NULL)
		totalSize -= jobP->packetP->buffer->getSize();


	//
	// Sanity check
	//
	// A little tough to kill the program just because its statitic is incorrect, but at least 
	// in the first phase it is interesting to be notified on an incosistency here ...
	//
	// It the final version, these LM_X should be replaced by LM_W or LM_E
	//
	if (head == NULL)
	{
		if (jobs != 0)
			LM_X(1, ("job list empty but job count == %d ...", jobs));

		if (totalSize != 0)
			LM_X(1, ("job list empty but totalSize == %d ...", totalSize));
	}

	LM_T(LmtJob, ("Popping Job %p", last));
	return last;
}



/* ****************************************************************************
*
* JobQueue::push - 
*/
void JobQueue::push(Job* jobP)
{
	Job* last = head;

	LM_T(LmtJob, ("Pushing a Job"));

	LM_T(LmtJob, ("Pushing a job to queue"));

	if (head == NULL) // List is empty
	{
		head = jobP;
		last = head;
	}
	else
	{
		while (last->next != NULL)
			last = last->next;

		last->next = jobP;
	}

	jobP->next = NULL;



	//
	// Statistics
	//
	jobs += 1;

	if (jobP->packetP == NULL)
	{
		LM_W(("NULL packet pointer"));
		return;
	}

	if (jobP->packetP->message != NULL)
		totalSize  += jobP->packetP->message->ByteSize();

	if (jobP->packetP->dataP != NULL)
		totalSize  += jobP->packetP->dataLen;

	if (jobP->packetP->buffer != NULL)
		totalSize += jobP->packetP->buffer->getSize();
}



/* ****************************************************************************
*
* JobQueue::push - 
*/
void JobQueue::push(PacketSenderInterface*  psi, Packet* packetP)
{
	Job* jobP = (Job*) malloc(sizeof(Job));

	if (jobP == NULL)
		LM_X(1, ("error allocating a Job (%d bytes): %s", sizeof(Job), strerror(errno)));

	jobP->psi      = psi;
	jobP->packetP  = packetP;

	push(jobP);
}



/* ****************************************************************************
*
* JobQueue::info - 
*/
void JobQueue::info(int* jobsP, long long* totalSizeP)
{
	if (jobsP != NULL)
		*jobsP = jobs;

	if (totalSizeP != NULL)
		*totalSizeP = totalSize;
}

}
