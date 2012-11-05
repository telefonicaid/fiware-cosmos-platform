

#ifndef _H_LOG_SERVER_COMMON
#define _H_LOG_SERVER_COMMON


#include <string>

// Channel definition
#define AU_LOG_MAX_CHANNELS              1024
#define AU_LOG_CHANNEL_MESSAGE           0
#define AU_LOG_CHANNEL_WARNING           1
#define AU_LOG_CHANNEL_ERROR             2

// Log Server
#define AU_LOG_SERVER_PORT               6001
#define AU_LOG_SERVER_QUERY_PORT         6000
#define AU_LOG_SERVER_DIRECTORY          "/var/log/logserver/"

// LOG LEVELS
#define AU_LOG_ERROR_EXIT 0 // always accepted and exits program
#define AU_LOG_ERROR      1
#define AU_LOG_WARNING    2
#define AU_LOG_VERBOSE    3
#define AU_LOG_MESSAGE    4
#define AU_LOG_DEBUG      5


// Formatting
// #define AU_LOG_DEFAULT_FORMAT "TYPE : date : time : EXEC : FILE[LINE] : FUNC : TEXT"
#define AU_LOG_DEFAULT_FORMAT            "exec_short[pid] : channel : date : time : file[line] : function : text"
#define AU_LOG_DEFAULT_FORMAT_LOG_CLIENT "exec_short[pid]@host : channel : date : time : function : text"
#define AU_LOG_DEFAULT_FORMAT_CONSOLE    "[type][time][channel] text"


// MACRO TO REGISTER A LOG CHANNEL

#define AU_REGISTER_CHANNEL(name) ::au::log_central.log_channels().RegisterChannel( name );

// MACROS TO EMIT LOGS

#define AU_L( l, c, s)                                                            \
  do {                                                                            \
    if (au::log_central.IsLogAccepted(c,l))                                       \
    {                                                                             \
      au::Log log;                                                                \
      log.Set("text", au::str s);                                                 \
      log.Set("function", __FUNCTION__);                                          \
                                                                                  \
      std::string file = __FILE__;                                                \
      size_t pos = file.find_last_of('/');                                        \
      if (pos == std::string::npos) {                                             \
        log.Set("file", file); }                                                  \
      else {                                                                      \
        log.Set("file", file.substr(pos + 1)); }                                  \
                                                                                  \
      au::LogData& log_data = log.log_data();                                     \
      log_data.level = l;                                                         \
      log_data.line = __LINE__;                                                   \
      log_data.channel = c;                                                       \
      log_data.tv.tv_sec = time(NULL);                                            \
      log_data.tv.tv_usec = 0;                                                    \
      log_data.timezone = 0;                                                      \
      log_data.pid = getpid();                                                    \
                                                                                  \
      au::log_central.Emit(&log);                                                 \
    }                                                                             \
  } while (0)                                                                     \

// General macros to emit logs for a channel

//#define AU_X(c,e,s) AU_L(AU_LOG_ERROR_EXIT,c,s); au::log_central.StopAndExit(e);
#define AU_E(c,s)   AU_L(AU_LOG_ERROR,c,s)
#define AU_W(c,s)   AU_L(AU_LOG_WARNING,c,s)
#define AU_V(c,s)   AU_L(AU_LOG_VERBOSE,c,s)
#define AU_M(c,s)   AU_L(AU_LOG_MESSAGE,c,s)
#define AU_D(c,s)   AU_L(AU_LOG_DEBUG,c,s)

// Macros to emit logs for system channel (0)

//#define AU_SX(e,s) AU_L(AU_LOG_ERROR_EXIT,0,s); au::log_central.StopAndExit(e);
#define AU_SE(s)   AU_L(AU_LOG_ERROR,0,s)
#define AU_SW(s)   AU_L(AU_LOG_WARNING,0,s)
#define AU_SV(s)   AU_L(AU_LOG_VERBOSE,0,s)
#define AU_SM(s)   AU_L(AU_LOG_MESSAGE,0,s)
#define AU_SD(s)   AU_L(AU_LOG_DEBUG,0,s)


#endif  // ifndef _H_LOG_SERVER_COMMON
