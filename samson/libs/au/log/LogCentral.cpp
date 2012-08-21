
#include "LogCentral.h"  // Own interface

namespace au {
LogCentral::LogCentral(std::string host, int port, std::string local_file)
  : token_("PermanentSocketConnection")
    , token_plugins_("token_plugins")
    , token_current_thread_("token_current_thread") {
      host_ = host;
      port_ = port;
      local_file_=local_file;

  // By default both connections are NULL
  socket_connection_ = NULL;
  local_file_descriptor_ = NULL;

  // Connect the next time a trace is sent
  time_reconnect_ = 0;

  // No current thread by defautl
  current_thread_activated_ = false;

  // Direct mode flag
  direct_mode_ = false;

  // Not using any fd here
  fd_ = -1;
}

LogCentral::~LogCentral() {
  close_socket_connection();
  close_local_file();
}

  std::string LogCentral::host() const {
    return host_;
  }
  
  int LogCentral::port() const{
    return port_;
  }
  
  std::string LogCentral::local_file() const
  {
    return local_file_;
  }
  
  int LogCentral::getFd() const
  {
    return fd_;
  }

  
void LogCentral::close_socket_connection() {
  if (socket_connection_) {
    socket_connection_->Close();
    delete socket_connection_;
    socket_connection_ = NULL;

    // This is the time counter since last connection
    time_since_last_connection_.Reset();

    // Force try reconnect in next log
    time_reconnect_ = 0;
  }
}

void LogCentral::close_local_file() {
  if (local_file_descriptor_) {
    local_file_descriptor_->Close();
    delete local_file_descriptor_;
    local_file_descriptor_ = NULL;
  }
}

void LogCentral::SetLogServer(std::string host, int port) {
  au::TokenTaker tt(&token_);

  // Set the new address
  host_ = host;
  port_ = port;

  // Close previous connections ( if any )
  close_socket_connection();
  close_local_file();

  // Connect the next time a trace is sent
  time_reconnect_ = 0;
}

void LogCentral::Write(au::SharedPointer<Log> log ) {
  // In direct mode, we just try to send traces ( not reconnection, no blocking )
  if (direct_mode_) {
    if (socket_connection_)
      log->Write(socket_connection_);
    else if (local_file_descriptor_)
      log->Write(local_file_descriptor_);
    return;
  }

  // If we are the writing thread, just return ( otherwise it will be locked )
  // If the system is taken, just wait
  while (true) {
    {
      // Mutex protection
      au::TokenTaker tt(&token_current_thread_);

      // Get my thread id
      pthread_t my_thread_id = pthread_self();

      // Check if I am blocking this log
      if (current_thread_activated_) {
        if ( pthread_equal(current_thread_ , my_thread_id ) ) {
          return;               // Secondary log...
        }
      } else {
        current_thread_ = my_thread_id;
        current_thread_activated_ = true;
        break;
      }
    }

    // Sleep waiting to send traces
    usleep(10000);
  }

  // Real write operation
  write_to_plugins(log);
  write_to_server_or_file(log);       // Log is accumulated and finally removed

  {
    // Mutex protection
    au::TokenTaker tt(&token_current_thread_);

    // Deactivate this lock
    current_thread_activated_ = false;
  }
}

  void LogCentral::AddPlugin(LogPlugin *p) {
  au::TokenTaker tt(&token_plugins_);
  plugins_.insert(p);
}

void LogCentral::RemovePlugin(LogPlugin *p) {
  au::TokenTaker tt(&token_plugins_);
  plugins_.erase(p);
}

  void LogCentral::write_to_plugins(au::SharedPointer<Log> log) {
  au::TokenTaker tt(&token_plugins_);

  au::set<LogPlugin>::iterator it;
  for (it = plugins_.begin(); it != plugins_.end(); it++) {
    (*it)->process_log(log);
  }
}

void LogCentral::write_to_server_or_file(SharedPointer<Log> log) {
  au::TokenTaker tt(&token_);

  // In direct mode, we do not reconnect

  // Check if the estabished connection should be canceled
  if (socket_connection_ && socket_connection_->IsClosed()) {
    close_socket_connection();
    fd_ = -1;
  }

  // Reconnect to server if necessary
  size_t time = time_since_last_connection_.seconds();
  if (!socket_connection_ && ( host_ != "" )  && ( time >= time_reconnect_ )) {
    // Try new connection with the server
    au::Status s = au::SocketConnection::Create(host_, port_, &socket_connection_);

    if (s != au::OK) {
      // Set the next time_reconnect
      if (time == 0)
        time_reconnect_ = 120; if (time_reconnect_ < time)
        time_reconnect_ = time; time_reconnect_ *= 2;           // Increase time to reconnect

      size_t next_try_time = time_reconnect_ - time;

      LM_W(("Not possible to connect with log server %s:%d (%s). Next try in %s"
            , host_.c_str(), port_, au::status(s), au::str_time(next_try_time).c_str()));
    } else {
      // Set the used fd
      fd_ = socket_connection_->fd();

      if (time  > 10)
        LM_LW(("Connected to log server after %s disconnected", au::str_time(time).c_str()));
    }
  }


  // Try socket first...
  if (socket_connection_) {
    if (log->Write(socket_connection_))
      return;

    // It was not possible to sent to server, close and remove socket connection
    close_socket_connection();
    fd_ = -1;
  }

  // Make sure, local file is ready to write

  if (!local_file_descriptor_) {
    fd_ = open(local_file_.c_str(), O_WRONLY | O_CREAT, 0644);
    LM_LT(LmtFileDescriptors, ("Open FileDescriptor fd:%d", fd_));

    if (fd_ >= 0) {
      local_file_descriptor_ = new FileDescriptor("local_log", fd_);
    } else {
      LM_LW(("Not possible to open local log file %s. Logs will be definitely lost", local_file_.c_str()));
      return;
    }
  }

  if (!log->Write(local_file_descriptor_)) {
    close_local_file();
    return;
  }
}

void LogCentral::SetDirectMode(bool flag) {
  direct_mode_ = flag;
}
}
