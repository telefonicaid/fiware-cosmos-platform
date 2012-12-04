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

#ifndef AU_LOG_H
#define AU_LOG_H

#include "au/log/LogMain.h"
#include <string>

/**
 * \namespace au
 * \brief Namespace for au library
 * @author Andreu Urruela, Gregorio Escalada & Ken Zangelin
 */

namespace au {
/**
 * \brief Namespace for Google Protocol Buffers objects in au lirbary
 */
namespace gpb {
}

class AULogChannels {
public:

  AULogChannels() {
    // By default, channels are not registered
    registered_ = false;

    // Default to -1 to do not emit if not registered
    zoo = -1;
  };

  void RegisterChannels() {
    if (registered_) {
      return;
    }
    registered_ = true;
    zoo  = LOG_REGISTER_CHANNEL("au::ZOO", "au library : Zookeeper wrapper");
    gpb  = LOG_REGISTER_CHANNEL("au::GPB", "au library : Google Protocol Buffers wrapper");
    rest = LOG_REGISTER_CHANNEL("au::R", "au library : Rest interface");
    excessive_time = LOG_REGISTER_CHANNEL("au::E", "au library : Excesive time");
    thread_manager = LOG_REGISTER_CHANNEL("au::TM", "au library : Thread manager");
    file_descriptor = LOG_REGISTER_CHANNEL("au::TM", "au library : File descriptor");
    listener = LOG_REGISTER_CHANNEL("au::L", "au library : Network listener");
  }

  // Channels definied in this library
  int zoo;
  int gpb;
  int rest;
  int excessive_time;
  int thread_manager;
  int file_descriptor;
  int listener;
private:
  bool registered_;
};

// Extern variable to logeverything
extern AULogChannels logs;
}

#endif  // ifndef AU_LOG_H