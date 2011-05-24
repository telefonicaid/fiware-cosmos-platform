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
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "Packet.h"             // Packet
#include "JobQueue.h"           // Own interface



namespace ss
{



/* ****************************************************************************
*
* Constructor
*/
JobQueue::JobQueue(void)
{
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

    if (head == NULL) // List is empty
		return NULL;

	while (last->next != NULL)
	{
		prev = last;
		last = last->next;
	}
	
	prev->next = NULL;

	if (prev == head) // List contained only ONE item
		head = NULL;

	jobP = last;



	//
	// Statistics
	//
	jobs -= 1;

	if (jobP->packetP->message != NULL)
		totalSize  -= jobP->packetP->message->ByteSize();

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

	return last;
}



/* ****************************************************************************
*
* JobQueue::push - 
*/
void JobQueue::push(Job* jobP)
{
	Job* last = head;

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
