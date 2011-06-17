/* ****************************************************************************
 *
 * FILE                     DelilahConsole.h
 *
 * DESCRIPTION			   Console terminal for delilah
 *
 */

#include <ncurses.h>

#include "samson/delilah/Delilah.h"					// samson::Delailh
#include "samson/network/Packet.h"						// ss:Packet
#include "au/Format.h"						// au::Format
#include "DelilahUploadDataProcess.h"		// samson::DelilahUpLoadDataProcess
#include "DelilahDownloadDataProcess.h"		// samson::DelilahDownLoadDataProcess
#include "engine/MemoryManager.h"				// samson::MemoryManager
#include <iostream>
#include <iomanip>
#include "samson/common/EnvironmentOperations.h"						// Environment operations (CopyFrom)
#include <iomanip>
#include "samson/module/samsonVersion.h"		// SAMSON_VERSION
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	
#include "au/CommandLine.h"				// au::CommandLine
#include "samson/stream/BlockManager.h"     // samson::stream::BlockManager
#include "PushComponent.h"  

#include "DelilahMonitorization.h"				// Own interface

#include "samson/module/samsonVersion.h"

namespace samson
{	
    
    void ncurses_atexit(void) 
    {                     
        endwin();
    }  
    
    void* runDelilahMonitorizationInBackground(void*p)
    {
        DelilahMonitorization*dm = (DelilahMonitorization*) p;
        dm->run();
        
        return NULL;
    }
    
    void* runDelilahMonitorizationGetCommandsInBackground(void*p)
    {
        DelilahMonitorization*dm = (DelilahMonitorization*) p;
        dm->getCommands();
        
        return NULL;
    }

    void DelilahMonitorization::runInBackground()
    {
        atexit(ncurses_atexit);
        
        LM_M(("Delilah monitorization run in background"));
        
        srand(time(NULL));
        
        
        initscr();  /* Start curses mode   */
        
        keypad(stdscr, TRUE);		/* We get F1, F2 etc..              */
        noecho();                   /* Don't echo() while we do getch   */
        raw();                    /* Line buffering disabled          */
        
        
        pthread_t t;
        pthread_create(&t, NULL, runDelilahMonitorizationInBackground, this);
        
        pthread_t t2;
        pthread_create(&t2, NULL, runDelilahMonitorizationGetCommandsInBackground, this);
        
    }

    void DelilahMonitorization::getCommands()
    {
        
        while( true )
        {
            ch = getch();
            

            switch (ch) {
                case 'q': exit(0);       break;
                case 'm': type=memory;   break;
                case 'g': type=general;   break;
            }
        }
    }
	
    
    void DelilahMonitorization::run()
    {

        LM_M(("Delilah monitorization"));
        
                
        while( true )
        {
            setRowsAndCols();
            
            clear();
            
            // au::Format::string("Version %s " , SAMSON_VERSION ).c_str()
            printLine( 0 );            
            printLine( 1 , " SAMSON MONITORIZATION PANEL", au::Format::string("Version %s", SAMSON_VERSION ).c_str()  );
            printLine( 2 );            


            switch (type) {
                case general:
                    printGeneral();
                    break;
                    
                case memory:
                    printMemory();
                    break;
            }
            
            
            printLine(rows-3);          
            printLine(rows-2 , "Type h (help) , g (general) , m (memory) , ...","");
            move( rows-1 , 0 );
            printw(" > ");
            
            refresh();/* Print it on to the real screen */
            
            usleep( 10000 );
            
        }
        

    }

    void DelilahMonitorization::printGeneral()
    {
        clear();
        printLine("General...");
        
    }
 
    
    void DelilahMonitorization::printMemory()
    {
        clear();
        printLine("");
        printLine("Memory information");
        printLine();
        printLine("");
        
        
        info_lock.lock();
        
        if ( !samsonStatus )
        {
            // If samsonStatus is not received, show a message informing about this
            printLine("\tWorker status still not received from SAMSON platform");
            return;
        }
        
        for (int i = 0 ; i < samsonStatus->worker_status_size() ; i++)
        {
            const network::WorkerStatus worker_status = samsonStatus->worker_status(i);
            
            int used_cores = worker_status.used_cores();
            int total_cores = worker_status.total_cores();
            double per_cores = (total_cores==0)?0:((double) used_cores / (double) total_cores);
            size_t used_memory = worker_status.used_memory();
            size_t total_memory = worker_status.total_memory();
            double per_memory = (total_memory==0)?0:((double) used_memory / (double) total_memory);
            int disk_pending_operations = worker_status.disk_pending_operations();
            //double per_disk = (total_memory==0)?0:((double) disk_pending_operations / (double) 40);
            
            std::ostringstream txt;
            
            txt << "------------------------------------------------------------------------------------------------" << std::endl;
            txt << "Worker " << i;
            txt << "  Process: " << au::Format::percentage_string(per_cores).c_str();
            txt << " Memory: " << au::Format::percentage_string(per_memory);
            txt << " Disk: " << disk_pending_operations;
            txt << "  ( uptime: " << au::Format::time_string( worker_status.up_time() ) << " )";
            txt << " ( updated: " << au::Format::time_string( cronometer_samsonStatus.diffTimeInSeconds() + worker_status.update_time() ) << " )" << std::endl;
            txt << "------------------------------------------------------------------------------------------------" << std::endl;
            
            
            txt << "\tMemory Manager: " << worker_status.memory_status() << "\n";
            txt << "\tShared Memory Manager:    " << worker_status.shared_memory_status() << "\n";
            
            printLine( txt.str().c_str() );
            
        }
        
        
        // Unlock the common information lock    
        info_lock.unlock();
    }
    
}