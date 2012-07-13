#include <pthread.h>
#include <iostream>                 // std::cerr
#include <signal.h>                 // kill(.)
#include <stdlib.h>                 // exit()
#include <sys/types.h>      
#include <sys/wait.h>               // waitpid()

#include "logMsg/logMsg.h"                 // LM_*
#include "logMsg/traceLevels.h"            // LmtIsolated, etc. 

#include "au/gpb.h"                     // au::readGPB & au::writeGPB
#include "au/ErrorManager.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/ThreadManager.h"
#include "au/containers/set.h"
#include "au/log/LogToServer.h"

#include "engine/MemoryManager.h"			// engine::MemoryManager
#include "engine/Notification.h"            // engine::Notification

#include "samson/common/traces.h"                       // logFd
#include "samson/common/SamsonSetup.h"                // Goyo: ss:SamsonSetup
#include "samson/isolated/ProcessIsolated.h"         //Goyo: For defines WORKER_TASK_ITEM_CODE_FLUSH_BUFFER
#include "samson/common/NotificationMessages.h"     // notification_samson_worker_send_trace
#include "samson/common/MemoryTags.h"                 // MemoryInput , MemoryOutputNetwork, ...

#include "samson/network/Packet.h"                     // samson::Packet


#include "samson/isolated/ProcessItemIsolated.h"    // Own interface
#include "samson/isolated/SharedMemoryManager.h"    

#include "SharedMemoryItem.h"           // engine::SharedMemoryItem


namespace samson
{
    
    class PidElement
    {
        pid_t pid;
        au::Cronometer cronometer;
        int kill_count;
        
    public:
        
        PidElement( pid_t _pid )
        {
            pid = _pid;
            kill_count = 0;
        }
        
        bool wait()
        {
            int stat_loc;
            pid_t p = waitpid(pid, &stat_loc , WNOHANG );
            if( p == pid)
            {
                if ( WIFEXITED(stat_loc) )
                {
                    int s = WEXITSTATUS( stat_loc );
                    LM_T(LmtIsolated,("Background process (pid=%d) ended with exit with code %d",pid, s));
                    
                }
                else if( WIFSIGNALED( stat_loc ) )
                {
                    int s = WTERMSIG( stat_loc );
                    LM_T(LmtIsolated,("Background process (pid=%d) ended with signal with signal %d",pid, s));
                    LM_E(("Background process (pid=%d) ended with signal with signal %d",pid, s));
                }
                else
                {
                    LM_T(LmtIsolated,("Background process (pid=%d) 'crashed' with unknown reason", pid));
                    LM_E(("Background process (pid=%d) 'crashed' with unknown reason", pid));
                }
                
                
                return true;
            }

            if( cronometer.diffTime() > 3.0 ) // Three seconds to die
            {
                // Send a kill signal to this process
                LM_T(LmtIsolated,("Killing background process (%d) manually", pid));
                kill( pid , SIGKILL );
                
                kill_count++;
                cronometer.reset();
                
                if( kill_count > 3 )
                    LM_W(("Background process (pid %d) is not dying after %d kills" , pid , kill_count ));
                
            }
            
            
            
            return false;
        }
        
    };

    class PidCollection
    {
        au::Token token;
        au::set<PidElement> pids;
        
    public:
        
        PidCollection() : token("PidCollection")
        {
            
        }
        
        ~PidCollection()
        {
            // Remove pending information
            if( pids.size() > 0 )
            {
                LM_W(("Removing information about %lu background processes. Still waiting to finish in PidCollection" , pids.size() ));
                pids.clearSet();
            }
        }
        
        void add( pid_t pid )
        {
            au::TokenTaker tt(&token);
            pids.insert( new PidElement(pid) );
            
            if( pids.size() > 20 )
                LM_W(("Waiting a high number of background process %d" , (int)  pids.size() ));
                                  
        }
        
        void clear()
        {
            au::TokenTaker tt(&token);
            
            std::set<PidElement*>::iterator it_pids;

            for ( it_pids = pids.begin() ; it_pids != pids.end() ;  )
            {
                PidElement* pid_element = (*it_pids);
                
                if( pid_element->wait() )
                {
                    delete pid_element;
                    pids.erase( it_pids++ );
                }
                else
                    ++it_pids;
                
            }
        }
        
        
    };
    
    
    PidCollection myPidCollection;
    
    
    // Static variable to active the "thread" mode of the background process execution
    bool ProcessItemIsolated::isolated_process_as_tread = false;
    	
	
	// Function to run by the backgroudn thread
	void* run_ProcessItemIsolated( void* p )
	{
        // Free resources automatically when this thread finish
        pthread_detach(pthread_self());
        
		ProcessItemIsolated* tmp = (ProcessItemIsolated*)p;
		tmp->runBackgroundProcessRun();
		return NULL;
	}
	
	ProcessItemIsolated::ProcessItemIsolated( std::string _description ) : ProcessItem( PI_PRIORITY_NORMAL_OPERATION ) 	
	{
        processItemIsolated_description = _description;
	}	

	ProcessItemIsolated::~ProcessItemIsolated()
    {
    }
	
    
	void ProcessItemIsolated::run()
	{
        
        LM_T( LmtIsolated , ("Isolated process %s: start ******************************************* ",getStatus().c_str()));
        
        if( isolated_process_as_tread )
            LM_T( LmtIsolated , ("Isolated process %s start in thread mode",getStatus().c_str()));
        else
            LM_T( LmtIsolated , ("Isolated process %s start in fork mode",getStatus().c_str()));
        			
		// Create a couple of pipes to communicate both process
		if ( pipe(pipeFdPair1) != 0 )
		{
			LM_E(("System error: not possible to create pipes when running this process"));
			error.set("System error: not possible to create pipes when running this process");
			return;
		}
		if ( pipe(pipeFdPair2) != 0 )
		{
			LM_E(("System error: not possible to create pipes when running this process"));
			error.set("System error: not possible to create pipes when running this process");
			LM_T( LmtFileDescriptors, ("Isolated process %s: pipes closed: pipeFdPair1[0]:%d, pipeFdPair1[1]:%d\n", pipeFdPair1[0], pipeFdPair1[1]));
			close( pipeFdPair1[0] );
			close( pipeFdPair1[1] );
			return;
		}
		
		LM_T(LmtFileDescriptors, ("Isolated process %s: pipes created. pipeFdPair1[0]:%d, pipeFdPair1[1]:%d, pipeFdPair2[0]:%d, pipeFdPair2[1]:%d\n",getStatus().c_str(), pipeFdPair1[0], pipeFdPair1[1], pipeFdPair2[0], pipeFdPair2[1]));
		LM_T( LmtIsolated , ("Isolated process %s: pipes created ",getStatus().c_str()));
		
        
        // Init isolated stuff
        if( !initProcessItemIsolated( ) )
        {
            error.set( au::str("Not possible to init an isolated item for operation %s", operation_name.c_str() ));
            LM_W(("ProcessItemIsolated not executed since init return false"));
            return;
        }
        
		// Create the other process to run the other side
        pid_t pid = 0;
        if( isolated_process_as_tread )
        {
            ProcessItemIsolated *tmp = (this);
            pthread_t t;
            au::ThreadManager::shared()->addThread("ProcessItemIsolated::run"
                                               ,&t, NULL, run_ProcessItemIsolated, tmp);
        }
        else
        {
        	LM_T( LmtIsolated , ("Isolated process %s: father about to fork",getStatus().c_str()));
            pid = fork();
            if ( pid < 0 )
                LM_X(1,("Fork return an error"));
            
            if( pid == 0 )	// Children running the background process
            {
                runBackgroundProcessRun();
                LM_T(LmtIsolated,("Child in Background process finished, calling _exit that will close its side pipes: pipeFdPair1[1]:%d, pipeFdPair2[0]:%d\n", pipeFdPair1[1], pipeFdPair2[0]));
                _exit(1000);
            }
        }
		
        // Exchange all the necessary messages between background and foreground process
        LM_T(LmtIsolated,  ("Isolated process %s: father runExchangeMessages start, child pid=%d ",getStatus().c_str(), pid));
        
        runExchangeMessages();
        
        LM_T( LmtIsolated , ("Isolated process %s: father runExchangeMessages finish ",getStatus().c_str()));
		
		// Close the rest of pipes all pipes
        if( isolated_process_as_tread )
        {
            sleep(1);  
            LM_T( LmtFileDescriptors , ("Isolated process %s: Closing unused side of the pipe (thread mode) pipeFdPair1[1]:%d, pipeFdPair2[0]:%d ",getStatus().c_str(), pipeFdPair1[1], pipeFdPair2[0]));
            close(pipeFdPair1[1]);
            close(pipeFdPair2[0]);
        }
		
		LM_T( LmtFileDescriptors , ("Isolated process %s: Closing the rest of fds of the pipe pipeFdPair1[0]:%d, pipeFdPair2[1]:%d ",getStatus().c_str(), pipeFdPair1[0], pipeFdPair2[1]));
		close(pipeFdPair1[0]);
		close(pipeFdPair2[1]);
		
		LM_T( LmtIsolated , ("Isolated process %s: waiting child pid=%d to finish ",getStatus().c_str(), pid));
		
		// Kill and wait the process
        if( !isolated_process_as_tread )
        {
            
            myPidCollection.add(pid);
            myPidCollection.clear();
        }
		
        LM_T( LmtIsolated , ("Isolated process %s: Finish ******************************************************************************************* ",getStatus().c_str()));
        
        // Finish process item
        finishProcessItemIsolated( );
        
	}
    
    bool ProcessItemIsolated::processProcessPlatformMessage( samson::network::MessageProcessPlatform * message )
    {
        
        // Eval if the process has been canceled by ProcessManager, if so, it should return assap
        
        if( isProcessItemCanceled() )
        {
            error.set( "ProcessItem canceled");
            LM_W(("ProcessItem canceled, operation:%d",  message->operation() ));
            
            // Send a kill message and finish
            samson::network::MessagePlatformProcess * response = new samson::network::MessagePlatformProcess();
            response->set_code( samson::network::MessagePlatformProcess_Code_code_kill );
            LM_T(LmtIsolated, ("Writing cancel response on pipeFdPair2[1]:%d", pipeFdPair2[1]));
            if (au::writeGPB(pipeFdPair2[1], response) != au::OK)
            {
                LM_E(("Error sending cancel response, code(%d),  (pipeFdPair2[1]:%d) ",  response->code(), pipeFdPair2[1]));
            }
            delete response;
            
            // Finish since it has been canceled
            return true;
        }
        
        
        
        // Process message
        
        switch (  message->code()  )
        {
            case samson::network::MessageProcessPlatform_Code_code_operation:
            {
                int operation  = message->operation();
                
                LM_T( LmtIsolated , ("Isolated process %s: Message to run operation  %d ",getStatus().c_str() , operation ));
                
                runCode( operation );
                
                LM_T( LmtIsolated , ("Isolated process %s: runCode() returned from operation %d, preparing to send continue to pipeFdPair2[1]:%d",getStatus().c_str() , operation, pipeFdPair2[1] ));
                
                // Send the continue
                samson::network::MessagePlatformProcess * response = new samson::network::MessagePlatformProcess();
                LM_T( LmtIsolated , ("Isolated process %s: response created ", getStatus().c_str()));
                response->set_code( samson::network::MessagePlatformProcess_Code_code_ok );
                LM_T( LmtIsolated , ("Isolated process %s: send the continue on pipeFdPair2[1]:%d ",getStatus().c_str() , pipeFdPair2[1] ));
                if (au::writeGPB(pipeFdPair2[1], response) != au::OK)
                {
                    LM_E(("Error sending message to run operation(%d), code(%d),  (pipeFdPair2[1]:%d) ",  operation, response->code(), pipeFdPair2[1]));
                }
                delete response;
                
                // Not finish the process
                return false;
            }
                break;
                
                break;
                
            case samson::network::MessageProcessPlatform_Code_code_progress:
            {
                LM_T( LmtIsolated , ("Isolated process %s: Message reporting progress %f ",getStatus().c_str() , message->progress() ));
                
                // set the progress and the progress status
                progress = message->progress();
                sub_status = message->progress_status();
                                
                // Send the continue
                samson::network::MessagePlatformProcess * response = new samson::network::MessagePlatformProcess();
                response->set_code( samson::network::MessagePlatformProcess_Code_code_ok );
                LM_T(LmtIsolated, ("Writing reporting message on pipeFdPair2[1]:%d", pipeFdPair2[1]));
                if (au::writeGPB(pipeFdPair2[1], response) != au::OK)
		{
		    LM_E(("Error sending progress report, code(%d),  (pipeFdPair2[1]:%d) ",  response->code(), pipeFdPair2[1]));
		}
                delete response;                    
                
                // Not finish the process
                return false;

            }
                break;
                
                
            case samson::network::MessageProcessPlatform_Code_code_user_error:
            {
                LM_T( LmtIsolated , ("Isolated process %s: Message reporting user error  ",getStatus().c_str() ));
                LM_E(("User generated error at operation %s received %s", operation_name.c_str() , message->error().c_str() ));
                
                // Set the error
                if( message->has_error() )
                    error.set( message->error() );
                else
                    error.set( "Undefined user-defined error" );
                
                // Send an ok back, and return
                samson::network::MessagePlatformProcess * response = new samson::network::MessagePlatformProcess();
                response->set_code( samson::network::MessagePlatformProcess_Code_code_ok );
                LM_T(LmtIsolated, ("Writing user error on pipeFdPair2[1]:%d", pipeFdPair2[1]));
                if (au::writeGPB(pipeFdPair2[1], response) != au::OK)
		{
		    LM_E(("Error sending user error, code(%d),  (pipeFdPair2[1]:%d), error message:%s ",  response->code(), pipeFdPair2[1], error.getMessage().c_str()));
		}
                delete response;

                // It has to finish since the background process has notified the error
                return true;
                
            }
                break;
                
            case samson::network::MessageProcessPlatform_Code_code_begin:
            {
                LM_X(1,("Received another code begin in an Isolated process"));
                return false; // Never got here...
            }
                break;
                
            case samson::network::MessageProcessPlatform_Code_code_end:
            {
                LM_T( LmtIsolated , ("Isolated process %s: Message reporting finish process  ",getStatus().c_str() ));
                
                // Send an ok back, and return
                
                samson::network::MessagePlatformProcess * response = new samson::network::MessagePlatformProcess();
                response->set_code( samson::network::MessagePlatformProcess_Code_code_ok );
                LM_T(LmtIsolated, ("Writing finish message on pipeFdPair2[1]:%d", pipeFdPair2[1]));
                if (au::writeGPB(pipeFdPair2[1], response) != au::OK)
		{
		    LM_E(("Error sending finish process, code(%d),  (pipeFdPair2[1]:%d) ",  response->code(), pipeFdPair2[1]));
		}
                delete response;
                
                // It has to finish since it has received the last message from the background process
                return true;
                
            }
                break;
        }
        
        LM_X(1, ("Internal error"));
        return false;
    }

    
    void ProcessItemIsolated::runExchangeMessages()
    {
        
        // First, read the "begin" message
        {
            // Read a message from the process
            samson::network::MessageProcessPlatform * message;
            // No timeout since "SAMSON code" is executed on the other side ( Observed long delays in high-load scenario )
            au::Status c = au::readGPB( pipeFdPair1[0] , &message, -1 );
            
            if ( c != au::OK )
            {
                LM_E( ("Isolated process %s: Problems reading the 'begin' message [error code '%s'] ",getStatus().c_str() , au::status(c)));
                error.set( au::str( "Problems starting background process '%s'", au::status(c)) );
                return;     // Problem with this read
            }
            
            if( !message )
                LM_X(1, ("Internal error"));

            if( message->code() != samson::network::MessageProcessPlatform_Code_code_begin )
            {
                error.set( au::str( "Problems starting background process since received code is not the expected 'protocol begin'" ));
                return;
            }

            delete message;
        
            // Close the unnecessary pipes
            if( !isolated_process_as_tread )
            {
                LM_T( LmtFileDescriptors , ("Isolated process %s: Closing secondary fds of pipes, pipeFdPair1[1]:%d, pipeFdPair2[0]:%d ",getStatus().c_str(), pipeFdPair1[1], pipeFdPair2[0]));
                close(pipeFdPair1[1]);
                close(pipeFdPair2[0]);
            }
            
            
            // Send the continue
            {
                samson::network::MessagePlatformProcess * response = new samson::network::MessagePlatformProcess();
                response->set_code( samson::network::MessagePlatformProcess_Code_code_ok );
                LM_T(LmtIsolated, ("Writing exchange message on pipeFdPair2[1]:%d", pipeFdPair2[1]));
                if (au::writeGPB(pipeFdPair2[1], response) != au::OK)
                {
                    LM_E(("Error sending exchange message, code(%d),  (pipeFdPair2[1]:%d) ",  response->code(), pipeFdPair2[1]));
                    error.set( au::str( "Error in protocol between platform and background process (answering begin)" ));
                    return;
                }
                delete response;
            }
            
            
        }
        
        LM_T( LmtIsolated , ("Isolated process %s: begin message received. Starting the continuous loop... ",getStatus().c_str()));
        
        // Continuous read of messages and perform required actions
        
		while( true )
		{
            
            
			// Take the timeout for reading operations from the other site
			int timeout_setup = SamsonSetup::shared()->getInt("isolated.timeout");
            
			LM_T(LmtIsolated, ("Isolated process %s: Reading a new message with timeout %d", getStatus().c_str() , timeout_setup));
            
            // Read a message from the process
            samson::network::MessageProcessPlatform * message;
            au::Status c = au::readGPB( pipeFdPair1[0] , &message, timeout_setup );
            
            if( c != au::OK )
            {
                // Not possible to read the message for any reason
				LM_E(("Isolated process op:'%s', %s: Not possible to read a message from pipeFdPair1[0]:%d with error_code' %s' (time_out:%d)", processItemIsolated_description.c_str() ,getStatus().c_str() , pipeFdPair1[0], au::status(c), timeout_setup ));
                
				error.set( au::str( "Third party operation '%s' has crashed - [ Error code %s ]", processItemIsolated_description.c_str() ,  au::status(c) ) );
                return;
            }
            
            if( !message )
                LM_X(1, ("Internal error"));

            if( processProcessPlatformMessage(message) )
            {
                delete message;
                return;
            }
            
            // Just remove the message and come back to receive a new message from the background process
            delete message;
            
        }

    }

    
    // Generic function to send messages from process to platform
    void ProcessItemIsolated::sendMessageProcessPlatform(samson::network::MessageProcessPlatform *message )
    {
         
    	LM_T(LmtIsolated,("Background process: Sending a message from process to platform on pipeFdPair1[1]:%d", pipeFdPair1[1]));
        
        // Write the message
        au::Status write_ans = au::writeGPB(pipeFdPair1[1], message );
        
        // If problems during write, abort
        if( write_ans!= au::OK )
        {                
	    LM_E(("Error sending message from process to platform (pipeFdPair1[1]:%d) write-error %s",  pipeFdPair1[1], au::status( write_ans) ));

            LM_T(LmtIsolated,("Error sending message from process to platform write-error %s",  au::status( write_ans) ));
            if( isolated_process_as_tread )
                return;
            else
                _exit(101);
        }
        
        // Read the response message
        samson::network::MessagePlatformProcess *response;

        LM_T(LmtIsolated,("Background process: Receive answer from platform on pipeFdPair2[0]:%d", pipeFdPair2[0]));
        // Read the answer from the platform
        au::Status read_ans = au::readGPB(pipeFdPair2[0], &response, -1);

        // If problems during read, die
        if( read_ans != au::OK)
        {
            LM_E(("Background process did not receive an answer from message with code %d (written on pipeFdPair1[1](%d) to the platform, reading from pipeFdPair2[0](%d). Error code '%s'", message->code(), pipeFdPair1[1], pipeFdPair2[0], au::status(read_ans) ));
            LM_T(LmtIsolated,("Error sending message from process to platform read-error '%s'", au::status(read_ans) ));
            if( isolated_process_as_tread )
                return;
            else
                _exit(102);
        }
        
        // If response code is kill, let's die
        if( response->code() == samson::network::MessagePlatformProcess_Code_code_kill )
        {
            LM_T(LmtIsolated,("Kill message received in thread-mode!!", write_ans));
            if( isolated_process_as_tread )
            {
                return;
            }
            else
                _exit(103);
        }

        // Revove response object
        delete response;
         
        LM_T(LmtIsolated,("Background process: Finish a message to process"));
         
    }

	
	// Function used inside runIsolated to send a code to the main process
	void ProcessItemIsolated::sendCode( int c )
	{
        LM_T(LmtIsolated,("Background process: Sending code %d (WORKER_TASK_ITEM_CODE_FLUSH_BUFFER:%d, WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH:%d)",c, WORKER_TASK_ITEM_CODE_FLUSH_BUFFER, WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH));
         
        samson::network::MessageProcessPlatform *message = new samson::network::MessageProcessPlatform();
        message->set_code( samson::network::MessageProcessPlatform_Code_code_operation );
        message->set_operation( c );

        sendMessageProcessPlatform( message );
        
        delete message;
	}

	void ProcessItemIsolated::setUserError( std::string error_message )
	{
        
        LM_E(("Background process: Sending user error %s", error_message.c_str() ));
        
        samson::network::MessageProcessPlatform *message = new samson::network::MessageProcessPlatform();
        message->set_code( samson::network::MessageProcessPlatform_Code_code_user_error );
        message->set_error( error_message );
        
        sendMessageProcessPlatform( message );
        
        delete message;
        
        // Kill my self, since it has been an error reported by the user
        _exit(0);
        
	}
	
	
	// Function used inside runIsolated to send a code to the main process
	void ProcessItemIsolated::trace( LogLineData *logData )
	{
        // This is the old trace mechanism.
        // It was desactivated in samson 0.6.1
        // A new system based on blocks should be provided
        
#if 0
         
        LM_T(LmtIsolated,("Background process: Sending trace %s", logData->text ));
        
        samson::network::MessageProcessPlatform *message = new samson::network::MessageProcessPlatform();
        message->set_code( samson::network::MessageProcessPlatform_Code_code_trace );

        samson::network::Trace *trace = message->mutable_trace( );
        
        trace->set_text( logData->text );
        trace->set_type( logData->type );
        trace->set_file( logData->file );
        trace->set_lineno( logData->lineNo );
        trace->set_fname( logData->fName );
        trace->set_tlev( logData->tLev );
        trace->set_stre( logData->stre );
        
        sendMessageProcessPlatform( message );
        
        delete message;        
#else
        if (logData == NULL) return; // 'strict' ...
#endif
	}	
	
	void ProcessItemIsolated::reportProgress( double p )
	{
        
        LM_T(LmtIsolated,("Background process: Report progress %f", p ));
        
        samson::network::MessageProcessPlatform *message = new samson::network::MessageProcessPlatform();
        message->set_code( samson::network::MessageProcessPlatform_Code_code_progress );        
        message->set_progress(p);
        
        sendMessageProcessPlatform( message );
        
        delete message; 
	}

	void ProcessItemIsolated::reportProgress( double p , std::string status )
	{
        
        LM_T(LmtIsolated,("Background process: Report progress %f", p ));
        
        samson::network::MessageProcessPlatform *message = new samson::network::MessageProcessPlatform();
        message->set_code( samson::network::MessageProcessPlatform_Code_code_progress );        
        message->set_progress(p);
        message->set_progress_status( status );
        
        sendMessageProcessPlatform( message );
        
        delete message; 
	}
    
    
	
	void ProcessItemIsolated::runBackgroundProcessRun()
	{
        // Restart the log system to avoid logs
        //au::restart_log_to_server( au::str("/tmp/samsonWorker_chield_%d" , getpid() ) );
        au::set_log_direct_mode(true); // Direct mode means no-blocking // no reconnection
        
        
        LM_M(("Start background SAMSON isolated process: %s" , processItemIsolated_description.c_str() ));
        
        LM_T(LmtIsolated,("Running runBackgroundProcessRun..."));

		// Close the other side of the pipes ( if it is in thread-mode, we cannot close)
        
        if( !isolated_process_as_tread )
        {
            
            close(pipeFdPair1[0]);
            close(pipeFdPair2[1]);
            
            //Trazas Goyo
            LM_T(LmtFileDescriptors,("Child closing pipe descriptors not used. Child closes pipeFdPair1[0]:%d, pipeFdPair2[1]:%d\n", pipeFdPair1[0], pipeFdPair2[1]));
            LM_T(LmtFileDescriptors,("Child closing pipe descriptors not used. Child uses pipeFdPair1[1]:%d, pipeFdPair2[0]:%d\n", pipeFdPair1[1], pipeFdPair2[0]));

            // Valgrind
            // Warning: Invalid file descriptor 1014 in syscal close()
            // So changing limit from 1024 to 1014

            for ( int i = 3 ;  i < 1014 ; i++ )
                if( ( i != pipeFdPair1[1] ) && ( i != pipeFdPair2[0] ) && ( i != logFd ) && ( i!= au::get_log_fd() ) )
                {
                    
                    //Trazas Goyo
                    LM_T(LmtFileDescriptors, ("Child closing descriptors but pipeFdPair1[1]:%d, pipeFdPair2[0]:%d, logFd:%d. fd:%d\n", pipeFdPair1[1], pipeFdPair2[0], logFd, i));
                    
                    close( i );
                }

        }
        
        LM_T(LmtIsolated,("Child sends the begin message"));
        // Send the "begin" message
        {
            samson::network::MessageProcessPlatform *message = new samson::network::MessageProcessPlatform();
            message->set_code( samson::network::MessageProcessPlatform_Code_code_begin );
            sendMessageProcessPlatform(message);
            delete message;
        }
        LM_T(LmtIsolated,("Child begin message sent"));

        LM_T(LmtIsolated,("Running runIsolated"));

 		runIsolated();
        
        LM_M(("Finish background SAMSON isolated process: %s" , processItemIsolated_description.c_str() ));
        
 		LM_T(LmtIsolated,("Child sends the end message"));
        // Send the "end" message
        {
            samson::network::MessageProcessPlatform *message = new samson::network::MessageProcessPlatform();
            message->set_code( samson::network::MessageProcessPlatform_Code_code_end );
            sendMessageProcessPlatform(message);
            delete message;
        }
        LM_T(LmtIsolated,("Child end message sent"));
		
		// Close the other side of the pipe
        if( !isolated_process_as_tread )
        {
            close(pipeFdPair1[1]);
            close(pipeFdPair2[0]);
            
            //Trazas Goyo
            LM_T(LmtFileDescriptors,  ("Child closing used pipe descriptors because finished. pipeFdPair1[1]:%d, pipeFdPair2[0]:%d\n", pipeFdPair1[1], pipeFdPair2[0]));
        }
        
        LM_T(LmtIsolated, ("Finishing runBackgroundProcessRun..."));
        
        
        // Stop the loging system ( make sure we close connection with log Server )
        au::stop_log_to_server();
        
	}	
	
	

	
	
}
