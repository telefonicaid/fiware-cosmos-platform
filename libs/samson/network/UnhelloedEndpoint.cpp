/* ****************************************************************************
*
* FILE                     UnhelloedEndpoint.h
*
* DESCRIPTION              Class for unhelloed endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 27 2011
*
*/
#include <unistd.h>                    // close
#include <fcntl.h>                     // F_SETFD
#include <pthread.h>                   // pthread_t

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Lmt*

#include "samson/common/status.h"
#include "Packet.h"                    // Packet
#include "EndpointManager.h"           // EndpointManager
#include "UnhelloedEndpoint.h"         // Own interface



namespace samson
{



/* ****************************************************************************
*
* UnhelloedEndpoint - 
*/
UnhelloedEndpoint::UnhelloedEndpoint
(
	EndpointManager*  _epMgr,
	Host*             _host,
	unsigned short    _port,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Unhelloed, 0, _host, _port, _rFd, _wFd)
{
}



/* ****************************************************************************
*
* ~UnhelloedEndpoint - 
*/
UnhelloedEndpoint::~UnhelloedEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Status UnhelloedEndpoint::msgTreat2(Packet* packetP)
{
	Message::HelloData*  helloP;
	Status               s;
	Endpoint2*           ep = NULL;

	switch (packetP->msgCode)
	{
	case Message::Hello:
		helloP = (Message::HelloData*) packetP->dataP;
		s = helloDataSet((Type) helloP->type, helloP->id);
		LM_T(LmtUnhelloed, ("Got a Hello %s from %s", messageType(packetP->msgType), name()));
		
		type = Unhelloed;
		if (s != OK)
		{
			LM_E(("helloDataSet error - setting endpoint in ScheduledForRemoval state"));
			stateSet(ScheduledForRemoval);
			LM_RE(s, ("Bad hello data"));
		}

		if (packetP->msgType == Message::Msg)
			helloSend(Message::Ack);

		LM_T(LmtHello, ("UnhelloedEndpoint received Hello from %s", name()));

		state = Ready;
		if ((samson::Endpoint2::Type) helloP->type == Delilah)
		{
			ep = epMgr->add((samson::Endpoint2::Type) helloP->type, id, host, port, rFd, wFd);
			ep->state = Ready;

			state = ScheduledForRemoval;
			rFd   = -1;
			wFd   = -1;
			epMgr->show("Changed Unhelloed to Delilah, The Unhelloed marked with ScheduledForRemoval", true);

			if (epMgr->me->type == Controller)
			{
				Packet* p = new Packet(Message::Id);

				++epMgr->delilahId;
				LM_T(LmtDelilahId, ("sending delilahId %d to %s", epMgr->delilahId, name()));
				ep->id = epMgr->delilahId;
				ep->nameSet(ep->type, ep->id, ep->host);

				p->dataP   = &epMgr->delilahId;
				p->dataLen = sizeof(epMgr->delilahId);
				p->msgType = Message::Evt;
				ep->send(p);
			}
		}
		else if (((samson::Endpoint2::Type) helloP->type == Worker) || ((samson::Endpoint2::Type) helloP->type == Setup))
		{
			LM_T(LmtUnhelloed, ("Time to update corresponding Worker endpoint and remove myself ..."));
			ep = epMgr->lookup((samson::Endpoint2::Type) helloP->type, host);

			if (ep == NULL)
				LM_X(1, ("No endpoint found for type '%s' and host '%s' - should be a worker", typeName(), host->name));

			ep->rFd   = rFd;
			ep->wFd   = wFd;
			ep->state = Ready;
			
			// Unhelloed to be removed but without closing any file descriptors
			state = ScheduledForRemoval;
			rFd   = -1;
			wFd   = -1;
		}
		else if ((samson::Endpoint2::Type) helloP->type == Controller)
			LM_W(("Anything to be done here (Hello from Controller)?"));
		else
			LM_X(1, ("What?"));



		//
		// Creating threads, if necessary
		//
		if (ep)
		{
			extern void* readerThread(void* vP);
			extern void* writerThread(void* vP);
			if (((epMgr->me->type == Worker) || (epMgr->me->type == Delilah)) && ((ep->type == Worker) || (ep->type == Delilah) || (ep->type == Controller)))
			{
				int  ps;
				//char semName[128];

                // Andreu: Not a pointer anymore
				//snprintf(semName, sizeof(semName), "jobQueue-%s", ep->name());
				//ep->jobQueueSem      = new au::Token(semName);
				ep->threaded         = true;

				LM_T(LmtThreads, ("Creating writer and reader threads for endpoint %s (plus jobQueueSem and jobQueueStopper)", name()));

                ep->thread_writer_running = true;
				if ((ps = pthread_create(&ep->writerId, NULL, writerThread, ep)) != 0)
				{
                    thread_writer_running = false;    
					LM_E(("Creating writer thread: pthread_create returned %d for %s", ps, name()));
					delete packetP;
					return PThreadError;
				}

                ep->thread_reader_running = true;
				if ((ps = pthread_create(&ep->readerId, NULL, readerThread, ep)) != 0)
				{
                    thread_reader_running = false;
					LM_E(("Creating reader thread: pthread_create returned %d for %s", ps, name()));
					delete packetP;
					return PThreadError;
				}
				LM_T(LmtThreads, ("Done creating threads (reader: 0x%x and writer: 0x%x)", ep->readerId, ep->writerId));
			}
		}
		epMgr->show("Unhelloed Received Hello", true);

		break;

	default:
		LM_E(("I only treat Hello messages - got a '%s', removing endpoint", messageCode(packetP->msgCode)));
		LM_E(("Setting endpoint in state ScheduledForRemoval"));
		stateSet(ScheduledForRemoval);
		return Error;
	}

	return OK;
}



/* ****************************************************************************
*
* helloDataSet - 
*/
Status UnhelloedEndpoint::helloDataSet(Type _type, int _id)
{
	if (epMgr->lookup(_type, _id) != NULL)
	{
		if ((_type != Worker) && (_type != Controller) && (_type != Delilah))
			LM_RE(Duplicated, ("Duplicated process: %s%d", typeName(_type), _id));
	}

	type = _type;
	id   = _id;

	nameSet(_type, id, host);
	LM_TODO(("Any state-change needed here ?"));

	return OK;
}



/* ****************************************************************************
*
* helloExchange - 
*/
Status UnhelloedEndpoint::helloExchange(int secs, int usecs)
{
	Message::Header       header;
	long                  dataLen  = 0;
	void*                 dataP    = NULL;
	Packet*               packetP  = new Packet(Message::Unknown);
	Message::HelloData*   helloP;
	Status                s;

	LM_T(LmtUnhelloed, ("Sending Hello Msg to %s", name()));
	helloSend(Message::Msg);

	LM_T(LmtUnhelloed, ("Awaiting reply"));
	if ((s = msgAwait(secs, usecs, "Hello Ack")) != 0)
		LM_RE(s, ("Endpoint2::msgAwait(expecting Hello): %s", status(s)));
	LM_T(LmtUnhelloed, ("Reply seems on its way in"));

	LM_T(LmtUnhelloed, ("Reading reply"));
	if ((s = receive(&header, &dataP, &dataLen, packetP)) != OK)
		LM_RE(s, ("Endpoint2::receive(expecting Hello): %s", status(s)));

	LM_T(LmtUnhelloed, ("Checking validity of reply (code: 0x%x)", header.code));
	if ((header.code != Message::Hello) || (header.type != Message::Ack))
	{
		free(dataP);
		LM_RE(Error, ("Message read not a Hello Msg (%s %s)", messageCode(header.code), messageType(header.type)));
	}

    LM_T(LmtUnhelloed, ("Adapting the KNOWN endpoints characteristics"));
	helloP = (Message::HelloData*) dataP;
	if ((s = helloDataSet((Endpoint2::Type) helloP->type, helloP->id)) != OK)
	{
		free(dataP);
		LM_RE(s, ("helloDataSet(): %s", status(s)));
	}

	free(dataP);
	LM_T(LmtUnhelloed, ("Successful Hello interchange"));
	
	return OK;
}

}
