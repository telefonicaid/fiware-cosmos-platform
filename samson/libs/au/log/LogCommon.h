

#ifndef _H_LOG_SERVER_COMMON
#define _H_LOG_SERVER_COMMON


#include <string>

// Channel definition
#define AU_LOG_MAX_CHANNELS 1024
#define AU_LOG_CHANNEL_MESSAGE 0
#define AU_LOG_CHANNEL_WARNING 1
#define AU_LOG_CHANNEL_ERROR   2

// Log Server
#define AU_LOG_SERVER_PORT       6001
#define AU_LOG_SERVER_QUERY_PORT 6000
#define AU_LOG_SERVER_DIRECTORY  "/var/log/logserver/"


// Formatting
//#define AU_LOG_DEFAULT_FORMAT "TYPE : date : time : EXEC : FILE[LINE] : FUNC : TEXT"
#define AU_LOG_DEFAULT_FORMAT "channel_alias : date : time : exec : file[line] : function : text"

#define AU_LOG_M 0
#define AU_LOG_W 1
#define AU_LOG_E 2
#define AU_LOG_X 3

#define AU_LOG_V  4
#define AU_LOG_V2 4
#define AU_LOG_V3 5
#define AU_LOG_V4 6
#define AU_LOG_V5 7


// MACROS TO EMIT LOGS

#define AU_LM_T(c, s)                                                            \
do {                                                                             \
if( au::log_central.CheckLogChannel(c))                                          \
{                                                                                \
  au::Log log;                                                                   \
  log.Set("text",au::str s );                                                    \
  log.Set("function", __FUNCTION__);                                             \
                                                                                 \
  std::string file = __FILE__;                                                   \
  size_t pos =file.find_last_of('/');                                            \
  if( pos == std::string::npos )                                                 \
  log.Set("file", file );                                                        \
  else                                                                           \
  log.Set("file", file.substr(pos+1) );                                          \
                                                                                 \
au::LogData& log_data = log.log_data();                                          \
  log_data.line = __LINE__;                                                      \
  log_data.channel = c;                                                          \
  log_data.tv.tv_sec = time(NULL);                                               \
  log_data.tv.tv_usec = 0;                                                       \
  log_data.timezone = 0;                                                         \
  log_data.pid = getpid();                                                       \
                                                                                 \
  au::log_central.Emit(&log);                                                    \
}                                                                                \
} while (0)                                                                      \


#define AU_LM_M(s) AU_LM_T(AU_LOG_M,s)
#define AU_LM_W(s) AU_LM_T(AU_LOG_W,s)
#define AU_LM_E(s) AU_LM_T(AU_LOG_E,s)
#define AU_LM_X(s) AU_LM_T(AU_LOG_X,s)

#define AU_LM_V(s)  AU_LM_T(AU_LOG_V,s)
#define AU_LM_V2(s) AU_LM_T(AU_LOG_V2,s)
#define AU_LM_V3(s) AU_LM_T(AU_LOG_V3,s)
#define AU_LM_V4(s) AU_LM_T(AU_LOG_V4,s)
#define AU_LM_V5(s) AU_LM_T(AU_LOG_V5,s)

#endif
