
/* ****************************************************************************
 *
 * FILE            DelilahUtils.cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
 *
 * DATE            8/31/11
 *
 * DESCRIPTION
 *
 *  Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#include <sstream>                  // std::ostringstream
#include <iomanip>                  // std::setw

#include "au/Descriptors.h"         // au::Descriptors

#include "DelilahUtils.h"			// Own interface


namespace samson {
    
    std::string getStreamOperationInfo( const pugi::xml_node& node )
    {
        // Get information for this state    
        std::string name = pugi::get( node , "name" );
        std::string description = pugi::get( node , "description" );
        
        std::ostringstream output;
        output << std::setw(15) << name << "  " << description;
        return output.str();
        
    }
    
    std::string getBLockListInfo( pugi::node node )
    {
        size_t size_total =  pugi::getUInt64( node , "size_total" );
        size_t size_on_memory =  pugi::getUInt64( node , "size_on_memory" );
        size_t size_on_disk =  pugi::getUInt64( node , "size_on_disk" );
        
        int num_blocks = pugi::getInt( node , "num_blocks" );
        size_t kvs = pugi::getUInt64( node , "kvs" );
        size_t size = pugi::getUInt64( node , "size" );
        
        if( ( kvs == 0 ) && (size == 0) )
            return "Empty";
        
        std::ostringstream output;
        output << num_blocks << " blocks with ";
        output << au::str( kvs , "kvs" ) << " in ";
        output << au::str( size , "bytes" )  ;
        output << " ";
        if( size_total > 0 )
        {
            double p_memory = (double) size_on_memory / (double) size_total;
            double p_disk = (double) size_on_disk / (double) size_total;
            
            output << au::Format::percentage_string( p_memory ) << " on memory & " << au::Format::percentage_string( p_disk ) << " on disk";
        }
        
        return output.str();
    }
    
    
    std::string getQueueInfo( const pugi::xml_node& queue )
    {
        
        // Get information for this state    
        std::string name = pugi::get( queue , "name" );
        
        const pugi::node block_list = queue.first_element_by_path("block_list");
        
        std::ostringstream output;
        output << std::setw(15) << name << ": [ "  << getBLockListInfo (block_list ) << " ]";
        
        return output.str();
    }
    
    std::string getStateInfo( const pugi::xml_node& state )
    {
        std::ostringstream output;
        
        // Get information for this state    
        std::string name = pugi::get( state, "name" );
        
        int num_state_items =  pugi::getInt( state , "num_state_items" );
        
        const pugi::node state_info = state.first_element_by_path("state_info").first_element_by_path("block_list");
        const pugi::node input_info = state.first_element_by_path("input_info").first_element_by_path("block_list");
        
        output << "State " << name << ": ( " << num_state_items << " state items ) ";
        output << " State: [ " << getBLockListInfo( state_info ) << " ] ";
        
        
        output << " Input: [ " << getBLockListInfo( input_info ) << " ] ";
        
        output << "\n";
        
        return output.str();
        
    }
    
    std::string getQueueTaskInfo( const pugi::xml_node& queue_task )
    {
        // Get information for this state    
        std::string id              = pugi::get( queue_task , "id" );
        std::string description     = pugi::get( queue_task , "description" );
        
        const pugi::node block_list = queue_task.first_element_by_path("block_list");
        
        std::ostringstream output;
        output << "Task " << id << " : " << description << "\n";
        output << "            -> Input " << getBLockListInfo( block_list ) << "\n";
        return output.str();
        
    }
    
    std::string getFormatInfo( const pugi::xml_node& node )
    {
        // Get information for this state    
        std::string key_format = pugi::get( node , "key_format" );
        std::string value_format = pugi::get( node , "value_format" );
        
        std::ostringstream output;
        output << "[" << key_format << "-" << value_format << "]";
        return output.str();
    }
    
    std::string getDataInfo( const pugi::xml_node& node )
    {
        std::ostringstream output;
        
        // Get information for this state    
        std::string name = pugi::get( node , "name" );
        std::string help = pugi::get( node , "help" );
        
        output << "** " << std::left << std::setw(40) << name << " - " << help;
        
        return output.str();
    }
    
    
    std::string getOperationInfo( const pugi::xml_node& node )
    {
        std::ostringstream output;
        
        // Get information for this state    
        std::string name = pugi::get( node , "name" );
        std::string type = pugi::get( node , "type" );
        std::string help = pugi::get( node , "help" );
        
        output << "** " << name << " ( " << type << " )\n";
        
        output << "\t\tInputs: ";
        pugi::xml_node input_formats = node.first_element_by_path("input_formats");
        for( pugi::xml_node_iterator n = input_formats.begin() ; n != input_formats.end() ; n++)
            output << getFormatInfo(*n) << " ";
        output << "\n";
        
        output << "\t\tOutputs: ";
        pugi::xml_node output_formats = node.first_element_by_path("output_formats");
        for( pugi::xml_node_iterator n = output_formats.begin() ; n != output_formats.end() ; n++)
            output << getFormatInfo(*n) << " ";
        output << "\n";
        output << "\t\tHelp: " << help << "\n";
        
        return output.str();
        
    }
    
    std::string getTaskInfo( const pugi::xml_node& node )
    {
        std::string id = pugi::get( node , "id" );
        std::string job_id = pugi::get( node , "job_id" );
        std::string name = pugi::get( node , "name" );
        std::string state = pugi::get( node , "state" );
        
        std::ostringstream output;
        output << "[ " << id << " ] [ Job " << job_id << " ] [ " << state << " ] " << name << " " ;
        
        if( state == "running" )
        {
            size_t total_info = pugi::getUInt64( node , "total_info" );
            size_t running_info = pugi::getUInt64( node , "running_info" );
            size_t processed_info = pugi::getUInt64( node , "processed_info" );
            
            output << "[ Progress: ";
            output << au::str( running_info , "bytes" ) << " / ";
            output << au::str( processed_info , "bytes" ) << " / ";
            output << au::str( total_info , "bytes" );
            output << "]";
        }
        
        return output.str();
        
    }
    
    std::string getWorkerTaskInfo( const pugi::xml_node& node )
    {
        std::string task_id = pugi::get( node , "task_id" );
        std::string operation = pugi::get( node , "operation" );
        std::string status = pugi::get( node , "status" );
        
        int num_workers = (int) pugi::getUInt64( node , "num_workers" );
        int num_finished_workers = (int) pugi::getUInt64( node , "num_finished_workers" );
        
        std::ostringstream output;
        output << "[ " << task_id << " ] [ " << status << " ] " << operation << " " ;
        
        if( num_finished_workers == num_workers )
            output << " All workers finised ";
        else
            output << " Completed workers " << num_finished_workers << " / " << num_workers;
        
        
        au::Descriptors descriptors;
        pugi::xml_node node_worker_subtasks = node.first_element_by_path("worker_subtasks");        
        for( pugi::xml_node_iterator n = node_worker_subtasks.begin() ; n != node_worker_subtasks.end() ; n++)
        {
            std::string description = pugi::get(*n, "description" );
            std::string state = pugi::get(*n, "state" );
            descriptors.add( au::str( "[ %s : %s ]" , description.c_str() , state.c_str() ) );
        }
        
        output << "\n\t SubTasks: " << descriptors.str();
        
        
        
        
        return output.str();
        
    }    
    
    
    std::string getJobInfo( const pugi::xml_node& node )
    {
        std::ostringstream output;
        
        std::string id = pugi::get( node , "id" );
        std::string command = pugi::get( node , "command" );
        std::string status = pugi::get( node , "status" );
        
        output << "  [ " << id << " ] [ " << std::setw(10) << std::left <<  status << " ] " << command;
        
        pugi::xml_node current_task = node.first_element_by_path("current_task").first_element_by_path("controller_task");
        
        if( pugi::get( current_task , "id" ) != "" )
            output << "\n\tCurrent task: " << getTaskInfo( current_task );
        
        return output.str();
    }
    
    std::string getModuleInfo( const pugi::xml_node& node )
    {
        std::ostringstream output;
        
        std::string name = pugi::get( node , "name" );
        std::string version = pugi::get( node , "version" );
        std::string author = pugi::get( node , "author" );
        
        
        int num_operations  = getNumChildrens( node.first_element_by_path("operations") , "operation" );
        int num_datas       = getNumChildrens( node.first_element_by_path("datas") , "dats" );
        
        output << "  Module " << std::left << std::setw(25) << name << " " << std::setw(10) << version;
        output << std::setw(15) << au::str("[ #ops: %3d #datas: %3d ]",num_operations, num_datas);
        output << " ( " << author << ")";
        
        return output.str();
    }
    
    std::string getNetworkInfo( const pugi::xml_node& node )
    {
        std::ostringstream output;
        
        std::string description = pugi::get( node , "description" );
        output << description << "\n";
        return output.str();
    }
    
    std::string getEngineSystemInfo( const pugi::xml_node& node )
    {
        
        pugi::xml_node node_memory_manager = node.first_element_by_path("memory_manager");
        pugi::xml_node node_disk_manager = node.first_element_by_path("disk_manager");
        pugi::xml_node node_process_manager = node.first_element_by_path("process_manager");
        
        std::ostringstream output;
        
        size_t memory       = pugi::getUInt64( node_memory_manager , "memory" );
        size_t used_memory  = pugi::getUInt64( node_memory_manager , "used_memory" );
        int num_buffers  = (int) pugi::getUInt64( node_memory_manager , "num_buffers" );
        
        output << "** Memory: " << au::str( used_memory , "bytes" ) << " / " << au::str( memory , "bytes" ) << " ( " << num_buffers << " buffers )\n";
        
        
        size_t num_pending_operations = pugi::getUInt64( node_disk_manager , "num_pending_operations" );
        size_t num_running_operations = pugi::getUInt64( node_disk_manager , "num_running_operations" );
        
        std::string t_statistics = pugi::get( node_disk_manager.first_element_by_path("statistics").first_element_by_path("total") , "description" );
        std::string r_statistics = pugi::get( node_disk_manager.first_element_by_path("statistics").first_element_by_path("read") , "description" );
        std::string w_statistics = pugi::get( node_disk_manager.first_element_by_path("statistics").first_element_by_path("write") , "description" );
        
        output << "\n";
        
        output << "** Disk: Running " << num_running_operations << " ops, waiting " << num_pending_operations << " ops\n";
        output << "      READ  [ " << r_statistics << " ]\n"; 
        output << "      WRITE [ " << w_statistics << " ]\n"; 
        output << "      TOTAL [ " << t_statistics << " ]\n"; 
        
        
        pugi::xml_node running = node_process_manager.first_element_by_path("running");
        pugi::xml_node queued = node_process_manager.first_element_by_path("queued");
        pugi::xml_node halted = node_process_manager.first_element_by_path("halted");
        
        au::Descriptors queued_elements;
        pugi::xml_node_iterator n;
        for( n = queued.begin() ; n != queued.end() ; n++)
            queued_elements.add( pugi::get(*n , "operation_name" ) );
        
        au::Descriptors halted_elements;
        for( n = halted.begin() ; n != halted.end() ; n++)
            halted_elements.add( pugi::get(*n , "operation_name" ) );
        
        output << "\n";
        
        output << "** Process manager:\n";
        output << "      QUEUED:  " << queued_elements.str() << "\n";
        output << "      HALTED:  " << halted_elements.str() << "\n";
        output << "      RUNNING:\n";
        
        for(  n = running.begin() ; n != running.end() ; n++)
        {
            output << "         ";
            output << "[ Priority " << pugi::get( *n , "priority") << " ] ";
            output << "[ Progress " << au::Format::percentage_string( pugi::getDouble( *n , "progress") ) << " ] ";
            output << pugi::get( *n , "operation_name");
            output << "\n";
            
        }
        
        
        return output.str();
    }    
    
    std::string getSetInfo( const pugi::xml_node& queue )
    {
        std::ostringstream output;
        
        // Get information for this state    
        std::string name = pugi::get( queue , "name" );
        
        size_t kvs = pugi::getUInt64( queue , "kvs" );
        size_t size = pugi::getUInt64( queue , "size" );
        
        size_t num_files = pugi::getUInt64( queue , "num_files" );
        
        pugi::xml_node format_node = queue.first_element_by_path("format");
        
        output << std::setw(30) << name;
        output << " ";
        output << au::str( kvs );
        output << " kvs in ";
        output << au::str( size ) << " bytes";
        output << " #File: " << num_files;
        output << " " << getFormatInfo( format_node );
        
        return output.str();
    }

}

