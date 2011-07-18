#ifndef _H_DelilahMonitorization
#define _H_DelilahMonitorization

/* ****************************************************************************
 *
 * FILE                     DelilahMonitorization.h
 *
 * DESCRIPTION			    Monitorization pannel for delilah
 *
 */

#include <cstdlib>                      // atexit
#include <sstream>                      // std::ostringstream
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <algorithm>

#include <readline/readline.h>
#include <readline/history.h>

#include "logMsg/logMsg.h"				// LM_M

#include "samson/delilah/Delilah.h"		// samson::Delilah

#include "DelilahClient.h"              // ss:DelilahClient

#include "au/CursesConsole.h"			// au::CursesConsole


namespace samson {
	
    typedef enum 
    {
        
        general,
        task,
        queues,
        queues_tasks,
        
    } DelilahMonitorizationType;
       
	/**
	 Main class for the DelilahConsole program
	 */
	
	class DelilahMonitorization : public Delilah , public au::CursesConsole
	{
		   
        DelilahMonitorizationType  type;
        size_t reference;

        std::vector<std::string>  main_commands;
        
	public:
        
		DelilahMonitorization( NetworkInterface *network ) : Delilah( network , true )
		{
			trace_on = true;
            type =  general;
            
            reference = 1024*1024*1024;
            
            
            // Main commands
            main_commands.push_back("show_general");
            main_commands.push_back("show_tasks");
            main_commands.push_back("show_queues");
            main_commands.push_back("show_queues_tasks");
		}
		
		~DelilahMonitorization()
		{
		}
		
        void runInBackground();
        
        // Function to print content
        void printContent();
        
		// Notifications from delilah
		void uploadDataConfirmation( DelilahUploadDataProcess *process ){};
		void downloadDataConfirmation( DelilahDownloadDataProcess *process ){};
		void pushConfirmation( PushComponent *process ){};
		virtual void notifyFinishOperation( size_t id ){};
        
		// Function to process messages from network elements not handled by Delila class
		int _receive(int fromId, Message::MessageCode msgCode, Packet* packet)
        {
            return 0;
        };		
        
		
		// Show a message on screen
		virtual void showMessage( std::string message)
		{
		}
		
		virtual void showTrace( std::string message)
		{
            /*
			if( trace_on )
				writeWarningOnConsole( message );
             */
		}
        
        // Eval commnad
        void evalComamnd( );
        
        void evalRealTimeComamnd( )
        {
            // do something with the command
        }
        

        // Get header information
        std::string getHeaderLeft();
        std::string getHeaderRight();

        // Autocompletion function
        void auto_complete( std::vector<std::string>& previous_words , std::string& current_word ,std::vector<std::string>& command_options );
        
    private:

        void printGeneral();
        void printTask();
        void printQueues();
        void printQueuesTasks();
        
        
	};
    
}


#endif
