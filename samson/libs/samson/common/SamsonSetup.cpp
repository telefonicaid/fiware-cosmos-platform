#include <iomanip>                     // std::setw
#include <iostream>                    // std::cerr
#include <stdio.h>

#include <errno.h>

#include "logMsg/logMsg.h"             // LM_X

#include "au/CommandLine.h"            // au::CommandLine
#include "au/ErrorManager.h"           // au::ErrorManager
#include "au/string/StringUtilities.h"                 // au::Format

#include "au/containers/StringVector.h"
#include "au/tables/Table.h"

#include "status.h"                    // Status codes

#include "samsonVars.h"                // HOME AND WORKING DIRECTORIES

#include "samson/common/SamsonSetup.h"  // Own interface
#include "samson/common/samsonDirectories.h"    // SAMSON_SETUP_FILE

namespace samson {
  
  SAMSON_EXTERNAL_VARS;
  
  SamsonSetup::SamsonSetup() {
    
    // General Platform parameters
    AddUInt64Item("general.memory", "10000000000", "Global available memory ");
    AddUInt64Item("general.num_processess", "16", "Number of cores" );
    AddUInt64Item("general.shared_memory_size_per_buffer", "268435456", "Size of the shared memory segments");
    AddUInt64Item("general.update_status_period", "2", "Period for the automatic update from workers to all delilah");
    
    // Isolation Process
    AddUInt64Item("isolated.timeout", "300", "Timeout for all 3rd partty operations");
    
    // Worker
    AddUInt64Item("worker.period_check_finish_tasks"
                  , "5"
                  ,"Period to review finished tasks in samsonWorker, to be clean from memory");

    AddUInt64Item("worker.period_to_freeze_data_model"
                  , "10"
                  , "Period to freeze data model. Every X seconds, workers exchange all necessary blocks");
    
    // Upload & Download operations
    AddUInt64Item("load.buffer_size", "67108864", "Size of the data block for load operations");
    
    // Stream processing
    AddUInt64Item("stream.max_scheduled_write_size", "120000000", "Maximum size scheduled for writing in the BlockManager");
    AddUInt64Item("stream.max_scheduled_read_size", "120000000", "Maximum size scheduled for writing in the BlockManager");
    
    AddUInt64Item("stream.max_operation_input_size"
                  , "400000000"
                  ,"Maximum input data ( in bytes ) to run an automatic stream processing task");
  }
  
  void SamsonSetup::SetWorkerDirectories(const std::string& samson_home, const std::string& samson_working) {
    char *envsamson_working_ = getenv("SAMSON_WORKING");
    char *env_samson_home = getenv("SAMSON_HOME");
    
    if (samson_working == "") {
      if (envsamson_working_) {
        samson_working_ = envsamson_working_;
      } else {
        samson_working_ = SAMSON_WORKING_DEFAULT;
      }
    }
    else
      samson_working_ = samson_working;
    
    if (samson_home_ == "") {
      if (env_samson_home) {
        samson_home_ = env_samson_home;
      } else {
        samson_home_ =  SAMSON_HOME_DEFAULT;
      }
    }
    else
      samson_home_ = samson_home;
    
    // Basic directories
    samson_home_ = au::GetCannonicalPath( samson_home_ );
    samson_working_ = au::GetCannonicalPath( samson_working_ );
    
    // load setup file
    Load( setup_filename());
    
    if (au::CreateFullDirectory(samson_working_) != au::OK) {
      LM_X(1, ("Error creating directory %s", samson_working_.c_str()));
    }
    if (au::CreateFullDirectory(samson_working_ + "/log") != au::OK) {
      LM_X(1, ("Error creating directory at %s", samson_working_.c_str()));
    }
    if (au::CreateFullDirectory(samson_working_ + "/blocks") != au::OK) {
      LM_X(1, ("Error creating directory at %s", samson_working_.c_str()));
    }
    if (au::CreateFullDirectory(samson_working_ + "/etc") != au::OK) {
      LM_X(1, ("Error creating directory at %s", samson_working_.c_str()));    // Create modules directory
    }
    if (au::CreateFullDirectory(samson_home_ + "/modules")) {
      LM_X(1, ("Error creating directory at %s", samson_home_.c_str()));
    }
  }
  
  std::string SamsonSetup::setup_filename() const {
    return samson_working_ + "/etc/setup.txt";
  }
  
  std::string SamsonSetup::shared_memory_log_filename()const {
    return samson_working_ + "/log/shared_memory_ids.data";
  }
  
  std::string SamsonSetup::modules_directory() const {
    return samson_home_ + "/modules";
  }
  
  std::string SamsonSetup::worker_modules_directory()const {
    return samson_working_ + "/modules_worker";
  }
  
  std::string SamsonSetup::blocks_directory()const {
    return samson_working_ + "/blocks";
  }
  
  std::string SamsonSetup::block_filename(size_t block_id)const {
    return samson_working_ + "/blocks/" + au::str("block_%lu", block_id);
  }
  
  bool isNumber(std::string txt) {
    for (size_t i = 0; i < txt.length(); i++) {
      if (!au::IsCharInRange(txt[i], 48, 57)) {
        return false;
      }
    }
    return true;
  }
  
  size_t SamsonSetup::block_id_from_filename(const std::string& fileName) const {
    std::string path = samson_working_ + "/blocks/block_";
    
    if (fileName.substr(0, path.size()) != path) {
      return false;
    }
    
    // Take the rest of the name
    std::string res_path = fileName.substr(path.size());
    
    return atoll(res_path.c_str());
  }
  
  std::string SamsonSetup::stream_manager_log_filename() const {
    return samson_working_ + "/log/log_stream_state.txt";
  }
  
  std::string SamsonSetup::cluster_information_filename() const {
    return samson_working_ + "/log/log_cluster_information.txt";
  }
  
  bool SamsonSetup::Save() {
    return SetupItemCollection::Save( setup_filename() );
  }
 
  SamsonSetup* SharedSamsonSetup()
  {
    return au::Singleton<SamsonSetup>::shared();
  }

}
