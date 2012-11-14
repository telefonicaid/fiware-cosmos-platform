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


#ifndef _H_LOG_SERVER_COMMON
#define _H_LOG_SERVER_COMMON


#include <pthread.h>
#include <string.h>
#include <string>

// Channel definition
#define LOG_MAX_CHANNELS               1024

// Log Server
#define LOG_SERVER_DEFAULT_PORT        6001
#define LOG_SERVER_DEFAULT_CLIENT_PORT 6000
#define LOG_SERVER_DEFAULT_DIRECTORY   "/var/log/logserver/"

// LOG LEVELS
#define LOG_LEVEL_ERROR_EXIT           0 // always accepted and exits program
#define LOG_LEVEL_ERROR                1
#define LOG_LEVEL_WARNING              2
#define LOG_LEVEL_VERBOSE              3
#define LOG_LEVEL_MESSAGE              4
#define LOG_LEVEL_DEBUG                5


// Formatting
// #define LOG_DEFAULT_FORMAT "TYPE : date : time : EXEC : FILE[LINE] : FUNC : TEXT"
#define LOG_DEFAULT_FORMAT             "[type][exec_short[pid]] : channel : date : time : file[line] : function : text"
#define LOG_DEFAULT_FORMAT_LOG_CLIENT  "[type][exec_short[pid]@host] : channel : date : time : function : text"
#define LOG_DEFAULT_FORMAT_CONSOLE     "[type][time][channel] text"


// MACRO TO REGISTER A LOG CHANNEL

#define LOG_REGISTER_CHANNEL(name, description) ::au::log_central.log_channels().RegisterChannel(name, description)

// MACROS TO EMIT LOGS

#define LOG_GENERATE(l, c, s)                                                              \
  do {                                                                                      \
    if (au::log_central.IsLogAccepted(c, l))                                                 \
    {                                                                                       \
      au::Log log;                                                                          \
      log.Set("text", au::str s);                                                           \
      log.Set("function", __FUNCTION__);                                                    \
                                                                                            \
      std::string file = __FILE__;                                                          \
      size_t pos = file.find_last_of('/');                                                  \
      if (pos == std::string::npos) {                                                       \
        log.Set("file", file); }                                                            \
      else {                                                                                \
        log.Set("file", file.substr(pos + 1)); }                                            \
                                                                                            \
      au::LogData& log_data = log.log_data();                                               \
      log_data.level = l;                                                                   \
      log_data.line = __LINE__;                                                             \
      log_data.channel = c;                                                                 \
      log_data.tv.tv_sec = time(NULL);                                                      \
      log_data.tv.tv_usec = 0;                                                              \
      log_data.timezone = 0;                                                                \
      log_data.pid = getpid();                                                              \
                                                                                            \
      ::pthread_t t = ::pthread_self();                                                     \
      log_data.tid = 0;                                                                     \
      ::memcpy(&log_data.tid, &t, std::min(sizeof(log_data.tid), sizeof( pthread_t))); \
                                                                                            \
      au::log_central.Emit(&log);                                                           \
    }                                                                                       \
  } while (0)                                                                               \

// General macros to emit logs for a channel

// #define AU_X(c,e,s) LOG_GENERATEAU_LOG_ERROR_EXIT,c,s); au::log_central.StopAndExit(e);
#define LOG_E(c, s)                             LOG_GENERATE(LOG_LEVEL_ERROR, c, s)
#define LOG_W(c, s)                             LOG_GENERATE(LOG_LEVEL_WARNING, c, s)
#define LOG_V(c, s)                             LOG_GENERATE(LOG_LEVEL_VERBOSE, c, s)
#define LOG_M(c, s)                             LOG_GENERATE(LOG_LEVEL_MESSAGE, c, s)
#define LOG_D(c, s)                             LOG_GENERATE(LOG_LEVEL_DEBUG, c, s)

// Macros to emit logs for system channel (0)

// #define AU_SX(e,s) LOG_GENERATEAU_LOG_ERROR_EXIT,0,s); au::log_central.StopAndExit(e);
#define LOG_SE(s)                               LOG_GENERATE(LOG_LEVEL_ERROR, 0, s)
#define LOG_SW(s)                               LOG_GENERATE(LOG_LEVEL_WARNING, 0, s)
#define LOG_SV(s)                               LOG_GENERATE(LOG_LEVEL_VERBOSE, 0, s)
#define LOG_SM(s)                               LOG_GENERATE(LOG_LEVEL_MESSAGE, 0, s)
#define LOG_SD(s)                               LOG_GENERATE(LOG_LEVEL_DEBUG, 0, s)

#endif  // ifndef _H_LOG_SERVER_COMMON
