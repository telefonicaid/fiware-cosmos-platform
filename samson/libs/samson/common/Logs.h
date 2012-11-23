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

#include "au/log/LogMain.h"

#ifndef SAMSON_COMMON_LOGS_H
#define SAMSON_COMMON_LOGS_H

namespace samson {
class LogChannels {
public:

  LogChannels() {
    // By default, channels are not registered
    registered_ = false;

    // Default to -1 to do not emit if not registered
    worker_controller = -1;
  };

  void RegisterChannels() {
    if (registered_) {
      return;
    }
    registered_ = true;

    samson                 = LOG_REGISTER_CHANNEL("samson::G", "samson library : General channel");
    in_messages            = LOG_REGISTER_CHANNEL("samson::IM", "samson library : Input network messages");
    out_messages           = LOG_REGISTER_CHANNEL("samson::OM", "samson library : Output network messages");
    worker                 = LOG_REGISTER_CHANNEL("samson::W", "samson library : Worker");
    block_manager          = LOG_REGISTER_CHANNEL("samson::BM", "samson library : Block Manager");
    task_manager           = LOG_REGISTER_CHANNEL("samson::TM", "samson library : Task Manager");
    worker_controller      = LOG_REGISTER_CHANNEL("samson::WC", "samson library : WorkerController");
    background_process     = LOG_REGISTER_CHANNEL("samson::BP", "samson library : Background Process");
    isolated_process       = LOG_REGISTER_CHANNEL("samson::IP", "samson library : Isolated Process");
    worker_command_manager = LOG_REGISTER_CHANNEL("samson::WCM", "samson library : Worker Comamnd Manager");
    worker_command         = LOG_REGISTER_CHANNEL("samson::WC", "samson library : Worker Command");
    worker_task            = LOG_REGISTER_CHANNEL("samson::WT", "samson library : Worker Task");
    worker_block_manager   = LOG_REGISTER_CHANNEL("samson::WBM", "samson library : Worker Block Manager");
    block_request          = LOG_REGISTER_CHANNEL("samson::BR", "samson library : Block Request");
    reduce_operation       = LOG_REGISTER_CHANNEL("samson::RO", "samson library : Reduce Operation");
    kv_file                = LOG_REGISTER_CHANNEL("samson::KVF",
                                                  "samson library : KVFile ( key-values data interpretation )");
    data_model             = LOG_REGISTER_CHANNEL("samson::DM", "samson library : Data model");
    rest                   = LOG_REGISTER_CHANNEL("samson::R", "samson library : Rest interface");

    delilah                = LOG_REGISTER_CHANNEL("delilah::G", "delilah library : General channel");
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
  int worker_task;
  int reduce_operation;
  int kv_file;
  int rest;
  int in_messages;
  int out_messages;
  int delilah;

private:

  bool registered_;
};

// Extern variable to logeverything
extern LogChannels logs;

// Init all logs
void RegisterLogChannels();

std::string str_block_id(size_t block_id);
}

#endif  // ifndef SAMSON_COMMON_LOGS_H

