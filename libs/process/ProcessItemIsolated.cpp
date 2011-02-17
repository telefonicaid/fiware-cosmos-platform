#include <pthread.h>
#include <iostream>               // std::cerr
#include <assert.h>               // assert(.)
#include <signal.h>               // kill(.)
#include <stdlib.h>               // exit()

#include "logMsg.h"               // LM_*
#include "traceLevels.h"          // LmtIsolated, etc. 
#include "iomMsgAwait.h"          // iomMsgAwait
#include "ProcessItemIsolated.h"  // Own interface



#define LmtIsolated               100

namespace ss
{



	void* run_ProcessItemIsolated( void* p )
	{
		ProcessItemIsolated* tmp = (ProcessItemIsolated*)p;
		tmp->runBackgroundProcessRun();
		return NULL;
	}
	
	void ProcessItemIsolated::run()
	{
#ifdef ISOLATED_PROCESS_AS_THREAD
		LM_T( LmtIsolated , ("Isolated process %d start in thread mode",id_processItem));
#else		
		LM_T( LmtIsolated , ("Isolated process %d start in fork mode",id_processItem));
#endif
	
		init();
		
		// Create a couple of pipes to communicate both process
		
		if ( pipe(pipeFdPair1) != 0 )
		{
			setError("System error: not possible to create pipes when running this process");
			return;
		}
		if ( pipe(pipeFdPair2) != 0 )
		{
			setError("System error: not possible to create pipes when running this process");
			close( pipeFdPair1[0] );
			close( pipeFdPair1[1] );
			return;
		}
		
		LM_T( LmtIsolated , ("Isolated process %d: pipes created ",id_processItem));
		
		
		// Create the other process to run the other side
#ifdef ISOLATED_PROCESS_AS_THREAD
		ProcessItemIsolated *tmp = (this);
		pthread_t t;
		pthread_create(&t, NULL, run_ProcessItemIsolated, tmp);
#else
		
		pid_t pid = fork();
		assert( pid >= 0 );
		if( pid == 0 )	// Children running the background process
		{
			runBackgroundProcessRun();
			exit(0);
		}
#endif
		
		s = starting;
		
		// Reading from parent
		int code;
		
		while( true )
		{
			int iom;
			int nb = -9;

			LM_T(LmtIsolated, ("Isolated process %d(%s): Reading from pipe", id_processItem, stateName()));

			iom = iomMsgAwait(pipeFdPair1[0], 60*5, 0);	// 5 minuts time-out
			if (iom != 1)
				LM_E(("iomMsgAwait returned error %d", iom));
			else
			{
				nb = read( pipeFdPair1[0] , &code , sizeof(int) );
				LM_T(LmtIsolated, ("Isolated process %d(%s): read %d bytes (code %d) ",id_processItem,stateName(),nb,code));
			}

			if (nb != sizeof(int))
			{
				char errorText[256];

				LM_T(LmtIsolated, ("Isolated process %d(%s): Set error and break the loop", id_processItem,stateName()));
				if (nb == -9)
					snprintf(errorText, sizeof(errorText), "Father process seems to have crashed - iomMsgAwait returned %d", iom);
				else if (nb == -1)
					snprintf(errorText, sizeof(errorText), "Father process seems to have crashed - read(): %s", strerror(errno));
				else
					snprintf(errorText, sizeof(errorText), "Father process seems to have crashed - read returned %d instead of expected %d", nb, (int) sizeof(int));

				setError(errorText);
				s = broken;
				break;	// Not loop any more
			}
			else
			{
				switch (s)
				{
					case starting:
						assert( code == -5);
						
						// Close the unnecessary pipes
#ifndef ISOLATED_PROCESS_AS_THREAD
						LM_T( LmtIsolated , ("Isolated process %d(%s): Closing secondary fds of pipes ",id_processItem,stateName()));
						close(pipeFdPair1[1]);
						close(pipeFdPair2[0]);
#endif
						
						s = running;
						LM_T( LmtIsolated , ("Isolated process %d(%s): Go to running ",id_processItem,stateName()));
						break;
					case running:
						
						if( code == -5 )
						{
							s = finished;
							LM_T( LmtIsolated , ("Isolated process %d(%s): Go to finish ",id_processItem,stateName()));
						}
						else
						{
							LM_T( LmtIsolated , ("Isolated process %d(%s): Executing code ",id_processItem,stateName()));
							runCode( code );
							LM_T( LmtIsolated , ("Isolated process %d(%s): Executed code ",id_processItem,stateName()));
						}
					default:
						break;
				}

				// Send something to the other side of the pipe to cotinue or finish
				LM_T( LmtIsolated , ("Isolated process %d(%s): Sending something back to the pipe ",id_processItem,stateName()));
				int c=1;
				write( pipeFdPair2[1] , &c , sizeof(c) );
				LM_T( LmtIsolated , ("Isolated process %d(%s): Sending something back to the pipe... OK! ",id_processItem,stateName()));

				if( s == finished )
				{
					LM_T( LmtIsolated , ("Isolated process %d(%s): End loop since we are finished ",id_processItem,stateName()));
					break;	// Not loop any more
				}
			}
			
			// Read again from the pipe to see what is going on
			if( s != running )
			{
				LM_T( LmtIsolated , ("Isolated process %d(%s): Break the loop sice we are not running any more ",id_processItem,stateName()));
				break; // Not loop any more
			}
		}
		
		// Close the rest of pipes all pipes
		
#ifdef ISOLATED_PROCESS_AS_THREAD	// These were not closed before
		LM_T( LmtIsolated , ("Isolated process %d(%s): Closing unused side of the pipe (thread mode) ",id_processItem,stateName()));
		close(pipeFdPair1[1]);
		close(pipeFdPair2[0]);
#endif
		
		LM_T( LmtIsolated , ("Isolated process %d(%s): Closing the rest of fds of the pipe ",id_processItem,stateName()));
		close(pipeFdPair1[0]);
		close(pipeFdPair2[1]);
		
		LM_T( LmtIsolated , ("Isolated process %d(%s): finish ",id_processItem,stateName()));
		finish();
		

		setStatus( "Killing" );
		
		// Kill and wait the process
#ifndef ISOLATED_PROCESS_AS_THREAD
		/*
		int stat_loc;
		kill( pid , SIGTERM );
		pid_t p = waitpid(pid, &stat_loc , 0 );
		assert( p!= -1 );
		 */
#endif
		
	}
	
	
	// Function used inside runIsolated to send a code to the main process
	void ProcessItemIsolated::sendCode( int c )
	{
		// Write in the pipe
		int nb = write(pipeFdPair1[1], &c, sizeof(c) );
		
		if( nb != sizeof(c) )
		{
			std::cerr << "Error in background process writing to pipe";
			exit(0);
		}
		
		// Read something to continue
		nb = read(pipeFdPair2[0], &c, sizeof(c) );
		
		if( nb != sizeof(c) )
		{
			std::cerr << "Error in background process reading from pipe";
			exit(0);
		}
	}
	
	void ProcessItemIsolated::runBackgroundProcessRun()
	{

		// Close the other side of the pipes ( if it is in thread-mode, we cannot close)
#ifndef ISOLATED_PROCESS_AS_THREAD
		close(pipeFdPair1[0]);
		close(pipeFdPair2[1]);
		
		for (int i = 0 ;  i < 1024 ; i++)
			if( ( i != pipeFdPair1[1] ) && ( i!= pipeFdPair2[0] ) )
				close( i );
		
#endif	
		sendCode( -5 );		// Send an initial code so the other side can close unnecessary pipes
		
		runIsolated();

		sendCode( -5 );		// Send a finish code so the other side can close unnecessary pipes
		
		// Close the other side of the pipe
#ifndef ISOLATED_PROCESS_AS_THREAD
		close(pipeFdPair1[1]);
		close(pipeFdPair2[0]);
#endif
		
	}	
	
	
}
