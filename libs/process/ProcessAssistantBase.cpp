
/* ****************************************************************************
 *
 * FILE                     ProcessAssistant.cpp
 *
 * AUTHOR                   Ken Zangelin
 *
 * CREATION DATE            Nov 5 2010
 *
 */
#include <time.h>                  // time
#include <pthread.h>               // pthread_t

#include "logMsg.h"                // LM_*
#include "Alarm.h"                 // ALARM
#include "processTraceLevels.h"    // LMT_*

#include "Message.h"               // Message::*
#include "iomServerOpen.h"         // iomServerOpen
#include "iomConnect.h"            // iomConnect
#include "iomAccept.h"             // iomAccept
#include "iomMsgAwait.h"           // iomMsgAwait
#include "iomMsgSend.h"            // iomMsgSend
#include "iomMsgRead.h"            // iomMsgRead
#include "Endpoint.h"              // Endpoint
#include "SamsonWorker.h"          // SamsonWorker
#include "Process.h"               // Process
#include "ProcessAssistantBase.h"      // Own interface
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "ProcessWriter.h"			// ss::ProcessAssistantOperationFramework
#include "ProcessAssistantOperationFramework.h"	// ss::ProcessAssistantOperationFramework
#include "WorkerTaskManager.h"		// ss::WorkerTaskManager

extern int logFd;


namespace ss {
	
	void* runProcessAsThread( void* _pa)
	{
		ProcessAssistantBase *pa = (ProcessAssistantBase*)_pa;
		
		Process *p = new Process( pa->_rFd , pa->_wFd );
		p->run();
		
		return NULL;
	}
	
	
#pragma mark ProcessAssistantBase
	
	/* ****************************************************************************
	 *
	 * Constructor
	 */
	
	ProcessAssistantBase::ProcessAssistantBase( int _core, WorkerTaskManager* _taskManager )
	{
		core		= _core;
		taskManager = _taskManager;

		// Get an output shared memory
		output_shm = MemoryManager::shared()->getFreeSharedMemory();
		if (output_shm == -1)
			LM_X(121, ("Not enough shared memory elements for the output of Process Assitance. Review setup or shared memory parameters on this server"));
		
		LM_T(LMT_COREWORKER, ("XXCORE: %d", core));
		
		setStatus("Init");
		
		// Start a core worker
		coreWorkerStart();
		
		setStatus("Children created");	
	}
	
	void ProcessAssistantBase::runAsThread()
	{
		pthread_t t;
		pthread_create(&t, NULL, runProcessAsThread, this);
	}
	
	
	void ProcessAssistantBase::runAsIndependentProcess()
	{
		if (fork() != 0)
		{
			close(pipeFdPair1[1]);
			close(pipeFdPair2[0]);
			return;
		}
		
		LM_T(60, ("CHILD RUNNING (rFd: %d, wFd: %d)", _rFd, _wFd));
		/* ************************************************************
		 *
		 * Setting auxiliar string for logMsg
		 */
		char auxString[16];
		
		sprintf(auxString, "core%02d", core);
		lmAux(auxString);
		
		LM_T(60, ("CHILD RUNNING (logFd == %d)", ::logFd));
		
		
#if !defined(__APPLE__)
		cpu_set_t cpuSet;
		
		CPU_ZERO(&cpuSet);
		CPU_SET(core, &cpuSet);
		if (sched_setaffinity(0, sizeof(cpuSet), &cpuSet) == -1)
			LM_XP(1, ("sched_setaffinity"));
#endif
		
		LM_T(LMT_COREWORKER, ("child %d running (pid: %d) on core %d", core, (int) getpid(), core));
		
		
		
		/* ************************************************************
		 *
		 * Core workers do not log to stdout
		 */
		lmFdUnregister(1);
		
		
		
		/* ************************************************************
		 *
		 * Close fathers fds ...
		 */
		int fd;
		
		LM_T(60, ("Close fathers file descriptors (but not mine: logFd == %d, rFd: %d, wFd: %d)", ::logFd, _rFd, _wFd));
		for (fd = 0; fd < 100; fd++)
		{
			if ((fd != ::logFd) && (fd != _rFd) && (fd != _wFd))
				close(fd);
			else
				LM_T(60, ("Not closing fd %d", fd));
		}
		
		
		
		/* ************************************************************
		 *
		 * Set progName
		 */
		progName = (char*) malloc(strlen("samsonCoreWorker_") + 10);
		if (progName == NULL)
			LM_X(1, ("samsonCoreWorker_%d died allocating: %s", getpid(), strerror(errno)));
		sprintf(progName, (char*) "samsonCoreWorker_%d", (int) getpid());
		
		LM_T(LMT_COREWORKER, ("Creating Process(rFd:%d, wFd:%d)", _rFd, _wFd));
		Process* processP = new Process(_rFd, _wFd );
		if (processP == NULL)
			LM_X(1, ("error allocating a Process"));
		
		
		processP->run();
		LM_X(1, ("Back from run - should not ever get here (coreWorker %d, pid: %d)", core, (int) getpid()));
	}			
	
	
	/* ****************************************************************************
	 *
	 * coreWorkerStart - 
	 */
	void ProcessAssistantBase::coreWorkerStart()
	{
		
		if (pipe(pipeFdPair1) != 0)  // pipeFdPair1[0] for reading for father
			LM_RVE(("pipe: %s", strerror(errno)));
		if (pipe(pipeFdPair2) != 0)  // pipeFdPair2[1] for writing for father
			LM_RVE(("pipe: %s", strerror(errno)));
		
		// Parent file descriptors
		rFd = pipeFdPair1[0];
		wFd = pipeFdPair2[1];
		
		// Child file descriptors
		_rFd = pipeFdPair2[0];
		_wFd = pipeFdPair1[1];
		
		
		//runAsIndependentProcess();
		runAsThread();
		
	}
	
	
	
	/* ****************************************************************************
	 *
	 * runCommand - 
	 */
	
	network::ProcessMessage ProcessAssistantBase::runCommand( network::ProcessMessage p )
	{
		
		LM_TODO(("Will connect to controller when ProcessAssistant uses EndpointMgr"));
		
		
		// send the commnd to the Process
		if( _write(p) == -1)
		{
			closeFds();
			coreWorkerStart();	// Restart the process again

			return processMessageWithCode( network::ProcessMessage::error );		// This is unfair since it seems that the process had an error and it is samson stuff
			
		}
		
		while( true )
		{
			network::ProcessMessage received_packet;
			
			if( _read(received_packet) == -1 )
			{
				closeFds();
				coreWorkerStart();	// Restart the process again
				return processMessageWithCode( network::ProcessMessage::crash);		
			}
				
			if( received_packet.code() == network::ProcessMessage::finished )
				return received_packet;
			
			if( received_packet.code() == network::ProcessMessage::crash )
				return received_packet;

			if( received_packet.code() == network::ProcessMessage::error )
				return received_packet;
			
			// Pass the command to the top class to see what to do with it ( probably process the output created by Process )
			receiveCommand( received_packet );
			
			// Send the continue message
			network::ProcessMessage p_continue = processMessageWithCode( network::ProcessMessage::continue_run );
			_write( p_continue );
			
		}
		
	}
}
