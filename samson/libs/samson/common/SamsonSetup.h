#ifndef _H_SAMSON_SETUP
#define _H_SAMSON_SETUP

#include <iostream>            // std::cout
#include <map>                 // std::map
#include <stdlib.h>            // atoll
#include <string>              // std::string
#include <sys/stat.h>          // mkdir

#include "logMsg/logMsg.h"

#include "au/Environment.h"  // au::Environment
#include "au/Singleton.h"
#include "au/file.h"
#include "au/containers/StringVector.h"
#include "au/containers/map.h"  // au::map

#include "samson/common/status.h"


namespace au {
class ErrorManager;
}

namespace samson {

typedef enum {
  SetupItem_uint64,
  SetupItem_string
} SamsonAdaptorType;

class SetupItem {

public:

  SetupItem( const std::string& name
            , const std::string& default_value
            , const std::string& description
            , SamsonAdaptorType type );
  
  bool set_value(const std::string& value);
  
  std::string value() const;
  std::string default_value() const;
  std::string description() const;
  std::string concept() const;
  
  bool CheckValidValue(const std::string& _value) const;
  
  void ResetToDefaultValue();
  
private:

std::string name_;
std::string default_value_;
std::string value_;
std::string description_;
SamsonAdaptorType type_;

};


class SetupItemCollection {
protected:

public:

  SetupItemCollection(){}
  ~SetupItemCollection();

  // Add a new setup parameter to consider
  void AddItem(const std::string& _name
               , const std::string& _default_value
               , const std::string& _description
               , SamsonAdaptorType type);
  
  // Load a particular file to include all setup parameters
  bool Load(const std::string& file);
  bool Save(const std::string& file);

  // Get access to parameters
  std::string Get(const std::string& name) const;
  bool Set(const std::string& name, const std::string& value);
  std::string GetDefault(const std::string& name) const;  // Access to default value
  size_t GetUInt64(const std::string& name) const;
  int GetInt(const std::string& name) const;
  
  // Check if a particular property if defined
  bool IsParameterDefined(const std::string& name) const;

  // Come back to default parameters
  void ResetToDefaultValues();

  //Debug string
  std::string str() const;

  // Get a vector with all parameter to setup
  std::vector<std::string> GetItemNames() const;
  
private:
  
  // Collection of setup items
  au::map< std::string, SetupItem > items_;
  
};


class SamsonSetup : public SetupItemCollection {

public:

  // Modify main working directories
  void SetWorkerDirectories( const std::string& samson_home, const std::string& samson_working );

  // Save to file in samson_working directory
  bool Save();

  // Get names of files and directories based on current setup
  std::string setup_filename() const;
  std::string shared_memory_log_filename() const;
  std::string modules_directory() const;
  std::string worker_modules_directory() const;
  std::string blocks_directory() const;
  std::string block_filename(size_t block_id) const;
  size_t block_id_from_filename(const std::string& fileName) const;
  std::string stream_manager_log_filename() const;
  std::string cluster_information_filename() const;
  std::vector<std::string> items_names() const;
  
private:

  friend class au::Singleton<SamsonSetup>;
  SamsonSetup();  // Constructor to be used only with au::Singleton
  
  std::string samson_home_;                 // Home directory for SAMSON system
  std::string samson_working_;              // Working directory for SAMSON system

  
};
}

#endif  // ifndef _H_SAMSON_SETUP
