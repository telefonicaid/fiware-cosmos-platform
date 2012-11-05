#ifndef _H_SAMSON_COMMON_COMMON
#define _H_SAMSON_COMMON_COMMON

#include <cstring>  // size_t
#include "au/Log.h"
#include "au/log/LogMain.h"

#define MAX_UINT_32           4294967291
#define KVFILE_MAX_KV_SIZE    64 * 1024 * 1024                                            // Max size for an individual key-value
#define KVFILE_NUM_HASHGROUPS 64 * 1024                                                   // Number of hash-groups

namespace samson {
// Unsigned types with different bits lengths

typedef size_t uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

  class LogChannels
  {
    
  public:
    
    LogChannels()
    {
      // By default, channels are not registered
      registered_ = false;
      
      // Default to -1 to do not emit if not registered
      worker_controller = -1;
      
    };
    
    void RegisterChannels()
    {
      if( registered_ )
        return;
      registered_ = true;
      
      samson             = AU_REGISTER_CHANNEL( "samson");
      worker             = AU_REGISTER_CHANNEL( "samson::W");
      block_manager      = AU_REGISTER_CHANNEL( "samson::BM");
      task_manager       = AU_REGISTER_CHANNEL( "samson::TM");
      worker_controller  = AU_REGISTER_CHANNEL( "samson::WC");
      background_process = AU_REGISTER_CHANNEL( "samson::BP");
      isolated_process   =  AU_REGISTER_CHANNEL( "samson::IP");
      worker_command_manager = AU_REGISTER_CHANNEL( "samson::WCM");
      worker_command         = AU_REGISTER_CHANNEL( "samson::WC");

      worker_block_manager = AU_REGISTER_CHANNEL( "samson::WBM");
      block_request        = AU_REGISTER_CHANNEL( "samson::BR");

      data_model = AU_REGISTER_CHANNEL("samson::DM");
    }
    
    // Channels definied in this library
    int samson;
    int worker;
    int block_manager;
    int worker_controller;
    int task_manager;
    int background_process;
    int isolated_process;
    int worker_command_manager;
    int worker_command;
    int worker_block_manager;
    int block_request;
    int data_model;
  private:
    
    bool registered_;
    
  };
  
  // Extern variable to logeverything
  extern LogChannels logs;

  // Init all logs
  void RegisterLogChannels();
  
}

#endif

