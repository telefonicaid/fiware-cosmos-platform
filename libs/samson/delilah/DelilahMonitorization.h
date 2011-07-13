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
        task,
        queues
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

        void printLine( int r , std::string left_line ,std::string rigth_line )
        {
		   if( r >= rows )
			  return;

            move( r , 0 );
            printw("%s",left_line.c_str());
            
            move( r , cols-1 - strlen( rigth_line.c_str() ) );
            printw("%s",rigth_line.c_str());
        }
        
        void clear()
        {
            std::string white_line;
            for (int i = 0 ; i < cols ; i++)
                white_line.append(" ");
            
            for (int i = 0 ; i < rows ; i++)
            {
                move( i , 0 );
                printw(  white_line.c_str() );
            }

                
            current_row = 0;

        }

        void printLine(  std::string left_line ,std::string rigth_line  )
        {
		   if( current_row >= (rows-3) )
			  return;

            printLine( current_row++ , left_line, rigth_line );
        }
        
        void printLine( std::string line )
        {
		   if( current_row >= (rows-3) )
			  return;

            printLine( current_row++ , line, "" );
        }
        
        void printLine()
        {
		   if( current_row >= (rows-3) )
			  return;

            printLine( current_row++ );
        }
        
        
        
    };
    
        
	/**
	 Main class for the DelilahConsole program
	 */
	
	class DelilahMonitorization : public Delilah , public CursesPanel
	{
		   
        DelilahMonitorizationType  type;
        
        size_t reference;
        
	public:
        
		DelilahMonitorization( NetworkInterface *network ) : Delilah( network , true )
		{
			trace_on = true;
            type =  general;
            
            reference = 1024*1024*1024;
		}
		
		~DelilahMonitorization()
		{
		}
		
        void runInBackground();
        
        // Main run command to start the show
        void run();
        
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

        void printGeneral();
        void printTask();
        void printQueues();
        
        
	};
    
}


#endif
