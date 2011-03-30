/* ****************************************************************************
*
* FILE                      Endpoint.cpp
*
* DESCRIPTION				Class for endpoints
*
*/
#include <string.h>             // strchr
#include <stdlib.h>             // atoi
#include <unistd.h>             // gethostname

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Packet.h"             // Packet
#include "Endpoint.h"           // Own interface



namespace ss {



/* ****************************************************************************
*
* init - 
*/
void Endpoint::init(void)
{
	this->name             = "no name";
	this->ip               = NULL;
	this->alias            = NULL;
	this->port             = 0xFFFF;
	this->rFd              = -1;
	this->wFd              = -1;
	this->state            = Unconnected;
	this->workers          = 0;
	this->type             = Unknown;
	this->status           = NULL;
	this->sender           = false;
	this->useSenderThread  = false;
	this->jobQueueHead     = NULL;
	this->helloReceived    = false;

	// Statistics
	this->msgsIn           = 0;
	this->msgsOut          = 0;
	this->msgsInErrors     = 0;
	this->msgsOutErrors    = 0;
	this->bytesIn          = 0;
	this->bytesOut         = 0;
	this->rMbps            = 0;   // Bytes per second in last read transfer
	this->rAccMbps         = 0;   // Bytes per second in all read transfers
	this->reads            = 0;   // Number of reads accounted for in 'accMbps'
	this->wMbps            = 0;   // Bytes per second in last write transfer
	this->wAccMbps         = 0;   // Bytes per second in all write transfers
	this->writes           = 0;   // Number of writes accounted for in 'wAccMbps'
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(void)
{
	LM_M(("new Endpoint: %p EMPTY", this));
	init();
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(Type type, unsigned short port)
{
	LM_T(LmtEndpoint,("new Endpoint: %p type %d and port (%d)", this, (int) type, port));
	init();

	this->type             = type;
	this->port             = port;
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(Type type, const char* alias, int nada)
{
	LM_T(LmtEndpoint,("new Endpoint: %p type %d and alias (%s)", this, (int) type, alias));
	nada = 0;
	init();

	this->type             = type;
	aliasSet(alias);
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(Type type, std::string ipAndPort)
{
	char* port = strchr((char*) ipAndPort.c_str(), ':');

	LM_T(LmtEndpoint,("new Endpoint: %p type %d and ipAndPort (%s)", this, (int) type, ipAndPort.c_str()));
	init();

	if (port == NULL)
		this->port  = 1234;
	else
	{
		*port = 0;
		++port;

		this->port  = atoi(port);
	}

	this->name  = ipAndPort;
	ipSet(ipAndPort.c_str());
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(Type type, std::string name, std::string ip, unsigned short port, int rFd, int wFd)
{
	LM_T(LmtEndpoint,("new Endpoint: %p type %d, name (%s), port (%d), rFd (%d), wFd (%d)", this, (int) type, name.c_str(), port, rFd, wFd));
	init();

	this->type             = type;
	this->name             = name;
	this->port             = port;
	this->rFd              = rFd;
	this->wFd              = wFd;
	this->state            = (rFd == -1)? Unconnected : Connected;

	ipSet(ip.c_str());
}



/* ****************************************************************************
*
* Destructor
*/
Endpoint::~Endpoint()
{
	LM_M(("destroying endpoint %p (name '%s')", this, name.c_str()));

	if (ip != NULL)
	{
		LM_M(("freeing ip '%s'", ip));
		free(ip);
		ip = NULL;
	}

	if (alias != NULL)
	{
		LM_M(("freeing alias"));
		free(alias);
		alias = NULL;
	}

	LM_M(("Done destroying endpoint"));
}



/* ****************************************************************************
*
* typeName - 
*/
const char* Endpoint::typeName(Endpoint::Type type)
{
	switch (type)
	{
	case Endpoint::Unknown:               return "Unknown";
	case Endpoint::Temporal:              return "Temporal";
	case Endpoint::Listener:              return "Listener";
	case Endpoint::Controller:            return "Controller";
	case Endpoint::Worker:                return "Worker";
	case Endpoint::CoreWorker:            return "CoreWorker";
	case Endpoint::ThreadedReader:        return "ThreadedReader";
	case Endpoint::ThreadedSender:        return "ThreadedSender";
	case Endpoint::Delilah:               return "Delilah";
	case Endpoint::WebListener:           return "WebListener";
	case Endpoint::WebWorker:             return "WebWorker";
	case Endpoint::Sender:                return "Sender";
	case Endpoint::Spawner:               return "Spawner";
	case Endpoint::Supervisor:            return "Supervisor";
	case Endpoint::Killer:                return "Killer";
	case Endpoint::Fd:                    return "Fd";
	case Endpoint::Setup:                 return "Setup";
	}

	return "UnknownType";
}



/* ****************************************************************************
*
* typeName - 
*/
const char* Endpoint::typeName(void)
{
   return typeName(this->type);
}



/* ****************************************************************************
*
* nam - 
*/
const char* Endpoint::nam(void)
{
	return name.c_str();
}



/* ****************************************************************************
*
* stateName - 
*/
const char* Endpoint::stateName(void)
{
	switch (state)
	{
	case Free:            return (char*) "Free";
	case Me:              return (char*) "ME";
	case FutureWorker:    return (char*) "FutureWorker";
	case Listening:       return (char*) "Listening";
	case Unconnected:     return (char*) "Unconnected";
	case Connected:       return (char*) "Connected";
	case Closed:          return (char*) "Closed";
	case Disconnected:    return (char*) "Disconnected";
	case Reconnecting:    return (char*) "Reconnecting";
	case Dead:            return (char*) "Dead";
	case Threaded:        return (char*) "Threaded";
	}

	return (char*) "Unknown";
}



/* ****************************************************************************
*
* reset - 
*/
void Endpoint::reset(void)
{
	if (rFd != -1)
		close(rFd);

	if ((wFd != rFd) && (wFd != -1))
		close(wFd);

	init();
}



/* ****************************************************************************
*
* jobPush - 
*/
void Endpoint::jobPush(SendJob* jobP)
{
	SendJobQueue* qP   = jobQueueHead;

	LM_T(LmtJob, ("Pushing a job"));

	if (jobQueueHead == NULL)
	{
		LM_T(LmtJob, ("Pushing first job"));
		jobQueueHead = new SendJobQueue;
		jobQueueHead->job   = jobP;
		jobQueueHead->next  = NULL;
		return;
	}

	while (qP->next != NULL)
		qP = qP->next;

	qP->next = new SendJobQueue;
	qP->next->job   = jobP;
	qP->next->next  = NULL;
}



/* ****************************************************************************
*
* jobPop - return last job in queue
*/
SendJob* Endpoint::jobPop(void)
{
	SendJobQueue*  prev = NULL;
	SendJobQueue*  qP   = jobQueueHead;
	SendJob*       jobP;

	LM_T(LmtJob, ("Popping a job?"));

	if (qP == NULL)
		return NULL;

	LM_T(LmtJob, ("Popping a job (qP == %p)", qP));
	while (qP->next != NULL)
	{
		prev = qP;
		qP   = qP->next;
	}

	jobP = qP->job;

	if (prev == NULL)  // Only one job in queue
		jobQueueHead = NULL;
	else
		prev->next = NULL;

	free(qP);
	return jobP;
}



/* ****************************************************************************
*
* jobInfo - 
*/
void Endpoint::jobInfo(int* messages, long long* dataLen)
{
	SendJobQueue* qP;
	
	*messages = 0;
	*dataLen  = 0;

	if (jobQueueHead == NULL)
		return;


	qP = jobQueueHead;
	while (qP->next != NULL)
	{
		if (qP->job->packetP == NULL)
			continue;

		*messages += 1;
		*dataLen  += qP->job->packetP->buffer->getSize();
		*dataLen  += qP->job->packetP->message->ByteSize();

        qP = qP->next;
	}
}



/* ****************************************************************************
*
* ipSet - 
*/
void Endpoint::ipSet(const char* ip)
{
	if (this->ip != NULL)
	{
		free(this->ip);
		this->ip = NULL;
	}

	if (ip == NULL)
	{
		this->ip = strdup("noip");
	}
	else
	{
		this->ip = strdup(ip);
	}
}



/* ****************************************************************************
*
* aliasGet - 
*/
char* Endpoint::aliasGet(void)
{
	return alias;
}



/* ****************************************************************************
*
* aliasSet - 
*/
void Endpoint::aliasSet(const char* alias)
{
	if (this->alias != NULL)
	{
		free(this->alias);
		this->alias = NULL;
	}

	if ((alias == NULL) || (alias[0] == 0))
		this->alias = strdup("noalias");
	else
		this->alias = strdup(alias);
}

}
