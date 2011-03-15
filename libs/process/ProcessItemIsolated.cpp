#include <pthread.h>
#include <iostream>               // std::cerr
#include <signal.h>               // kill(.)
#include <stdlib.h>               // exit()

#include "logMsg.h"               // LM_*
#include "traceLevels.h"          // LmtIsolated, etc. 
#include "iomMsgAwait.h"          // iomMsgAwait
#include "ProcessItemIsolated.h"  // Own interface


//#define ISOLATED_PROCESS_AS_THREAD



namespace ss
{
	
	ProcessItemIsolated::ProcessItemIsolated() : ProcessItem( ProcessItem::data_generator ) 	
	{
	}
	
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
		LM_T( LmtIsolated , ("Isolated process %s start in thread mode",getStatus().c_str()));
#else		
		LM_T( LmtIsolated , ("Isolated process %s start in fork mode",getStatus().c_str()));
#endif
	
		init();
		
		// Create a couple of pipes to communicate both process
		
		if ( pipe(pipeFdPair1) != 0 )
		{
			error.set("System error: not possible to create pipes when running this process");
			return;
		}
		if ( pipe(pipeFdPair2) != 0 )
		{
			error.set("System error: not possible to create pipes when running this process");
			close( pipeFdPair1[0] );
			close( pipeFdPair1[1] );
			return;
		}
		
		LM_T( LmtIsolated , ("Isolated process %s: pipes created ",getStatus().c_str()));
		
		
		// Create the other process to run the other side
#ifdef ISOLATED_PROCESS_AS_THREAD
		ProcessItemIsolated *tmp = (this);
		pthread_t t;
		pthread_create(&t, NULL, run_ProcessItemIsolated, tmp);
#else
		
		pid_t pid = fork();
		if ( pid < 0 )
			LM_X(1,("fork return an error"));
		
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

			LM_T(LmtIsolated, ("Isolated process %s(%s): Reading from pipe", getStatus().c_str(), stateName()));

			iom = iomMsgAwait(pipeFdPair1[0], 60*5, 0);	// 5 minuts time-out
			if (iom != 1)
				LM_E(("iomMsgAwait returned error %d", iom));
			else
			{
				nb = read( pipeFdPair1[0] , &message , sizeof(message) );
				LM_T(LmtIsolated, ("Isolated process %s(%s): read %d bytes (code %d) ",getStatus().c_str(),stateName(),nb,message.code));
			}

			if (nb != sizeof(InterProcessMessage))
			{
				char errorText[256];

				LM_T(LmtIsolated, ("Isolated process %s(%s): Set error and break the loop", getStatus().c_str(),stateName()));
				if (nb == -9)
					snprintf(errorText, sizeof(errorText), "Operation has crashed - iomMsgAwait returned %d", iom);
				else if (nb == -1)
					snprintf(errorText, sizeof(errorText), "Operation has crashed - read(): %s", strerror(errno));
				else
					snprintf(errorText, sizeof(errorText), "Operation has crashed - read returned %d instead of expected %d", nb, (int) sizeof(InterProcessMessage));

				error.set(errorText);
				s = broken;
				break;	// Not loop any more
			}
			else
			{
				
				// Message prepared for the answer to the isolated process
				InterProcessMessage m;
				m.code = PI_CODE_CONTINUE;
				
				
				switch (s)
				{
					case starting:
						
						if( message.code != -5)
						{
							LM_W(("Error in the protocol of the isolated process. Received a code %d instead of the expected %d", message.code , 5));
							s = broken;
							break;
						}
						
						// Close the unnecessary pipes
#ifndef ISOLATED_PROCESS_AS_THREAD
						LM_T( LmtIsolated , ("Isolated process %s(%s): Closing secondary fds of pipes ",getStatus().c_str(),stateName()));
						close(pipeFdPair1[1]);
						close(pipeFdPair2[0]);
#endif
						
						s = running;
						LM_T( LmtIsolated , ("Isolated process %s(%s): Go to running ",getStatus().c_str(),stateName()));
						break;
						
					case running:
						
						if( message.code == -5 )
						{
							s = finished;
							LM_T( LmtIsolated , ("Isolated process %s(%s): Go to finish ",getStatus().c_str(),stateName()));
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
							error.set(l->text);
							s = broken;
							break;	// Not loop any more
						}
						else
						{
							LM_T( LmtIsolated , ("Isolated process %s(%s): Executing code ",getStatus().c_str(),stateName()));
							m.code = runCode( message.code );
							LM_T( LmtIsolated , ("Isolated process %s(%s): Executed code ",getStatus().c_str(),stateName()));
						}
					default:
						break;
				}

				// Send something to the other side of the pipe to cotinue or finish
				LM_T(LmtIsolated , ("Isolated process %s(%s): Sending something back to the pipe ",getStatus().c_str(),stateName()));

				write( pipeFdPair2[1] , &m , sizeof(m) );
				LM_T( LmtIsolated , ("Isolated process %s(%s): Sending something back to the pipe... OK! ",getStatus().c_str(),stateName()));

				if( s == finished )
				{
					LM_T( LmtIsolated , ("Isolated process %s(%s): End loop since we are finished ",getStatus().c_str(),stateName()));
					break;	// Not loop any more
				}
			}
			
			// Read again from the pipe to see what is going on
			if( s != running )
			{
				LM_T( LmtIsolated , ("Isolated process %s(%s): Break the loop sice we are not running any more ",getStatus().c_str(),stateName()));
				break; // Not loop any more
			}
		}
		
		// Close the rest of pipes all pipes
		
#ifdef ISOLATED_PROCESS_AS_THREAD	// These were not closed before
		LM_T( LmtIsolated , ("Isolated process %s(%s): Closing unused side of the pipe (thread mode) ",getStatus().c_str(),stateName()));
		close(pipeFdPair1[1]);
		close(pipeFdPair2[0]);
#endif
		
		LM_T( LmtIsolated , ("Isolated process %s(%s): Closing the rest of fds of the pipe ",getStatus().c_str(),stateName()));
		close(pipeFdPair1[0]);
		close(pipeFdPair2[1]);
		
		LM_T( LmtIsolated , ("Isolated process %s(%s): finish ",getStatus().c_str(),stateName()));
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
		
		if( message.code == PI_CODE_KILL )
		{
			// Message received from the platform to kill me
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
		exit(0);
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
