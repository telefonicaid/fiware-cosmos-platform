#include <pthread.h>
#include <iostream>                 // std::cerr
#include <signal.h>                 // kill(.)
#include <stdlib.h>                 // exit()

#include "logMsg.h"                 // LM_*
#include "traceLevels.h"            // LmtIsolated, etc. 
#include "iomMsgAwait.h"            // iomMsgAwait

#include <sys/types.h>      
#include <sys/wait.h>               // waitpid()

#include "ProcessItemIsolated.h"    // Own interface
#include "MemoryManager.h"			// ss::MemoryManager
#include "SharedMemoryItem.h"           // engine::SharedMemoryItem
#include "SharedMemoryManager.h"    


namespace ss
{

    // Static variable to active the "thread" mode of the background process execution
    bool ProcessItemIsolated::isolated_process_as_tread = false;
    
	/*
	 Struct of data used to exchange information in the pipe connecting the platform
	 and the isolated process
	 */
	
	typedef struct
	{
		int code;				// Code of the operation
		LogLineData logData;	// Trace information
		double progress;		// Progress report
	} InterProcessMessage;
	
	// Function to run by the backgroudn thread
	void* run_ProcessItemIsolated( void* p )
	{
		ProcessItemIsolated* tmp = (ProcessItemIsolated*)p;
		tmp->runBackgroundProcessRun();
		return NULL;
	}
	
	ProcessItemIsolated::ProcessItemIsolated() : ProcessItem( PI_PRIORITY_NORMAL_OPERATION ) 	
	{
        // By default we have no asignation of shared memory
        shm_id = -1;
        item = NULL;
	}	

	ProcessItemIsolated::~ProcessItemIsolated()
    {
        if( shm_id != -1 )
            engine::SharedMemoryManager::shared()->releaseSharedMemoryArea( shm_id );		
        
        if ( item )
			engine::SharedMemoryManager::shared()->freeSharedMemory( item );
    }
	
    
	void ProcessItemIsolated::run()
	{
        if( isolated_process_as_tread )
            LM_T( LmtIsolated , ("Isolated process %s start in thread mode",getStatus().c_str()));
        else
            LM_T( LmtIsolated , ("Isolated process %s start in fork mode",getStatus().c_str()));
        			
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
        pid_t pid = 0;
        if( isolated_process_as_tread )
        {
            ProcessItemIsolated *tmp = (this);
            pthread_t t;
            pthread_create(&t, NULL, run_ProcessItemIsolated, tmp);
        }
        else
        {
            pid = fork();
            if ( pid < 0 )
                LM_X(1,("Fork return an error"));
            
            if( pid == 0 )	// Children running the background process
            {
                runBackgroundProcessRun();
                exit(0);
            }
        }
		
		// Set the state to the "starting" mode
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
					snprintf(errorText, sizeof(errorText), "Operation has crashed - [iomMsgAwait returned %d]", iom);
				else if (nb == -1)
					snprintf(errorText, sizeof(errorText), "Operation has crashed - [read(): %s]", strerror(errno));
				else
					snprintf(errorText, sizeof(errorText), "Operation has crashed - [ Broken pipe between background and foreground processes ]");

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
						
						if( message.code != PI_CODE_BEGIN_BACKGROUND)
						{
							LM_W(("Error in the protocol of the isolated process. Received a code %d instead of the expected %d", message.code , PI_CODE_BEGIN_BACKGROUND));
							s = broken;
							break;
						}
						
						// Close the unnecessary pipes
                        if( !isolated_process_as_tread )
                        {
                            LM_T( LmtIsolated , ("Isolated process %s(%s): Closing secondary fds of pipes ",getStatus().c_str(),stateName()));
                            close(pipeFdPair1[1]);
                            close(pipeFdPair2[0]);
                        }

							
						// Set the state to running mode
						s = running;
						LM_T( LmtIsolated , ("Isolated process %s(%s): Go to running ",getStatus().c_str(),stateName()));
						break;
						
					case running:
						
						if( message.code == PI_CODE_END_BACKGROUND )
						{
							s = finished;
							LM_T( LmtIsolated , ("Isolated process %s(%s): Go to finish ",getStatus().c_str(),stateName()));
						}
						else if ( message.code == PI_CODE_TRACE )
						{
							LogLineData *l = &message.logData;
							if (lmOk(l->type, l->tLev) == LmsOk)
								lmOut(l->text, l->type , l->file, l->lineNo , l->fName, l->tLev , l->stre );

							// LM_T( LmtUser01 + message.trace_channel , ( message.trace_message));
							//LM_M(( "TRACE %d %s", message.trace_channel, message.trace_message)); 
						}
						else if ( message.code == PI_CODE_REPORT_PROGRESS )
						{
							double p = message.progress;
							progress = p;
						}
						else if ( message.code == PI_CODE_USER_ERROR ) // Error in the operation
						{
							LogLineData *l = &message.logData;
							error.set(l->text);
							s = broken;
							break;	// Not loop any more
						}
						else
						{
							// Run axuiliar codes
							// Note: Get the return code from runCode call
							LM_T( LmtIsolated , ("Isolated process %s(%s): Executing code ",getStatus().c_str(),stateName()));
							m.code = runCode( message.code );
							LM_T( LmtIsolated , ("Isolated process %s(%s): Executed code ",getStatus().c_str(),stateName()));
						}
					default:
						break;
				}

				// Send something to the other side of the pipe to cotinue or finish
				LM_T(LmtIsolated , ("Isolated process %s(%s): Sending something back to the pipe ",getStatus().c_str(),stateName()));

				int s;
				s = write(pipeFdPair2[1], &m, sizeof(m));
				if (s != sizeof(m))
					LM_W(("written only %d bytes (wanted to write %d)", s, sizeof(m)));

				LM_T( LmtIsolated , ("Isolated process %s(%s): Sending something back to the pipe... OK! ",getStatus().c_str(),stateName()));

				if( s == finished )
				{
					LM_T( LmtIsolated , ("Isolated process %s(%s): End loop since we are finished ",getStatus().c_str(),stateName()));
					break;	// Not loop any more
				}
			}
			
			// If we are not running any more, break the loop
			if( s != running )
			{
				LM_T( LmtIsolated , ("Isolated process %s(%s): Break the loop sice we are not running any more ",getStatus().c_str(),stateName()));
				break; // Not loop any more
			}
		}
		
		// Close the rest of pipes all pipes
		
        if( isolated_process_as_tread )
        {
            LM_T( LmtIsolated , ("Isolated process %s(%s): Closing unused side of the pipe (thread mode) ",getStatus().c_str(),stateName()));
            close(pipeFdPair1[1]);
            close(pipeFdPair2[0]);
        }
		
		LM_T( LmtIsolated , ("Isolated process %s(%s): Closing the rest of fds of the pipe ",getStatus().c_str(),stateName()));
		close(pipeFdPair1[0]);
		close(pipeFdPair2[1]);
		
		LM_T( LmtIsolated , ("Isolated process %s(%s): finish ",getStatus().c_str(),stateName()));
		
		// Kill and wait the process
        if( !isolated_process_as_tread )
        {
            
            // Loop until the background thread is killed
            sub_status = "Waiting";
            s = waiting;
            int stat_loc;
            pid_t p=0;
            do
            {
                p = waitpid(pid, &stat_loc , WNOHANG );
                
                // Send a kill message if still not died
                if( p!= pid )
                {
                    kill( pid , SIGKILL );
                    
                    // Give the background process some air to die in peace
                    sleep(0.1);
                }
                
            } while (p != pid);
        }
		
		LM_T( LmtIsolated , ("Isolated process %s(%s): finish and children process killed ",getStatus().c_str(),stateName()));
        
        
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
		int nb;
		if ((nb = write(pipeFdPair1[1], &message, sizeof(message))) != sizeof(message))
			LM_W(("written only %d bytes (wanted to write %d)", nb, sizeof(message)));
		
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
		message.code = PI_CODE_REPORT_PROGRESS;
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

        if( !isolated_process_as_tread )
        {

            close(pipeFdPair1[0]);
            close(pipeFdPair2[1]);
            
            for (int i = 0 ;  i < 1024 ; i++)
                if( ( i != pipeFdPair1[1] ) && ( i!= pipeFdPair2[0] ) )
                    close( i );
        }

		sendCode( PI_CODE_BEGIN_BACKGROUND );		// Send an initial code so the other side can close unnecessary pipes
				
		runIsolated();

		sendCode( PI_CODE_END_BACKGROUND );			// Send a finish code so the other side can close unnecessary pipes
		
		// Close the other side of the pipe
        if( !isolated_process_as_tread )
        {
            close(pipeFdPair1[1]);
            close(pipeFdPair2[0]);
        }
            
	}	
	
	
	// Function to specify if we are ready to be executed of continued from a halt	
	bool ProcessItemIsolated::isReady()
	{
        if( shm_id == -1 )
        {
            // Try to get a shared memory buffer to produce output
            shm_id = engine::SharedMemoryManager::shared()->retainSharedMemoryArea();
            if( shm_id != -1 )
                item = engine::SharedMemoryManager::shared()->getSharedMemory( shm_id );
        }
        
		bool available_memory = engine::MemoryManager::shared()->availableMemoryOutput();
        
        // Return true only if output memory is available and there is a shared memory buffer for me
        return  ( available_memory && item );
	}
	
	
}
