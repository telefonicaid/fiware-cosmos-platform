#ifndef _H_DelilahMonitorization
#define _H_DelilahMonitorization

/* ****************************************************************************
 *
 * FILE                     DelilahMonitorization.h
 *
 * DESCRIPTION			    Monitorization pannel for delilah
 *
 */

#include <ncurses.h>

#include <cstdlib>				// atexit

#include "logMsg/logMsg.h"				

#include "au/Console.h"			// au::Console
#include "DelilahClient.h"      // ss:DelilahClient
#include "samson/delilah/Delilah.h"			// samson::Delilah
#include <sstream>                  // std::ostringstream
#include <time.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <algorithm>

namespace samson {
	
	
    typedef enum 
    {
        general,
        memory
    } DelilahMonitorizationType;

    
    class CursesPanel
    {
        int current_row;
        
    public:
        
        int rows,cols;
        
        void setRowsAndCols()
        {
            getmaxyx(stdscr,rows,cols);
        }
        
        void printLine( int r )
        {
            move(r,0);
            for ( int i = 0 ; i < cols-1 ; i++)
                printw("-");
        }

        void printLine( int r , const char* left_line ,const char* rigth_line )
        {
            move( r , 0 );
            printw("%s",left_line);
            
            move( r , cols-1 - strlen( rigth_line ) );
            printw("%s",rigth_line);
        }
        
        void clear()
        {
            current_row = 3;
        }
        
        void printLine( const char* line )
        {
            printLine( current_row++ , line, "" );
        }
        
        void printLine()
        {
            printLine( current_row++ );
        }
        
        
        
    };
    
        
	/**
	 Main class for the DelilahConsole program
	 */
	
	class DelilahMonitorization : public Delilah , public CursesPanel
	{
		   
        DelilahMonitorizationType  type;
        int ch;
        
	public:
		
        
		DelilahMonitorization( NetworkInterface *network ) : Delilah( network , true )
		{
			trace_on = true;
            type =  general;
		}
		
		~DelilahMonitorization()
		{
		}
		
        void runInBackground();
        
        // Main run command to start the show
        void run();

		// Get chars thread
        void getCommands();
        
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
        
    private:

        void printMemory();
        void printGeneral();
        
        
	};
    
}


#endif