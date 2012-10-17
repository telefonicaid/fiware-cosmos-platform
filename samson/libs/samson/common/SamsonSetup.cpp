#include <iomanip>                     // std::setw
#include <iostream>                    // std::cerr
#include <stdio.h>

#include <errno.h>

#include "logMsg/logMsg.h"             // LM_X

#include "au/CommandLine.h"            // au::CommandLine
#include "au/ErrorManager.h"           // au::ErrorManager
#include "au/string.h"                 // au::Format

#include "au/containers/StringVector.h"
#include "au/tables/Table.h"

#include "status.h"                    // Status codes

#include "samsonVars.h"                // HOME AND WORKING DIRECTORIES

#include "samson/common/SamsonSetup.h"  // Own interface
#include "samson/common/samsonDirectories.h"    // SAMSON_SETUP_FILE

namespace samson {
  
  SAMSON_EXTERNAL_VARS;
  
  SetupItem::SetupItem(const std::string& name, const std::string& default_value, const std::string& description, SamsonAdaptorType type) :
  name_(name)
  ,default_value_(default_value)
  ,value_(default_value)
  ,description_(description)
  ,type_(type)
  {
    
    if (!CheckValidValue(default_value)) {
      LM_W(("Default value %s not valid for setup item %s", default_value_.c_str(), name_.c_str()));
    }
  }
  
  bool SetupItem::set_value(const std::string& value) {
    if (!CheckValidValue(value)) {
      return false;
    }
    value_ = value;
    return true;
  }
  
  bool SetupItem::CheckValidValue(const std::string& value) const {
    
    if (type_ == SetupItem_string) {
      return true;    // Everything is valid
    }
    
    if (type_ == SetupItem_uint64) {
      size_t p  = value.find_first_not_of("0123456789");
      return (p == std::string::npos);
    }
    
    // Unknown type
    return false;
  }
  
  std::string SetupItem::value() const {
    return value_;
  }
  
  std::string SetupItem::default_value() const {
    return default_value_;
  }
  
  std::string SetupItem::description() const {
    return description_;
  }
  
  std::string SetupItem::concept() const {
    return name_.substr(0, name_.find(".", 0));
  }
  
  void SetupItem::ResetToDefaultValue() {
    value_ = default_value_;
  }
  
#pragma mark SetupItemCollection
  
  SetupItemCollection::~SetupItemCollection() {
    items_.clearMap();
  }
  
  void SetupItemCollection::AddItem(const std::string& name
                                    , const std::string& default_value
                                    , const std::string& description
                                    , SamsonAdaptorType type) {
    if (items_.findInMap(name) != NULL) {
      LM_W(("Item %s already added to setup... ignoring", name.c_str()));
      return;
    }
    
    items_.insertInMap(name, new SetupItem(name, default_value, description, type));
  }
  
  
  bool SetupItemCollection::Save( const std::string& fileName )
  {
    
    FILE *file = fopen(fileName.c_str(), "w");
    
    if (!file) {
      LM_W(("Impossible to open setup file %s", fileName.c_str()));
      return false;
    }
    
    fprintf(file, "# ----------------------------------------------------------------------------------------\n");
    fprintf(file, "# SAMSON SETUP\n");
    fprintf(file, "# ----------------------------------------------------------------------------------------\n");
    fprintf(file, "# File auto-generated using the samsonConfig tool.\n");
    fprintf(file, "# Edit manually, or use samsonConfig to regenerate.\n\n\n");
    
    
    
    au::map< std::string, SetupItem >::iterator i;
    
    // First concept
    std::string concept = "Z";
    
    for (i = items_.begin(); i != items_.end(); i++) {
      std::string tmp_concept = i->second->concept();
      
      if (tmp_concept != concept) {
        fprintf(file, "# ------------------------------------------------------------------------ \n");
        fprintf(file, "# SECTION: %s\n", tmp_concept.c_str());
        fprintf(file, "# ------------------------------------------------------------------------ \n\n");
        concept = tmp_concept;
      }
      
      fprintf(file, "%-40s\t%-20s # %s\n"
              , i->first.c_str()
              , i->second->value().c_str()
              , i->second->description().c_str());
    }
    
    fclose(file);
    
    return true;
  }
  
  bool SetupItemCollection::Load(const std::string& fileName) {
    
    FILE *file = fopen(fileName.c_str(), "r");
    
    if (!file) {
      LM_W(("Warning: Setup file %s not found\n", fileName.c_str()));
      return false;
    }
    
    char line[2000];
    while (fgets(line, sizeof(line), file)) {
      au::CommandLine c;
      c.Parse(line);
      
      if (c.get_num_arguments() == 0) {
        continue;    // Skip comments
      }
      std::string mainCommand = c.get_argument(0);
      if (mainCommand[0] == '#') {
        continue;
      }
      if (c.get_num_arguments() >= 2) {
        std::string name = c.get_argument(0);
        std::string value =  c.get_argument(1);
        
        SetupItem *item = items_.findInMap(name);
        
        if (item) {
          item->set_value(value);
        } else {
          LM_W(("Unknown parameter %s found in setup file %s", name.c_str(), fileName.c_str()));
        }
      }
    }
    
    fclose(file);
    return  true;
  }
  
  std::string SetupItemCollection::Get(const std::string& name) const {
    SetupItem *item = items_.findInMap(name);
    if (!item) {
      LM_E(("Parameter %s not defined in the setup. This is not acceptable", name.c_str()));
      return "Error";
    }
    return item->value();
  }

  std::string SetupItemCollection::GetDefault(const std::string& name) const {
    SetupItem *item = items_.findInMap(name);
    if (!item) {
      LM_E(("Parameter %s not defined in the setup. This is not acceptable", name.c_str()));
      return "Error";
    }
    return item->default_value();
  }
  
  size_t SetupItemCollection::GetUInt64(const std::string& name) const {
    std::string value = Get(name);
    return atoll(value.c_str());
  }
  
  int SetupItemCollection::GetInt(const std::string& name) const {
    std::string value = Get(name);
    return atoi(value.c_str());
  }
  
  bool SetupItemCollection::IsParameterDefined(const std::string& name) const {
    return ( items_.findInMap(name) != NULL);
  }
  
  bool SetupItemCollection::Set(const std::string& name, const std::string& value) {
    SetupItem *item = items_.findInMap(name);
    if (!item) {
      LM_W(("Parameter %s not defined in the setup. This is not acceptable", name.c_str()));
      return false;
    }
    return item->set_value(value);
  }
  
  void SetupItemCollection::ResetToDefaultValues() {
    au::map< std::string, SetupItem >::iterator it_items;
    for (it_items = items_.begin(); it_items != items_.end(); it_items++) {
      it_items->second->ResetToDefaultValue();
    }
  }
  
  std::string SetupItemCollection::str() const {
    au::tables::Table table("Parameter,left|Default value|Current value,left|Description,left");
    
    table.setTitle("Samson setup parameters");
    
    std::ostringstream output;
    au::map< std::string, SetupItem >::const_iterator i;
    for (i = items_.begin(); i != items_.end(); i++) {
      au::StringVector values;
      values.push_back(i->first);
      values.push_back(i->second->default_value());
      values.push_back(i->second->value());
      values.push_back(i->second->description());
      
      table.addRow(values);
    }
    return table.str();
  }
  
  std::vector<std::string> SetupItemCollection::GetItemNames() const {
    return items_.getKeysVector();
  }

  
#pragma mark SamsonSetup
  
  SamsonSetup::SamsonSetup() {
    
    // General Platform parameters
    AddItem("general.memory", "10000000000", "Global available memory ", SetupItem_uint64);
    AddItem("general.num_processess", "16", "Number of cores", SetupItem_uint64);
    AddItem("general.shared_memory_size_per_buffer", "268435456", "Size of the shared memory segments", SetupItem_uint64);
    AddItem("general.update_status_period", "2"
        , "Period for the automatic update from workers to all delilah"
        , SetupItem_uint64);
    
    // Isolation Process
    AddItem("isolated.timeout", "300", "Timeout for all 3rd partty operations", SetupItem_uint64);
    
    // Worker
    AddItem("worker.period_check_finish_tasks", "5",
        "Period to review finished tasks in samsonWorker, to be clean from memory",
        SetupItem_uint64);
    
    // Upload & Download operations
    AddItem("load.buffer_size", "67108864", "Size of the data block for load operations", SetupItem_uint64);
    
    // Stream processing
    AddItem("stream.max_scheduled_write_size", "120000000", "Maximum size scheduled for writing in the BlockManager",
        SetupItem_uint64);
    AddItem("stream.max_scheduled_read_size", "120000000", "Maximum size scheduled for writing in the BlockManager",
        SetupItem_uint64);
    
    AddItem("stream.max_operation_input_size", "400000000",
        "Maximum input data ( in bytes ) to run an automatic stream processing task",
        SetupItem_uint64);
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
    return samson_working_ + "/blocks/" + au::str("%block_%lu", block_id);
  }
  
  bool isNumber(std::string txt) {
    for (size_t i = 0; i < txt.length(); i++) {
      if (!au::isCharInRange(txt[i], 48, 57)) {
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
  
}
