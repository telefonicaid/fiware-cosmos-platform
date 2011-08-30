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
#include "au/Info.h"                    // au::Info


#include "pugi/pugi.h"                      // pugi::Pugi

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
    
    std::string DelilahMonitorization::getHeaderLeft()
    {
        switch (type) {
            case general:
                return "General information";
                break;
                
            case task:
                return "Tasks";
                break;
                
            case queues:
                return "Stream queues information";
                break;
                
            case queues_tasks:
                return "Stream queue tasks";
                break;
        }
        
        return "Unknown information";
        
    }
    
    std::string DelilahMonitorization::getHeaderRight()
    {
        std::ostringstream txt;
        txt << " ( Updated: " << getUpdateSeconds() <<  " )";
        return txt.str();
    }
    
    
    void DelilahMonitorization::evalComamnd( )
    {
        if( command == "show_tasks" )
        {
            type = task;
            return;
        }
        
        if( command == "show_general" )
        {
            type = general;
            return;
        }
        
        if( command == "show_queues" )
        {
            type = queues;
            return;
        }
        
        if( command == "show_queues_tasks" )
        {
            type = queues_tasks;
            return;
        }

        if( command == "quit" )
        {
            quitConsole = true;
            return;
        }
        
        
    }
    
    
    void DelilahMonitorization::printContent()
    {

        print("");
        
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
                
            case queues_tasks:
                printQueuesTasks();
                break;
                
        }

    }
    
    
    void DelilahMonitorization::printGeneral()
    {
        /*
        if( !samsonStatus )
            return;
        
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

            print("");
            print( au::str("Worker %03d %s", i , txt.str().c_str()) );           
            print("");
            

            print( au::str("\tCores  [ %s ] %s / %s : %s" , 
                                      au::Format::percentage_string(per_cores).c_str() , 
                                      au::str(used_cores).c_str() , 
                                      au::str(total_cores).c_str(),      
                                      au::Format::progress_bar( per_cores , getCols() - 50 ).c_str()
                      ));
            
            print( au::str("\tMemory [ %s ] %s / %s : %s" , 
                                      au::Format::percentage_string(per_memory).c_str() , 
                                      au::str(used_memory).c_str() , 
                                      au::str(total_memory).c_str(),     
                                      au::Format::progress_bar( per_memory , getCols() - 50 ).c_str()
                      ));
                        
            // Disk operations
            
            print( au::str("\tDisk                      %s : %s" , 
                                        au::str(disk_pending_operations).c_str() ,
                                        au::Format::progress_bar( per_disk , getCols() - 50 ).c_str() ));

            print( au::str("\t                  Read   %s: %s  " , 
                                          au::str(read_rate,"Bs").c_str() ,
                                          au::Format::progress_bar( (double)read_rate /(double) (200*(1024*1024)) , getCols() - 50 ).c_str() ));

            print( au::str("\t                  Write  %s: %s  " , 
                                          au::str(write_rate,"Bs").c_str() ,
                                          au::Format::progress_bar( (double)write_rate /(double) (200*(1024*1024)) , getCols() - 50 ).c_str() ));

            
            print( au::str("\tNetwork                 " )); 
            
            print( au::str("\t                  Read   %s: %s  " , 
                                          au::str(network_read_rate,"Bs").c_str() ,
                                          au::Format::progress_bar( (double)network_read_rate /(double) (200*(1024*1024)) , getCols() - 50 ).c_str() ));
            
            print( au::str("\t                  Write  %s: %s  " , 
                                          au::str(network_write_rate,"Bs").c_str() ,
                                          au::Format::progress_bar( (double)network_write_rate /(double) (200*(1024*1024)) , getCols() - 50 ).c_str() ));
            
            
        }        
*/        
    }
 
    
    void DelilahMonitorization::printTask()
    {

        /*
        if ( !samsonStatus )
            return;
        
        au::TokenTaker tt( &info_lock );
        
        print("");
        print("Tasks");
        print("");

        
        for ( int i = 0 ; i < samsonStatus->controller_status().task_manager_status().task_size() ; i++)
        {
            const network::ControllerTask &task =  samsonStatus->controller_status().task_manager_status().task(i);
            
            std::stringstream txt;
            
            txt << "\t" << au::str( "[ %04lu / Job: %04lu ] " , task.task_id() , task.job_id() );
            
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
 
            print( txt.str().c_str() );
            
            if( task.has_error() )
            {
                print( au::str( "  --> Error: ( %s )" ,  task.error().message().c_str() ) );
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
                txt << au::str( task.processed_info().size() );
                txt << " / " << au::str( task.running_info().size() );
                txt << "/" << au::str( task.total_info().size() ) << " ";
                txt << au::Format::double_progress_bar(processed_completed, running_progress, '*', '-', ' ' , getCols() - 55  );
                
                print( txt.str().c_str() );
                
            }
            
        }
*/
    }

    void DelilahMonitorization::printQueues()
    {
        
        au::TokenTaker tt( &info_lock );

        
        std::istringstream is_xml_document( xml_info );
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load( is_xml_document );
        
        print("");
        std::ostringstream header;
        header << "Stream Queues          ";
        header << "( Reference " << au::str( reference ) << " )";
        print( header.str() );
        print("");
        print("");
        
        pugi::ValuesCollection queues = pugi::values(doc, "//queue/name" ).uniq();
        
        for ( int i = 0 ; i < (int) queues.size() ; i++)
        {
            std::string queue_name = queues[i];
                        
            std::stringstream txt;
            txt << au::str("   %s:", queues[i].c_str() );
            print( txt.str().c_str() );
            
            
            pugi::ValuesCollection channels = pugi::values( doc, "//queue[@name='" + queues[i] + "']/channel/attribute::name").uniq();
            
            for (int c = 0 ; c < (int)channels.size() ; c++)
            {
                
                size_t size = pugi::UInt64(doc, "sum(//queue[@name='" + queues[i] + "']/channel[@name='"+channels[c]+"']/block_list/size)" );
                size_t kvs  = pugi::UInt64(doc, "sum(//queue[@name='" + queues[i] + "']/channel[@name='"+channels[c]+"']/block_list/kvs)" );
                
                size_t s_total  = pugi::UInt64(doc, "sum(//queue[@name='"+queues[i] + "']/channel[@name='"+channels[c]+"']/block_list/size_total)" );
                size_t s_memory = pugi::UInt64(doc, "sum(//queue[@name='"+queues[i] + "']/channel[@name='"+channels[c]+"']/block_list/size_on_memory)" );
                size_t s_disk   = pugi::UInt64(doc, "sum(//queue[@name='"+queues[i] + "']/channel[@name='"+channels[c]+"']/block_list/size_on_disk)" );
                
                {
                    std::stringstream txt;
                    txt << au::str("   \tChannel %s : " ,  channels[c].c_str() );
                    txt << au::str( size ) << "( #kvs: " << au::str( kvs ) << " ) ";
                    print( txt.str().c_str() );
                }
                
                // Size total
                {
                    std::stringstream txt;
                    txt << au::str("   \t\tSize      %s : ", au::str(s_total).c_str() );
                    txt << au::Format::progress_bar( (double)s_total / (double) reference , getCols() - 65  );
                    print( txt.str().c_str() );
                }
                
                // Size on memory 
                {
                    std::stringstream txt;
                    txt << au::str("   \t\tOn memory %s : ", au::str(s_memory).c_str() );
                    txt << au::Format::progress_bar( (double)s_memory / (double) reference , getCols() - 65  );
                    print( txt.str().c_str() );
                }
                
                // Size on disk
                {
                    std::stringstream txt;
                    txt << au::str("   \t\tOn disk   %s : ", au::str(s_disk).c_str() );
                    txt << au::Format::progress_bar( (double)s_disk / (double) reference , getCols() - 65  );
                    print( txt.str().c_str() );
                }
            }
            
            print(" ");
            
        }

    }

    void DelilahMonitorization::printQueuesTasks()
    {

        au::TokenTaker tt( &info_lock );

        std::istringstream is_xml_document( xml_info );
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load( is_xml_document );
        
        print("");
        std::ostringstream header;
        header << "Stream Queue Tasks          ";
        header << "( Reference " << au::str( reference ) << " )";
        print( header.str() );
        print("");
        print("");

        
        pugi::ValuesCollection workers = pugi::values(doc, "//worker/attribute::id" ).uniq();
        
        for ( int i = 0 ; i < (int) workers.size() ; i++)
        {
            std::string q_worker = "//worker[@id='" + workers[i] + "']";

            // Get the collection of tasks
            pugi::ValuesCollection tasks = pugi::values(doc, q_worker + "//queues_task_manager/queue_task/attribute::id" ).uniq();
           
            print(au::str("Worker %s:     %lu queue-tasks " , workers[i].c_str() , tasks.size() ) );
            
            for ( size_t t = 0 ; t < tasks.size() ; t++ )
            {
                std::string q_task = q_worker + "//queues_task_manager/queue_task[id='" + tasks[t] + "']";

                std::string description     = pugi::String( doc , q_task + "/description" );
                
                size_t size_total           = pugi::UInt64(doc, "sum(" + q_task + "//size_total)" );
                size_t size_on_memory       = pugi::UInt64(doc, "sum(" + q_task + "//size_on_memory)" );
                size_t size_on_disk         = pugi::UInt64(doc, "sum(" + q_task + "//size_on_disk)" );

                double progress = pugi::Double( doc , q_task + "/progress" );
                //double progress = 0.5;
                
                std::string line = au::str("     Task %5s %s :" , tasks[t].c_str(), description.c_str() );
                line += "[ Size " +  au::str(size_total);
                line += " Memory: " + au::Format::percentage_string(size_on_memory, size_total);
                line += " Disk: "   + au::Format::percentage_string(size_on_disk, size_total);
                line += " ] ";
                
                line += au::Format::progress_bar( progress , getCols() - line.length() - 8 );
                
                print( line );
            }
            
        }
        
    }    
    
    void DelilahMonitorization::auto_complete
        ( std::vector<std::string>& previous_words , std::string& current_word ,std::vector<std::string>& command_options )
    {
        // If no previous command, just main commands
        if( previous_words.size() == 0)
        {
            command_options.insert( command_options.end() , main_commands.begin() ,  main_commands.end() );
            return;
        }
    }

    
    
}
