#include <pthread.h>
#include <iostream>               // std::cerr
#include <assert.h>               // assert(.)
#include <signal.h>               // kill(.)
#include <stdlib.h>               // exit()

#include "logMsg.h"               // LM_*
#include "traceLevels.h"          // LmtIsolated, etc. 
#include "iomMsgAwait.h"          // iomMsgAwait
#include "ProcessItemIsolated.h"  // Own interface


//#define ISOLATED_PROCESS_AS_THREAD

#define CODE_REPORT_PROGRESS	-2
#define CODE_USER_ERROR			-3
#define CODE_TRACE				-4
#define CODE_OPERATION_CONTROL	-5

namespace ss
{
	
	typedef struct
	{
		int code;				// Code of the operation
		LogLineData logData;	// Trace information
		double progress;		// Progress report
	} InterProcessMessage;
	

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
		InterProcessMessage message;
		
		
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
				nb = read( pipeFdPair1[0] , &message , sizeof(message) );
				LM_T(LmtIsolated, ("Isolated process %d(%s): read %d bytes (code %d) ",id_processItem,stateName(),nb,message.code));
			}

			if (nb != sizeof(InterProcessMessage))
			{
				char errorText[256];

				LM_T(LmtIsolated, ("Isolated process %d(%s): Set error and break the loop", id_processItem,stateName()));
				if (nb == -9)
					snprintf(errorText, sizeof(errorText), "Operation has crashed - iomMsgAwait returned %d", iom);
				else if (nb == -1)
					snprintf(errorText, sizeof(errorText), "Operation has crashed - read(): %s", strerror(errno));
				else
					snprintf(errorText, sizeof(errorText), "Operation has crashed - read returned %d instead of expected %d", nb, (int) sizeof(InterProcessMessage));

				setError(errorText);
				s = broken;
				break;	// Not loop any more
			}
			else
			{
				switch (s)
				{
					case starting:
						
						assert( message.code == -5);
						
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
						
						if( message.code == -5 )
						{
							s = finished;
							LM_T( LmtIsolated , ("Isolated process %d(%s): Go to finish ",id_processItem,stateName()));
						}
						else if ( message.code == -4 )
						{
							LogLineData *l = &message.logData;
							if (lmOk(l->type, l->tLev) == LmsOk)
								lmOut(l->text, l->type , l->file, l->lineNo , l->fName, l->tLev , l->stre );

							// LM_T( LmtUser01 + message.trace_channel , ( message.trace_message));
							//LM_M(( "TRACE %d %s", message.trace_channel, message.trace_message)); 
						}
						else if ( message.code == CODE_REPORT_PROGRESS )
						{
							double p = message.progress;
							progress = p;
						}
						else if ( message.code == -3 ) // Error in the operation
						{
							LogLineData *l = &message.logData;
							setError(l->text);
							s = broken;
							break;	// Not loop any more
						}
						else
						{
							LM_T( LmtIsolated , ("Isolated process %d(%s): Executing code ",id_processItem,stateName()));
							runCode( message.code );
							LM_T( LmtIsolated , ("Isolated process %d(%s): Executed code ",id_processItem,stateName()));
						}
					default:
						break;
				}

				// Send something to the other side of the pipe to cotinue or finish
				LM_T(LmtIsolated , ("Isolated process %d(%s): Sending something back to the pipe ",id_processItem,stateName()));
				
				InterProcessMessage m;
				m.code = -1;

				write( pipeFdPair2[1] , &m , sizeof(m) );
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
		InterProcessMessage message;
		message.code = c;
		
		// Write in the pipe
		int nb = write(pipeFdPair1[1], &message, sizeof(message) );
		
		if( nb != sizeof(message) )
		{
			std::cerr << "Error in background process writing to pipe";
			exit(0);
		}
		
		// Read something to continue
		nb = read(pipeFdPair2[0], &message, sizeof(message) );
		
		if( nb != sizeof(message) )
		{
			std::cerr << "Error in background process reading from pipe " << "read " << nb << " bytes instead of " << sizeof(message);
			exit(0);
		}
	}

	void ProcessItemIsolated::setUserError( std::string error_message )
	{
		// Send a message to the parent reporting this error and exit
		InterProcessMessage message;
		message.code = -3; // Error in the operation

		// Copy of the message
		strncpy(message.logData.text, error_message.c_str() , sizeof(message.logData.text) );  
		
		// Write in the pipe
		write(pipeFdPair1[1], &message, sizeof(message) );
		
		// Exit this task ( in the background )
		assert( false );
		//exit(0);
		
	}
	
	
	// Function used inside runIsolated to send a code to the main process
	void ProcessItemIsolated::trace( LogLineData *logData )
	{
		InterProcessMessage message;
		message.code = -4;
		memcpy(&message.logData, logData, sizeof( LogLineData) );
		
		// Write in the pipe
		int nb = write(pipeFdPair1[1], &message, sizeof(message) );
		
		if( nb != sizeof(message) )
		{
			std::cerr << "Error in background process writing to pipe";
			exit(0);
		}
		
		// Read something to continue
		nb = read(pipeFdPair2[0], &message, sizeof(message) );
		
		if( nb != sizeof(message) )
		{
			std::cerr << "Error in background process reading from pipe " << "read " << nb << " bytes instead of " << sizeof(message);
			exit(0);
		}
	}	
	
	void ProcessItemIsolated::reportProgress( double p )
	{
		InterProcessMessage message;
		message.code = CODE_REPORT_PROGRESS;
		message.progress = p;
		
		// Write in the pipe
		int nb = write(pipeFdPair1[1], &message, sizeof(message) );
		
		if( nb != sizeof(message) )
		{
			std::cerr << "Error in background process writing to pipe";
			exit(0);
		}
		
		// Read something to continue
		nb = read(pipeFdPair2[0], &message, sizeof(message) );
		
		if( nb != sizeof(message) )
		{
			std::cerr << "Error in background process reading from pipe " << "read " << nb << " bytes instead of " << sizeof(message);
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
