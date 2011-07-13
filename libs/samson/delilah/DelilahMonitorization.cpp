/* ****************************************************************************
 *
 * FILE                     DelilahConsole.h
 *
 * DESCRIPTION			   Console terminal for delilah
 *
 */

#include <ncurses.h>
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	
#include <iostream>
#include <iomanip>

#include "au/Format.h"						// au::Format
#include "au/CommandLine.h"				// au::CommandLine

#include "engine/MemoryManager.h"				// samson::MemoryManager

#include "samson/common/EnvironmentOperations.h"						// Environment operations (CopyFrom)
#include "samson/common/Info.h"                             // samson::Info

#include "samson/network/Packet.h"						// ss:Packet

#include "samson/module/samsonVersion.h"		// SAMSON_VERSION
#include "samson/module/samsonVersion.h"

#include "samson/delilah/Delilah.h"					// samson::Delailh

#include "samson/stream/BlockManager.h"     // samson::stream::BlockManager
#include "DelilahUploadDataProcess.h"		// samson::DelilahUpLoadDataProcess
#include "DelilahDownloadDataProcess.h"		// samson::DelilahDownLoadDataProcess

#include "PushComponent.h"  
#include "DelilahMonitorization.h"				// Own interface


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

        WINDOW *w = initscr(); /* Start curses mode   */
        cbreak();
        nodelay(w, TRUE);        
        
        keypad(stdscr, TRUE);		/* We get F1, F2 etc..              */
        noecho();                   /* Don't echo() while we do getch   */
        //raw();                    /* Line buffering disabled          */

        
        while( true )
        {
            setRowsAndCols();
            
            clear();
            
            // au::Format::string("Version %s " , SAMSON_VERSION ).c_str()
            printLine( );            
            

            if ( samsonStatus )
            {
                
                std::ostringstream txt;
                txt << "( Controller Uptime: " << au::Format::time_string( samsonStatus->controller_status().up_time() ) << " )";
                txt << " ( Updated: " << cronometer_samsonStatus.str() <<  " )";
                
                printLine( " SAMSON MONITORIZATION PANEL", txt.str().c_str()  );
                
            }
            else
                printLine( " SAMSON MONITORIZATION PANEL", "Waiting info..."  );
            
            printLine( );            

            switch (type) {
                case general:
                    printGeneral();
                    break;
                    
                case task:
                    printTask();
                    break;
                    
                case queues:
                    printQueues();
                    break;
            }

            
            printLine(rows-2);          
            printLine(rows-1 , "" , au::Format::string("Version %s", SAMSON_VERSION ).c_str());
            move( rows-1 , 0 );
            
            refresh();/* Print it on to the real screen */
            
            usleep( 10000 );

            
            int ch = getch();
            if( ch != ERR )
            {
                // Do something
                if( ch == 'q' )
                    exit(0);
                
                if( ch == 'g' )
                    type = general;
                
                if( ch == 't' )
                    type = task;
                
                if( ch == 's' )
                    type = queues;
        
                if( ch == '+' )
                    reference *= 2;
                
                if( ch == '-' )
                {
                    reference /= 2;
                    if( reference == 0)
                        reference = 1;
                }
                
            }
        }
        

    }

    void DelilahMonitorization::printGeneral()
    {
        
        if( !samsonStatus )
        {
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

            size_t read_rate = worker_status.disk_read_rate();
            size_t write_rate = worker_status.disk_write_rate();
            
            size_t network_read_rate = worker_status.network_read_rate();
            size_t network_write_rate = worker_status.network_write_rate();
            
            
            std::stringstream txt;
            txt << " [ Process: " << au::Format::percentage_string(per_cores).c_str();
            txt << " Memory: " << au::Format::percentage_string(per_memory);
            txt << " Disk: " << disk_pending_operations;
            txt << "  ( uptime: " << au::Format::time_string( worker_status.up_time() ) << " )";
            txt << " ( updated: " << au::Format::time_string( cronometer_samsonStatus.diffTimeInSeconds() + worker_status.update_time() ) << " ) ]";

            printLine("");
            printLine( au::Format::string("Worker %03d %s", i , txt.str().c_str()) );           
            printLine("");
            

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

            printLine( au::Format::string("\t                  Read   %s: %s  " , 
                                          au::Format::string(read_rate,"Bs").c_str() ,
                                          au::Format::progress_bar( (double)read_rate /(double) (200*(1024*1024)) , cols - 50 ).c_str() ));

            printLine( au::Format::string("\t                  Write  %s: %s  " , 
                                          au::Format::string(write_rate,"Bs").c_str() ,
                                          au::Format::progress_bar( (double)write_rate /(double) (200*(1024*1024)) , cols - 50 ).c_str() ));

            
            printLine( au::Format::string("\tNetwork                 " )); 
            
            printLine( au::Format::string("\t                  Read   %s: %s  " , 
                                          au::Format::string(network_read_rate,"Bs").c_str() ,
                                          au::Format::progress_bar( (double)network_read_rate /(double) (200*(1024*1024)) , cols - 50 ).c_str() ));
            
            printLine( au::Format::string("\t                  Write  %s: %s  " , 
                                          au::Format::string(network_write_rate,"Bs").c_str() ,
                                          au::Format::progress_bar( (double)network_write_rate /(double) (200*(1024*1024)) , cols - 50 ).c_str() ));
            
            
        }        
        
        
    }
 
    
    void DelilahMonitorization::printTask()
    {

        if ( !samsonStatus )
            return;
        
        info_lock.lock();
        
        printLine("");
        printLine("Tasks");
        printLine("");

        
        for ( int i = 0 ; i < samsonStatus->controller_status().task_manager_status().task_size() ; i++)
        {
            const network::ControllerTask &task =  samsonStatus->controller_status().task_manager_status().task(i);
            
            std::stringstream txt;
            
            txt << "\t" << au::Format::string( "[ %04lu / Job: %04lu ] " , task.task_id() , task.job_id() );
            
            switch (task.state()) {
                case network::ControllerTask_ControllerTaskState_ControllerTaskInit:
                    txt << "Init        " << task.task_description();
                    break;
                case network::ControllerTask_ControllerTaskState_ControllerTaskCompleted:
                    txt << "Completed   " << task.task_description();
                    break;
                case network::ControllerTask_ControllerTaskState_ControllerTaskFinish:
                    txt << "Finished    " << task.task_description();
                    break;
                case network::ControllerTask_ControllerTaskState_ControllerTaskRunning:
                    txt << "Running     " << task.task_description() ;
                    break;
            }
 
            printLine( txt.str().c_str() );
            
            if( task.has_error() )
            {
                printLine( au::Format::string( "  --> Error: ( %s )" ,  task.error().message().c_str() ) );
            }
           
            if( task.state() == network::ControllerTask_ControllerTaskState_ControllerTaskRunning )
            {
                
                double running_progress;
                if( task.total_info().size() == 0 )
                    running_progress = 0;
                else
                    running_progress  =  (double) task.running_info().size() / (double) task.total_info().size();
                
                double processed_completed;
                if( task.processed_info().size() == 0)
                    processed_completed = 0;
                else
                    processed_completed = (double) task.processed_info().size() / (double) task.total_info().size();
                
                std::stringstream txt;
                txt << "\t\tProgress : ";
                txt << au::Format::string( task.processed_info().size() );
                txt << " / " << au::Format::string( task.running_info().size() );
                txt << "/" << au::Format::string( task.total_info().size() ) << " ";
                txt << au::Format::double_progress_bar(processed_completed, running_progress, '*', '-', ' ' , cols - 55  );
                
                printLine( txt.str().c_str() );
                
            }
            
        }
        
        // Unlock the common information lock    
        info_lock.unlock();
        
    }

    void DelilahMonitorization::printQueues()
    {
        
        info_lock.lock();


        printLine("");
        printLine("Stream Queues");
        printLine();
        printLine("");
        
        ValuesCollection queue_names = info->getValues( "worker.*.queues_manager.queues.*.name"  );
                
        for ( int i = 0 ; i < (int) queue_names.size() ; i++)
        {
            ValuesCollection channel_names = info->getValues( au::Format::string("worker.*.queues_manager.queues.%s.matrix.*", queue_names[i].c_str()));
            
            std::ostringstream num_operations_path;
            num_operations_path << "worker.*.queues_manager.queues." << queue_names[i] << ".num_operations";
            size_t num_operations = info->getValues( num_operations_path.str() ).sumSizeT();
            
            std::stringstream txt;
            txt << au::Format::string("   %-20s : Running operations %s ", queue_names[i].c_str() , au::Format::string( num_operations ).c_str() );
            printLine( txt.str().c_str() );
            
            for (int channel = 0 ; channel < (int)channel_names.size() ; channel++)
            {
                
                std::ostringstream size_path;
                size_path << "worker.*.queues_manager.queues." << queue_names[i] << ".matrix." << channel_names[channel] << ".size";
                size_t size = info->getValues( size_path.str() ).sumSizeT();
                
                std::ostringstream num_kvs_path;
                num_kvs_path << "worker.*.queues_manager.queues." << queue_names[i] << ".matrix." << channel_names[channel] << ".kvs";
                size_t num_kvs = info->getValues( num_kvs_path.str() ).sumSizeT();

                
                
                std::stringstream txt;
                txt << au::Format::string("   \tChannel %s : " ,  channel_names[channel].c_str() );
                txt << au::Format::string( size ) << "( #kvs: " << au::Format::string( num_kvs ) << " ) ";
                txt << au::Format::progress_bar( (double)size / (double) reference , cols - 65  );
                printLine( txt.str().c_str() );
            }
            printLine(" ");
            
        }
        
        
        
        // Unlock the common information lock    
        info_lock.unlock();
        
    }
    
    
}
