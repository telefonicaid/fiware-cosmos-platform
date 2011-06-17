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

    void DelilahMonitorization::run()
    {

        LM_M(("Delilah monitorization"));
        
        atexit(ncurses_atexit);
        
        srand(time(NULL));
        
        initscr();  /* Start curses mode   */
        
        keypad(stdscr, TRUE);		/* We get F1, F2 etc..              */
        //noecho();                   /* Don't echo() while we do getch   */
        //raw();                    /* Line buffering disabled          */

        
        while( true )
        {
            setRowsAndCols();
            
            clear();
            
            // au::Format::string("Version %s " , SAMSON_VERSION ).c_str()
            printLine( );            
            printLine( " SAMSON MONITORIZATION PANEL", au::Format::string("Version %s", SAMSON_VERSION ).c_str()  );
            printLine( );            


            switch (type) {
                case general:
                    printGeneral();
                    break;
                    
                case memory:
                    printMemory();
                    break;
            }
            
            
            //printLine(rows-3);          
            //printLine(rows-2 , "Type h (help) , g (general) , m (memory) , ...","");
            move( rows-1 , 0 );
            printw(" > ");
            
            refresh();/* Print it on to the real screen */
            
            usleep( 10000 );
            
        }
        

    }

    void DelilahMonitorization::printGeneral()
    {
        
        if( !samsonStatus )
        {
            printLine("Waiting for the information message...");
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
            double per_disk = (total_memory==0)?0:((double) disk_pending_operations / (double) 40);

            printLine( au::Format::string("Worker %03d", i) );
            

            printLine( au::Format::string("\tCores  [ %s ] %s / %s : %s" , 
                                      au::Format::percentage_string(per_cores).c_str() , 
                                      au::Format::string(used_cores).c_str() , 
                                      au::Format::string(total_cores).c_str(),      
                                      au::Format::progress_bar( per_cores , cols - 50 ).c_str()
                      ));
            
            printLine( au::Format::string("\tMemory [ %s ] %s / %s : %s" , 
                                      au::Format::percentage_string(per_memory).c_str() , 
                                      au::Format::string(used_memory).c_str() , 
                                      au::Format::string(total_memory).c_str(),     
                                      au::Format::progress_bar( per_memory , cols - 50 ).c_str()
                      ));
                        
            // Disk operations
            
            printLine( au::Format::string("\tDisk                      %s : %s" , 
                                        au::Format::string(disk_pending_operations).c_str() ,
                                        au::Format::progress_bar( per_disk , cols - 50 ).c_str() ));

            printLine("");
            
        }        
        
        
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