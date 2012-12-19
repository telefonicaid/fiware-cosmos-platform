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

#include <memory>

#include "Connection.h"  // Own interface
#include "au/ExecesiveTimeAlarm.h"
#include "au/TemporalBuffer.h"
#include "engine/Engine.h"
#include "engine/Notification.h"
#include "zoo/common.h"

namespace au {
namespace zoo {
Connection::Connection(const std::string& host, const std::string& user, const std::string& password)
  : token_("zoo::Connection") {
  handler_ = NULL;
  Connect(host, user, password);
}

Connection::Connection() : token_("zoo::Connection") {
  handler_ = NULL;
}

Connection::~Connection() {
  // Close connection if it was still open
  Close();
}

// Remove nodes
int Connection::Remove(const std::string&path, int version) {
  au::TokenTaker tt(&token_);

  // Just make sure we are connected
  int rc = WaitUntilConnected(1000);

  if (rc) {
    return rc;
  }

  LOG_V(logs.zoo, ("Delete node %s (version %d)", path.c_str(), version));
  au::ExecesiveTimeAlarm alarm(0, au::str("ZK delete '%s'", path.c_str()));
  return zoo_delete(handler_, path.c_str(), version);
}

int Connection::Set(const std::string& path, const char *value, int value_len, int version) {
  au::TokenTaker tt(&token_);

  // Just make sure we are connected
  int rc = WaitUntilConnected(1000);

  if (rc) {
    return rc;
  }

  // Create a node
  LOG_V(logs.zoo, ("Set node %s (value %d bytes ,version %d)", path.c_str(), value_len, version));
  rate_write_.Push(value_len);
  au::ExecesiveTimeAlarm alarm(0, au::str("ZK set '%s'", path.c_str()));
  return zoo_set(handler_, path.c_str(), value, value_len, version);
}

int Connection::Set(const std::string& path, ::google::protobuf::Message *value, int version) {
  int size = value->ByteSize();

  // Temporal buffer to get data
  au::TemporalBuffer buffer(size);

  if (!value->SerializeToArray(buffer.data(), size)) {
    LOG_E(logs.zoo, ("GPB Serialitzation error for node %s: %s"
                     , path.c_str(), value->InitializationErrorString().c_str()));
    return ZC_ERROR_GPB;
  }

  int rc = Set(path, buffer.data(), size, version);
  return rc;
}

int Connection::Set(const std::string& path, const std::string& value, int version) {
  return Set(path, value.c_str(), value.length(), version);
}

int Connection::Get(const std::string& path
                    , size_t engine_id
                    , char *buffer
                    , int *buffer_len
                    , struct Stat *stat) {
  au::TokenTaker tt(&token_);

  // Get without the buffer is just an exists call
  if (buffer == NULL) {
    return Exists(path, engine_id, stat);
  }

  LOG_V(logs.zoo, ("Get node %s (buffer %d bytes)", path.c_str(), *buffer_len));

  au::ExecesiveTimeAlarm alarm(0, au::str("ZK get '%s'", path.c_str()));
  int rc = zoo_wget(handler_
                    , path.c_str()
                    , static_watcher
                    , (void *)engine_id
                    , buffer
                    , buffer_len
                    , stat);

  if (!rc) {
    rate_read_.Push(*buffer_len);
  }
  return rc;
}

int Connection::Get(const std::string& path
                    , size_t engine_id
                    , std::string& value
                    , struct Stat *stat) {
  while (true) {
    // Get information about state
    struct Stat intern_stat;
    int rc = Exists(path, &intern_stat);
    if (rc) {
      return rc;
    }

    // Temporal buffer with teh size colected at stat
    int buffer_size = intern_stat.dataLength;
    au::TemporalBuffer buffer(buffer_size + 1);

    // Get data using temporal buffer ( and collecting data2 to check if version changed )
    struct Stat intern_stat2;
    rc = Get(path, engine_id, buffer.data(), &buffer_size, &intern_stat2);

    if (rc) {
      return rc;
    }

    if (intern_stat2.version != intern_stat.version) {
      continue;      // Try again
    }

    // Copy stat information for the caller
    if (stat) {
      *stat = intern_stat;
    }

    // Parse the line
    buffer.data()[buffer_size] = '\0';
    value = buffer.data();  // Copy the string to provided output argument 'value'
    return 0;
  }
}

int Connection::Get(const std::string& path
                    , std::string& value
                    , struct Stat *stat) {
  while (true) {
    // Get information about state
    struct Stat intern_stat;
    int rc = Exists(path, &intern_stat);
    if (rc) {
      return rc;
    }

    // Temporal buffer with teh size colected at stat
    int buffer_size = intern_stat.dataLength;
    au::TemporalBuffer buffer(buffer_size + 1);

    // Get data using temporal buffer ( and collecting data2 to check if version changed )
    struct Stat intern_stat2;
    rc = Get(path, buffer.data(), &buffer_size, &intern_stat2);

    if (rc) {
      return rc;
    }

    if (intern_stat2.version != intern_stat.version) {
      continue;        // Try again
    }

    // Copy stat information for the caller
    if (stat) {
      *stat = intern_stat;
    }

    // Parse the line
    buffer.data()[buffer_size] = '\0';
    value = buffer.data();    // Copy the string to provided output argument 'value'
    return 0;
  }
}

int Connection::Get(const std::string& path, ::google::protobuf::Message *value) {
  // Get information about state
  struct Stat stat;
  int rc = Exists(path, &stat);

  if (rc) {
    return rc;
  }

  while (true) {
    // Temporal buffer with teh size colected at stat
    int buffer_size = stat.dataLength;
    au::TemporalBuffer buffer(buffer_size);

    // Get data using temporal buffer ( and collecting data2 to check if version changed )
    struct Stat stat2;
    rc = Get(path, buffer.data(), &buffer_size, &stat2);

    if (rc) {
      return rc;
    }

    if (stat2.version != stat.version) {
      // Version has changed while reading, get stat again and try again...
      int rc = Exists(path, &stat);
      if (rc) {
        return rc;
      }
      continue;
    }

    // It is the same version, so let's read and return
    if (value->ParseFromArray(buffer.data(), buffer_size)) {
      return 0;   // OK
    } else {
      LOG_E(logs.zoo, ("GPB Serialitzation error for node %s: %s"
                       , path.c_str(), value->InitializationErrorString().c_str()));
      LOG_SW(("GPB Serialitzation error for node %s: %s", path.c_str(), value->InitializationErrorString().c_str()));
      return ZC_ERROR_GPB;
    }
  }
}

int Connection::Get(const std::string& path
                    , size_t engine_id
                    , ::google::protobuf::Message *value
                    , struct Stat *stat) {
  while (true) {
    // Get information about node
    struct Stat stat_intern;
    int rc = Exists(path, &stat_intern);
    if (rc) {
      return rc;
    }

    // Temporal buffer
    int buffer_size = stat_intern.dataLength;
    au::TemporalBuffer buffer(buffer_size);

    // Get data based on temporal buffer
    struct Stat stat_intern2;
    rc = Get(path, engine_id, buffer.data(), &buffer_size, &stat_intern2);

    if (rc) {
      return rc;
    }

    if (stat_intern2.version != stat_intern.version) {
      continue;    // Version has changed while reading, get stat again and try again...
    }

    // Copy using the provided link
    if (stat) {
      *stat = stat_intern;
    }

    // Version is the same, so let's parse and return
    if (value->ParseFromArray(buffer.data(), buffer_size)) {
      return 0;    // OK
    } else {
      LOG_SW(("GPB error when ParseFromArray for node %s with a buffer of %d bytes", path.c_str(), buffer_size));
      return ZC_ERROR_GPB;
    }
  }
}

int Connection::Get(const std::string& path, char *buffer, int *buffer_len, struct Stat *stat) {
  au::TokenTaker tt(&token_);

  // We are interested in getting stat(
  LOG_V(logs.zoo, ("Get node %s (buffer %d bytes)", path.c_str(), buffer_len));

  au::ExecesiveTimeAlarm alarm(0, au::str("ZK get '%s'", path.c_str()));
  int rc = zoo_get(handler_, path.c_str(), 0, buffer, buffer_len, stat);

  if (!rc) {
    rate_read_.Push(*buffer_len);
  }

  return rc;
}

int Connection::Get(const std::string& path, engine::BufferPointer buffer) {
  // Just make sure we are connected
  int rc = WaitUntilConnected(1000);

  if (rc) {
    return rc;
  }

  int buffer_len = buffer->max_size();
  rc = Get(path, buffer->data(), &buffer_len, NULL);
  if (!rc) {
    buffer->set_size(buffer_len);                     // Set the real size
  }
  return rc;
}

int Connection::Exists(const std::string& path, struct Stat *stat) {
  au::TokenTaker tt(&token_);

  // We are interested in getting stat(
  au::ExecesiveTimeAlarm alarm(0, au::str("ZK check if exist '%s'", path.c_str()));
  int result =  zoo_exists(handler_, path.c_str(), 0, stat);

  LOG_V(logs.zoo, ("Check exist node %s -> %s", path.c_str(), (result == 0) ? "yes" : "no"));
  return result;
}

int Connection::Exists(const std::string& path, size_t engine_id,
                       struct Stat *stat) {
  au::TokenTaker tt(&token_);

  // We are interested in getting stat(
  au::ExecesiveTimeAlarm alarm(0, au::str("ZK check if exist '%s'", path.c_str()));
  int result = zoo_wexists(handler_
                           , path.c_str()
                           , static_watcher
                           , (void *)engine_id
                           , stat);

  LOG_V(logs.zoo, ("Check exist node %s -> %s", path.c_str(), (result == 0) ? "yes" : "no"));
  return result;
}

int Connection::WaitUntilConnected(int milliseconds) {
  if (!handler_) {
    return ZC_ERROR_NO_CONNECTION;                     // It will never became connected if no connection has been stablished
  }
  au::Cronometer cronomter;
  while (true) {
    if (cronomter.seconds() > ((double)milliseconds / 1000.0)) {
      return ZC_ERROR_CONNECTION_TIMEOUT;
    }

    if (IsConnected()) {
      return 0;                     // Already connected
    }
    usleep(100000);
  }
}

int Connection::GetChildrens(const std::string& path, String_vector *vector) {
  au::TokenTaker tt(&token_);
  int rc = WaitUntilConnected(1000);

  if (rc) {
    return rc;
  }
  LOG_V(logs.zoo, ("Get childrens of node %s", path.c_str()));
  au::ExecesiveTimeAlarm alarm(0, au::str("ZK get childrens of '%s'", path.c_str()));
  return zoo_get_children(handler_, path.c_str(), 0, vector);
}

int Connection::GetChildrens(const std::string& path, au::StringVector& childrens) {
  String_vector vector;

  init_vector(&vector);
  int rc = GetChildrens(path, &vector);

  if (rc) {
    // Some error
    free_vector(&vector);
    return rc;
  }

  for (int i = 0; i < vector.count; i++) {
    childrens.push_back(vector.data[i]);
  }
  free_vector(&vector);

  return rc;
}

// Static C function to give watchers
void Connection::static_watcher(zhandle_t *zzh,
                                int type,
                                int state,
                                const char *path,
                                void *watcherCtx) {
  if ((type == ZOO_CREATED_EVENT)
      || (type == ZOO_DELETED_EVENT)
      || (type == ZOO_CHANGED_EVENT)
      || (type == ZOO_CHILD_EVENT))
  {
    // Recover engine_id using context
    size_t engine_id = (size_t)watcherCtx;

    engine::Notification *notification = new engine::Notification(notification_zoo_watcher);
    notification->AddEngineListener(engine_id);
    notification->environment().Set("path", path);
    notification->environment().Set("state", state);
    notification->environment().Set("type", type);

    // Schedule this notification
    engine::Engine::shared()->notify(notification);
    return;
  }

  if (type == ZOO_SESSION_EVENT) {
    LOG_SW(("Zoo connection is probably disconnected"));
    return;
  }

  if (type == ZOO_NOTWATCHING_EVENT) {
    LOG_SW(("ZOO_NOTWATCHING_EVENT received. Not handeled"));
    return;
  }

  LOG_SW(("Unknown event received for a zoo::Connection %d", type));
}

int Connection::Create(const std::string& path, int flags, const char *value, int value_len) {
  std::string intern_path = path;                     // Intern name to call the other interface

  return Create(intern_path, flags, value, value_len);
}

int Connection::Create(std::string& path, int flags, engine::BufferPointer buffer) {
  if (buffer != NULL || (buffer->size() == 0)) {
    return Create(path, flags, NULL, 0);
  } else {
    return Create(path, flags, buffer->data(), buffer->size());
  }
}

int Connection::Create(std::string& path, int flags, ::google::protobuf::Message *value) {
  if (!value->IsInitialized()) {
    LOG_E(logs.zoo, ("GPB Serialitzation error for node %s: %s"
                     , path.c_str(), value->InitializationErrorString().c_str()));
    return ZC_ERROR_GPB_NO_INITIALIZED;
  }

  int size = value->ByteSize();
  au::TemporalBuffer buffer(size);
  if (!value->SerializeToArray(buffer.data(), size)) {
    LOG_E(logs.zoo, ("GPB Serialitzation error for node %s: %s"
                     , path.c_str(), value->InitializationErrorString().c_str()));
    return ZC_ERROR_GPB;
  }

  int rc = Create(path, flags, buffer.data(), size);
  return rc;
}

int Connection::Create(const std::string& path, int flags, ::google::protobuf::Message *value) {
  int size = value->ByteSize();
  au::TemporalBuffer buffer(size);

  if (!value->SerializeToArray(buffer.data(), size)) {
    LOG_E(logs.zoo, ("GPB Serialitzation error for node %s: %s"
                     , path.c_str(), value->InitializationErrorString().c_str()));
    return ZC_ERROR_GPB;
  }

  return Create(path, flags, buffer.data(), size);
}

int Connection::Create(std::string& path, int flags, const std::string & value) {
  return Create(path, flags, value.c_str(), value.length());
}

int Connection::Create(std::string& path, int flags, const char *value, int value_len) {
  au::TokenTaker tt(&token_);

  // Just make sure we are connected
  int rc = WaitUntilConnected(1000);

  if (rc) {
    return rc;
  }

  char buffer[512];  // Buffer to hold the final name of the node
  size_t buffer_length = sizeof(buffer) / sizeof(char);
  struct ACL ALL_ACL[] = { { ZOO_PERM_ALL, ZOO_AUTH_IDS } };
  struct ACL_vector ACL_VECTOR = { 1, ALL_ACL };

  // Create a node
  LOG_V(logs.zoo, ("Create node %s (Value: %d bytes )", path.c_str(), buffer_length));
  au::ExecesiveTimeAlarm alarm(0, au::str("ZK create '%s'", path.c_str()));
  rc = zoo_create(handler_, path.c_str(), value, value_len, &ACL_VECTOR, flags, buffer, buffer_length - 1);
  if (!rc) {
    path = buffer;                     // Get the new name ( it is different when flag ZOO_SEQUETIAL is used )
    rate_write_.Push(buffer_length);
  }
  return rc;
}

int Connection::Connect(const std::string& host, const std::string& user,
                        const std::string& password) {
  au::TokenTaker tt(&token_);

  int rc = Connect(host);

  if (rc) {
    return rc;
  }

  return AddAuth(user, password);
}

int Connection::Connect(const std::string& host) {
  au::TokenTaker tt(&token_);

  // Close, just it case it was connected
  Close();

  // Init zookeerp
  LOG_V(logs.zoo, ("Init connection to %s", host.c_str()));
  handler_ = zookeeper_init(host.c_str(), NULL, 5000, 0, NULL, 0);
  if (handler_) {
    return WaitUntilConnected(1000);
  } else {
    return ZC_ERROR_CONNECTION_TIMEOUT;                     // Error, not possible to create handler
  }
}

int Connection::AddAuth(const std::string& user, const std::string& password) {
  au::TokenTaker tt(&token_);

  // Just make sure we are connected
  int rc = WaitUntilConnected(1000);

  if (rc) {
    return rc;
  }

  std::string user_password = user + ":" + password;
  LOG_V(logs.zoo, ("Add auth  user: %s", user.c_str()));

  au::ExecesiveTimeAlarm alarm(0, au::str("ZK add credentials"));
  rc = zoo_add_auth(handler_, "digest", user_password.c_str(), user_password.length(), 0, 0);

  // If corect, just wait until connected
  if (rc == 0) {
    rc = WaitUntilConnected(1000);
  }
  return rc;
}

void Connection::Close() {
  au::TokenTaker tt(&token_);

  if (handler_) {
    LOG_V(logs.zoo, ("Close connection"));
    zookeeper_close(handler_);
    handler_ = NULL;
  }
}

bool Connection::IsConnected() {
  au::TokenTaker tt(&token_);

  if (!handler_) {
    return false;
  }
  au::ExecesiveTimeAlarm alarm(0, au::str("ZK check state"));
  return(zoo_state(handler_) == ZOO_CONNECTED_STATE);
}

std::string Connection::GetStatusString() {
  au::TokenTaker tt(&token_);

  // Just make sure we are connected
  int rc = WaitUntilConnected(1000);

  if (rc) {
    return "Unconnected";
  }

  LOG_V(logs.zoo, ("Get connection status"));
  au::ExecesiveTimeAlarm alarm(0, au::str("ZK check state"));
  rc = zoo_state(handler_);

  if (rc == ZOO_EXPIRED_SESSION_STATE) {
    return au::str("ZOO_EXPIRED_SESSION_STATE (%d)", rc);
  }
  if (rc == ZOO_AUTH_FAILED_STATE) {
    return au::str("ZOO_AUTH_FAILED_STATE (%d)", rc);
  }
  if (rc == ZOO_CONNECTING_STATE) {
    return au::str("ZOO_CONNECTING_STATE (%d)", rc);
  }
  if (rc == ZOO_ASSOCIATING_STATE) {
    return au::str("ZOO_ASSOCIATING_STATE (%d)", rc);
  }
  if (rc == ZOO_CONNECTED_STATE) {
    return "Connected";
  }

  return au::str("Unknown (%d)", rc);
}

double Connection::GetConnectionTime() {
  return cronometer_.seconds();
}
}
}