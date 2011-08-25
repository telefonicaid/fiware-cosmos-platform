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

#include "engine/MemoryManager.h"			// engine::MemoryManager
#include "engine/Notification.h"            // engine::Notification

#include "samson/common/traces.h"                       // logFd
#include "samson/common/SamsonSetup.h"                // Goyo: ss:SamsonSetup
#include "samson/common/NotificationMessages.h"     // notification_samson_worker_send_trace
#include "samson/common/MemoryTags.h"                 // MemoryInput , MemoryOutputNetwork, ...

#include "samson/network/iomMsgAwait.h"            // iomMsgAwait
#include "samson/network/Packet.h"                     // samson::Packet


#include "samson/isolated/ProcessItemIsolated.h"    // Own interface
#include "samson/isolated/SharedMemoryManager.h"    

#include "SharedMemoryItem.h"           // engine::SharedMemoryItem


namespace samson
{

   
    
    // Static variable to active the "thread" mode of the background process execution
    bool ProcessItemIsolated::isolated_process_as_tread = false;
    	
	
	// Function to run by the backgroudn thread
	void* run_ProcessItemIsolated( void* p )
	{
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
        LM_T( LmtIsolated , ("Isolated process %s: start ******************************************************************************************* ",getStatus().c_str()));
        
        
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
		
		//LM_M( ("Isolated process %s: pipes created. pipeFdPair1[0]:%d, pipeFdPair1[1]:%d, pipeFdPair2[0]:%d, pipeFdPair2[1]:%d\n",getStatus().c_str(), pipeFdPair1[0], pipeFdPair1[1], pipeFdPair2[0], pipeFdPair2[1]));
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
                _exit(1000);
            }
        }
		
        // Enchange all the necessary messages between background and foreground process
        LM_T( LmtIsolated , ("Isolated process %s: runExchangeMessages start ",getStatus().c_str()));
        
        runExchangeMessages();
        
        LM_T( LmtIsolated , ("Isolated process %s: runExchangeMessages finish ",getStatus().c_str()));
		
		// Close the rest of pipes all pipes
        if( isolated_process_as_tread )
        {
            sleep(1);  
            LM_T( LmtIsolated , ("Isolated process %s: Closing unused side of the pipe (thread mode) pipeFdPair1[1]:%d, pipeFdPair2[0]:%d ",getStatus().c_str(), pipeFdPair1[1], pipeFdPair2[0]));
            close(pipeFdPair1[1]);
            close(pipeFdPair2[0]);
        }
		
		LM_T( LmtIsolated , ("Isolated process %s: Closing the rest of fds of the pipe pipeFdPair1[0]:%d, pipeFdPair2[1]:%d ",getStatus().c_str(), pipeFdPair1[0], pipeFdPair2[1]));
		close(pipeFdPair1[0]);
		close(pipeFdPair2[1]);
		
		LM_T( LmtIsolated , ("Isolated process %s: finish ",getStatus().c_str()));
		
		// Kill and wait the process
        if( !isolated_process_as_tread )
        {
            
            // Loop until the background thread is killed
            sub_status = "Waiting";
            int stat_loc;
            pid_t p=0;
            do
            {
                
                // Wait for
                for ( int i = 0 ; i < 100 ; i++)
                {
                    p = waitpid(pid, &stat_loc , WNOHANG );
                    if( p == pid)
                        break;
                    usleep(10000);
                }

                
                // Send a kill message if still not died
                if( p!= pid )
                {
                    LM_T(LmtIsolated,("Killin background process manually"));
                    kill( pid , SIGKILL );
                    
                    // Give the background process some air to die in peace
                    usleep(10000);
                }
                else
                {
                    if ( WIFEXITED(stat_loc) )
                    {
                        int s = WEXITSTATUS( stat_loc );
                        LM_T(LmtIsolated,("Background process existed with code %d",s));
                        
                    }
                    else if( WIFSIGNALED( stat_loc ) )
                    {
                        int s = WTERMSIG( stat_loc );
                        LM_T(LmtIsolated,("Background process existed with code %d",s));
                    }
                    else
                        LM_T(LmtIsolated,("Background process 'crashed' with unknown reason"));
                }
                
            } while (p != pid);
        }
		
        LM_T( LmtIsolated , ("Isolated process %s: Finish ******************************************************************************************* ",getStatus().c_str()));
        
	}
    
    bool ProcessItemIsolated::processProcessPlatformMessage( samson::network::MessageProcessPlatform * message )
    {
        
        // Eval if the process has been canceled by ProcessManager, if so, it should return assap
        
        if( isProcessItemCanceled() )
        {
            error.set( "ProcessItem canceled" );
            
            // Send a kill message and finish
            samson::network::MessagePlatformProcess * response = new samson::network::MessagePlatformProcess();
            response->set_code( samson::network::MessagePlatformProcess_Code_code_kill );
            au::writeGPB(pipeFdPair2[1], response);
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
                
                // Send the continue
                samson::network::MessagePlatformProcess * response = new samson::network::MessagePlatformProcess();
                response->set_code( samson::network::MessagePlatformProcess_Code_code_ok );
                au::writeGPB(pipeFdPair2[1], response);
                delete response;
                
                // Not finish the process
                return false;
            }
                break;
                
            case samson::network::MessageProcessPlatform_Code_code_trace:
            {
                
                /**
                 Send the trace to all delilahs
                 */
                
                Packet * p = new Packet( Message::Trace );
#ifdef __LP64__
                p->message->set_delilah_id( 0xFFFFFFFFFFFFFFFF );
                p->message->mutable_trace()->CopyFrom( message->trace() );
                p->message->set_delilah_id( 0xFFFFFFFFFFFFFFFF );
#else
                p->message->set_delilah_id( 0xFFFFFFFF );
                p->message->mutable_trace()->CopyFrom( message->trace() );
                p->message->set_delilah_id( 0xFFFFFFFF );
#endif
                engine::Engine::shared()->notify( new engine::Notification( notification_samson_worker_send_trace , p ) );
                //LM_M(("Notifying a trace to the engine"));
                
                // Old trace system, tracing here... to be removed
                /*
                 if (lmOk(message->trace().type(), message->trace().tlev() ) == LmsOk)
                 {
                 std::string _text = message->trace().text();
                 std::string file = message->trace().file();
                 std::string fname = message->trace().fname();
                 std::string stre = message->trace().stre();
                 
                 LM_T( LmtIsolated , ("Isolated process %s: Message with trace %s ",getStatus().c_str() , _text.c_str() ));
                 
                 lmOut(
                 (char*)_text.c_str(),   
                 (char) message->trace().type() , 
                 file.c_str(), 
                 message->trace().lineno() , 
                 fname.c_str(), 
                 message->trace().tlev() , 
                 stre.c_str() 
                 );
                 }
                 */
                
                
                // Send the continue
                samson::network::MessagePlatformProcess * response = new samson::network::MessagePlatformProcess();
                response->set_code( samson::network::MessagePlatformProcess_Code_code_ok );
                au::writeGPB(pipeFdPair2[1], response);
                delete response;
                
                // Not finish the process
                return false;
                
            }
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
                au::writeGPB(pipeFdPair2[1], response);
                delete response;                    
                
                // Not finish the process
                return false;

            }
                break;
                
                
            case samson::network::MessageProcessPlatform_Code_code_user_error:
            {
                LM_T( LmtIsolated , ("Isolated process %s: Message reporting user error  ",getStatus().c_str() ));
                
                // Set the error
                if( message->has_error() )
                    error.set( message->error() );
                else
                    error.set( "Undefied user-defined error" );
                
                // Send an ok back, and return
                samson::network::MessagePlatformProcess * response = new samson::network::MessagePlatformProcess();
                response->set_code( samson::network::MessagePlatformProcess_Code_code_ok );
                au::writeGPB(pipeFdPair2[1], response);
                delete response;

                // It has to finish since the background process has notifyied the error
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
                au::writeGPB(pipeFdPair2[1], response);
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
                LM_T( LmtIsolated , ("Isolated process %s: Problems reading the 'begin' message [error code '%s'] ",getStatus().c_str() , au::status(c)));
                error.set( au::str( "Problems starting background process '%s'", au::status(c)) );
                return;     // Problem with this read
            }
            
            if( !message )
                LM_X(1, ("Internal error"));

            if( message->code() != samson::network::MessageProcessPlatform_Code_code_begin )
            {
                delete message;
                return;
            }
            else
                delete message;
        
            // Close the unnecessary pipes
            if( !isolated_process_as_tread )
            {
                LM_T( LmtIsolated , ("Isolated process %s: Closing secondary fds of pipes, pipeFdPair1[1]:%d, pipeFdPair2[0]:%d ",getStatus().c_str(), pipeFdPair1[1], pipeFdPair2[0]));
                close(pipeFdPair1[1]);
                close(pipeFdPair2[0]);
            }
            
            
            // Send the continue
            {
                samson::network::MessagePlatformProcess * response = new samson::network::MessagePlatformProcess();
                response->set_code( samson::network::MessagePlatformProcess_Code_code_ok );
                au::writeGPB(pipeFdPair2[1], response);
                delete response;
            }
                
            
        }
        
        LM_T( LmtIsolated , ("Isolated process %s: begin message received. Starting the continuous loop... ",getStatus().c_str()));
        
        // Continuous read of messages and perform required actions
        
		while( true )
		{
            
            
			// Take the timeout for reading operations from the other site
			int timeout_setup = SamsonSetup::getInt("isolated.timeout");
            
			LM_T(LmtIsolated, ("Isolated process %s: Reading a new message with timeout %d", getStatus().c_str() , timeout_setup));
            
            // Read a message from the process
            samson::network::MessageProcessPlatform * message;
            au::Status c = au::readGPB( pipeFdPair1[0] , &message, timeout_setup );
            
            if( c != au::OK )
            {
                // Not possible to read the message for any reason
				LM_T(LmtIsolated, ("Isolated process %s: Not possible to read a message with error_code' %s'", getStatus().c_str() , au::status(c) ));
                
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
         
        //LM_M(("Background process: Sending a message to process"));
        
        // Write the message
        au::Status write_ans = au::writeGPB(pipeFdPair1[1], message );
        
        // If problems during write, abort
        if( write_ans!= au::OK )
        {                
            LM_T(LmtIsolated,("Error sending message from process to platform write-error %s",  au::status( write_ans) ));
            if( isolated_process_as_tread )
                return;
            else
                _exit(101);
        }
        
        // Read the response message
        samson::network::MessagePlatformProcess *response;

        // Read the answer from the platform
        au::Status read_ans = au::readGPB(pipeFdPair2[0], &response, -1);

        // If problems during read, die
        if( read_ans != au::OK)
        {
            LM_W(("Background process did not receive an answer from message with code %d to the platform. Error code '%s'", 
                  message->code(), au::status(read_ans) ));
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
        LM_T(LmtIsolated,("Background process: Sending code %d",c));
         
        samson::network::MessageProcessPlatform *message = new samson::network::MessageProcessPlatform();
        message->set_code( samson::network::MessageProcessPlatform_Code_code_operation );
        message->set_operation( c );

        sendMessageProcessPlatform( message );
        
        delete message;
	}

	void ProcessItemIsolated::setUserError( std::string error_message )
	{
        
        LM_T(LmtIsolated,("Background process: Sending user error %s", error_message.c_str() ));
        
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
        LM_T(LmtIsolated,("Running runBackgroundProcessRun..."));

		// Close the other side of the pipes ( if it is in thread-mode, we cannot close)
        
        if( !isolated_process_as_tread )
        {
            
            close(pipeFdPair1[0]);
            close(pipeFdPair2[1]);
            
            //Trazas Goyo
            //LM_M(("Child closing pipe descriptors not used. pipeFdPair1[0]:%d, pipeFdPair2[1]:%d\n", pipeFdPair1[0], pipeFdPair2[1]));
            
            for ( int i = 3 ;  i < 1024 ; i++ )
                if( ( i != pipeFdPair1[1] ) && ( i!= pipeFdPair2[0] ) && ( i!= logFd ) ) 
                {
                    
                    //Trazas Goyo
                    //LM_M(("Child closing descriptors but pipeFdPair1[1]:%d, pipeFdPair2[0]:%d, logFd:%d. fd:%d\n", pipeFdPair1[1], pipeFdPair2[0], logFd, i));
                    
                    close( i );
                }
        }
        
        
        // Send the "begin" message
        {
            samson::network::MessageProcessPlatform *message = new samson::network::MessageProcessPlatform();
            message->set_code( samson::network::MessageProcessPlatform_Code_code_begin );
            sendMessageProcessPlatform(message);
            delete message;
        }        

        LM_T(LmtIsolated,("Running runIsolated"));
        
 		runIsolated();
        
        //LM_M(("Finishing runIsolated"));
        
        // Send the "end" message
        {
            samson::network::MessageProcessPlatform *message = new samson::network::MessageProcessPlatform();
            message->set_code( samson::network::MessageProcessPlatform_Code_code_end );
            sendMessageProcessPlatform(message);
            delete message;
        }
		
		// Close the other side of the pipe
        if( !isolated_process_as_tread )
        {
            close(pipeFdPair1[1]);
            close(pipeFdPair2[0]);
            
            //Trazas Goyo
            //LM_M(("Child closing used pipe descriptors because finished. pipeFdPair1[1]:%d, pipeFdPair2[0]:%d\n", pipeFdPair1[1], pipeFdPair2[0]));
        }
        
        //LM_M(("Finishing runBackgroundProcessRun..."));
        
        
	}	
	
	

	
	
}
