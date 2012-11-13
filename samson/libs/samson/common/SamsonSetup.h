/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef _H_SAMSON_SETUP
#define _H_SAMSON_SETUP

#include <iostream>            // std::cout
#include <map>                 // std::map
#include <stdlib.h>            // atoll
#include <string>              // std::string
#include <sys/stat.h>          // mkdir

#include "logMsg/logMsg.h"

#include "au/Environment.h"  // au::Environment
#include "au/singleton/Singleton.h"
#include "au/file.h"
#include "au/containers/StringVector.h"
#include "au/containers/map.h"  // au::map
#include "au/setup/SetupItemCollection.h"
#include "samson/stream/Block.h"
#include "samson/common/status.h"


namespace samson {

class SamsonSetup : public au::SetupItemCollection {

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

  std::string samson_home()
  {
    return samson_home_;
  }
  
  std::string samson_working()
  {
    return samson_working_;
  }

private:

  friend class au::Singleton<SamsonSetup>;
  SamsonSetup();  // Constructor to be used only with au::Singleton
  
  std::string samson_home_;                 // Home directory for SAMSON system
  std::string samson_working_;              // Working directory for SAMSON system

  
};
  
  SamsonSetup* SharedSamsonSetup();
  
}

#endif  // ifndef _H_SAMSON_SETUP
