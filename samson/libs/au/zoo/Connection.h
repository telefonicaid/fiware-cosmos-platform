
#ifndef SAMSON_LIBS_SAMSON_ZOO_ZOOKEEPERCONNECTION_H_
#define SAMSON_LIBS_SAMSON_ZOO_ZOOKEEPERCONNECTION_H_

#include <string>

#include "zookeeper/zookeeper.h"
#include <google/protobuf/message.h>

#include "au/Log.h"
#include "au/console/Console.h"
#include "au/containers/StringVector.h"
#include "au/containers/set.h"
#include "au/string/StringUtilities.h"
#include "au/tables/Table.h"
#include "engine/Buffer.h"

// Name of the notification
#define notification_zoo_watcher "notification_zoo_watcher"

namespace au {
namespace zoo {
class Connection;

// ----------------------------------------------------------------------
//
// class samson::zoo::Connection
//
// C++ wrapper of zookepper C API with additions for
// google-protocol-buffers and engine library
//
// ----------------------------------------------------------------------

class Connection {
  // Mutex protection
  au::Token token_;

  // Main zookeeper handler
  zhandle_t *handler_;

public:

  // Constructor & destructor
  Connection(const std::string& host
             , const std::string& user
             , const std::string& password);
  Connection();
  ~Connection();

  // Register and unregister ConnetionWatcher objects
  /*
   * Note:
   * zookeeper c API does not allow to remove a watcher.
   * Callbacks will be done using Engine so delegate can be removed
   */

  // Static watcher method ( to be used in the zookepper C API )
  static void static_watcher(zhandle_t *zzh
                             , int type
                             , int state
                             , const char *path
                             , void *watcherCtx);


  // Connection operations
  int Connect(const std::string& host, const std::string& user, const std::string& password);
  int Connect(const std::string& host);
  int WaitUntilConnected(int milliseconds);
  int AddAuth(const std::string& user, const std::string& password);
  void Close();
  bool IsConnected();
  std::string GetStatusString();

  // Create node functions
  int Create(const std::string& path, int flags = 0, const char *value = NULL, int value_len = 0);
  int Create(std::string& path, int flags, engine::BufferPointer buffer);
  int Create(std::string& path, int flags, ::google::protobuf::Message *value);
  int Create(const std::string& path, int flags, ::google::protobuf::Message *value);
  int Create(std::string& path, int flags, const std::string & value);
  int Create(std::string& path, int flags = 0, const char *value = NULL, int value_len = 0);

  // Remove nodes
  int Remove(const std::string&path, int version = -1);

  // Set functions
  int Set(const std::string& path, const char *value = NULL, int value_len = 0, int version = -1);
  int Set(const std::string& path, ::google::protobuf::Message *value, int version = -1);
  
  // Get functions
  int Get(const std::string& path, char *buffer, int *buflen, struct Stat *stat = NULL);
  int Get(const std::string& path, ::google::protobuf::Message *value);
  int Get(const std::string& path, engine::BufferPointer buffe);
  int Get(const std::string& path, std::string& value);
  int Get(const std::string& path
          , size_t engine_id
          , char *buffer = NULL
          , int *buffer_len = NULL
          , struct Stat *stat = NULL);
  int Get(const std::string& path
          , size_t engine_id
          , std::string& value
          , struct Stat *stat = NULL);
  int Get(const std::string& path
          , size_t engine_id
          , ::google::protobuf::Message *value
          , struct Stat *stat = NULL);

  // Exist functions
  int Exists(const std::string& path, struct Stat *stat = NULL);
  int Exists(const std::string& path
             , size_t engine_id
             , struct Stat *stat = NULL);
  // Get childrens
  int GetChildrens(const std::string& path, String_vector *vector);
  int GetChildrens(const std::string& path, au::StringVector& childrens);

  // Connection time in seconds
  double GetConnectionTime();

  size_t get_rate_in(){ return rate_read_.rate(); };
  size_t get_rate_out(){ return rate_write_.rate(); };
  
private:

  // Cronometer since its creation
  au::Cronometer cronometer_;
  
  // Rate measurements
  au::rate::Rate rate_read_;
  au::rate::Rate rate_write_;
  
};
}
}                       // End of namespace samson::zoo

#endif  // SAMSON_LIBS_SAMSON_ZOO_ZOOKEEPERCONNECTION_H_
