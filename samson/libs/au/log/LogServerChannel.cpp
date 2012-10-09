#include "LogServerChannel.h"  // Own interface
#include <time.h>
#define Char_to_int(x) ((x) - 48)

#include "logMsg/logMsg.h"                 // LM_T
#include "logMsg/traceLevels.h"            // LmtFileDescriptors, etc.

#include "au/containers/vector.h"
#include "au/gpb.h"

namespace au {
LogServerChannel::LogServerChannel(int port, std::string _directory)
  : network::Service(port), token("LogServerChannel") {
  directory = _directory;

  file_counter = 0;
  current_size = 0;
  fd = NULL;
}

LogServerChannel::~LogServerChannel() {
}

std::string LogServerChannel::getFileNameForLogFile(int counter) {
  return au::str("%s/log_%d", directory.c_str(), counter);
}

void LogServerChannel::openFileDescriptor(au::ErrorManager *error) {
  // Check if disconnected
  if (fd) {
    if (fd->IsClosed()) {
      delete fd;
      fd = NULL;
    }
  }
  if (!fd) {
    std::string current_file_name;

    while (true) {
      // Get the name of the log file
      current_file_name = getFileNameForLogFile(file_counter++);

      struct stat buf;
      int s = stat(current_file_name.c_str(), &buf);
      if (s == 0) {
        continue;             // File exist, so let's try the next number...
      }
      int _fd = open(current_file_name.c_str(), O_CREAT | O_WRONLY, 0644);
      LM_LT(LmtFileDescriptors, ("Open FileDescriptor fd:%d", fd));

      if (_fd < 0) {
        error->set(au::str("Error opening file %s (%s)", current_file_name.c_str(), strerror(errno)));
        return;
      }

      // Create a file descriptor
      fd = new au::FileDescriptor("log fd", _fd);

      // Init byte counter for the current file
      current_size = 0;

      // File created!
      return;
    }
  }
}

void LogServerChannel::initLogServerChannel(au::ErrorManager *error) {
  // Create directory
  if (( mkdir(directory.c_str(), 0755) != 0 ) && ( errno != EEXIST )) {
    error->set(au::str("Error creating directory %s (%s)", directory.c_str(), strerror(errno)));
    return;
  }

  // Open the first file-descriptor
  openFileDescriptor(error);

  if (error->IsActivated()) {
    return;
  }

  au::Status s = InitService();
  if (s != au::OK) {
    error->set(au::str("Error initializing server (%s)", au::status(s)));
    return;
  }
}

void LogServerChannel::run(au::SocketConnection *socket_connection, bool *quit) {
  // Read initial packet to understand if this is a log provider connection or a log prove connection
  gpb::LogConnectionHello *hello;
  au::Status s = readGPB(socket_connection->fd(), &hello, 10);  // 10 seconds timeout to read hello message

  if (s != au::OK) {
    LM_W(("Error reading hello message for incomming connection (%s)", au::status(s)));
    return;
  }

  if (hello->type() == au::gpb::LogConnectionHello_LogConnectionType_LogProbe) {
    // Prove connection
    std::string filter;
    if (hello->has_filter()) {
      filter = hello->filter();
    }

    LogProveConnection log_prove_connection(filter);

    {
      au::TokenTaker tt(&token_log_connections_);
      log_connections_.insert(&log_prove_connection);
    }

    while (!*quit) {
      LogPointer log = log_prove_connection.Pop();

      if (log != NULL) {
        log->Write(socket_connection);
      } else {
        usleep(100000);
      }

      if (socket_connection->IsClosed()) {
        {
          au::TokenTaker tt(&token_log_connections_);
          log_connections_.erase(&log_prove_connection);
        }
        return;
      }
    }
  }


  // Provider
  while (!*quit) {
    // Read a log
    au::SharedPointer<Log>log(new Log());
    if (!log->Read(socket_connection)) {
      LM_V(("Closed connection from %s", socket_connection->host_and_port().c_str()));
      return;  // Not possible to read a log...
    }

    std::string channel = au::str("%s_%s_%d"
                                  , log->Get("EXEC").c_str()
                                  , socket_connection->host_and_port().c_str()

                                  , log->log_data().pid);
    // Additional information to the log
    log->Set("channel", channel);
    log->Set("host", socket_connection->host_and_port());  // Additional information to the log


    // Push logs to probes
    {
      au::TokenTaker tt(&token_log_connections_);
      std::set<LogProveConnection *>::iterator iterator;
      for (iterator = log_connections_.begin(); iterator != log_connections_.end(); iterator++) {
        (*iterator)->Push(log);
      }
    }

    // Add log...
    add(log);
  }
}

void LogServerChannel::add(au::SharedPointer<Log> log) {
  // Mutex protection
  au::TokenTaker tt(&token);

  // Push log to the on-memory log
  log_container.Push(log);

  // Monitorize rate of logs
  rate.Push(log->SerialitzationSize());

  // Check max size for file
  if (fd) {
    if (current_size > 64000000) {
      fd->Close();
      delete fd;
      fd = NULL;
    }  // Open if necessary
  }
  if (!fd) {
    current_size = 0;

    au::ErrorManager error;
    openFileDescriptor(&error);
    if (error.IsActivated()) {
      LM_W(("Not possible to open local file to save logs. Logs will be deninitelly lost"));
      return;
    }
  }

  // Write to file
  if (fd) {
    log->Write(fd);
  }
  current_size += log->SerialitzationSize();
}

void LogServerChannel::addNewSession() {
  au::SharedPointer<Log> log(new Log());
  log->SetNewSession();
  add(log);

  // Clear on memory logs
  log_container.Clear();
}

class ChannelInfo {
public:

  std::string name_;

  int logs_;            // Number of logs
  size_t size_;         // Total size

  au::Descriptors descriptors_;

  std::string time_;       // Most recent time stamp

  ChannelInfo(std::string name, au::SharedPointer<Log> log) {
    name_ = name;

    logs_ = 0;
    size_ = 0;

    // First time
    time_ = log->Get("date") + " " + log->Get("time");

    add_log(log);
  }

  void add_log(au::SharedPointer<Log> log) {
    logs_++;
    size_ += log->SerialitzationSize();

    std::string type = log->Get("TYPE");
    if (type == "") {
      type = "?";
    }
    descriptors_.Add(type);
  }
};

class ChannelsInfo {
  au::vector<ChannelInfo> channel_info;

public:

  void add_channel(std::string channel, au::SharedPointer<Log> log) {
    for (size_t i = 0; i < channel_info.size(); i++) {
      if (channel_info[i]->name_ == channel) {
        channel_info[i]->add_log(log);
        return;
      }
    }
    // Create a new one
    ChannelInfo *_channel_info = new ChannelInfo(channel, log);
    channel_info.push_back(_channel_info);
  }

  std::string getTable() {
    au::tables::Table table("Channel|Last time|#Logs|Type|Size,f=uint64");

    table.setTitle("Channels");
    for (size_t i = 0; i < channel_info.size(); i++) {
      au::StringVector values;

      values.push_back(channel_info[i]->name_);
      values.push_back(channel_info[i]->time_);

      values.push_back(au::str("%d", channel_info[i]->logs_));
      values.push_back(channel_info[i]->descriptors_.str());
      values.push_back(au::str("%lu", channel_info[i]->size_));

      table.addRow(values);
    }
    return table.str();
  }

  size_t getNumChannels() {
    return channel_info.size();
  }
};


std::string LogServerChannel::getInfo() {
  return log_container.getInfo();
}

std::string LogServerChannel::getChannelsTable(au::CommandLine *cmdLine) {
  // Get formats from
  int limit             = cmdLine->GetFlagInt("limit");
  bool is_multi_session = cmdLine->GetFlagBool("multi_session");
  bool file             = cmdLine->GetFlagBool("file");

  // Get current log file
  int tmp_file_counter = file_counter;

  // Information about channels
  ChannelsInfo channel_info;

  // Table based on on-memory logs
  // --------------------------------------------------------
  if (!file) {
    std::vector< au::SharedPointer<Log> > logs = log_container.logs();
    for (size_t i = 0; i < logs.size(); i++) {
      // Get channel
      std::string channel = logs[i]->Get("channel");

      // Collect this information
      channel_info.add_channel(channel, logs[i]);

      if (limit > 0) {
        if (((int)channel_info.getNumChannels()) >= limit) {
          break;
        }
      }
    }
    return channel_info.getTable();
  }

  // Table based on files
  // --------------------------------------------------------
  bool finish = false;
  while (!finish) {
    // read file of logs

    au::ErrorManager error;
    std::string file_name = getFileNameForLogFile(tmp_file_counter);
    std::vector< au::SharedPointer<Log> > logs = readLogFile(file_name, error);

    if (!error.IsActivated()) {
      size_t num_logs = logs.size();
      for (size_t i = 0; i < num_logs; i++) {
        // Get log and add to the table log formatter
        au::SharedPointer<Log> log = logs[ num_logs - i - 1 ];

        if (!is_multi_session && log->IsNewSession()) {
          finish = true;
          break;
        }

        // Get channel
        std::string channel = log->Get("channel");

        // Collect this information
        channel_info.add_channel(channel, log);

        if (limit > 0) {
          if (((int)channel_info.getNumChannels()) >= limit) {
            finish = true;
            break;
          }
        }
      }
    }

    // Move to the previous log file
    tmp_file_counter--;

    // Finish if no more files to scan
    if (tmp_file_counter < 0) {
      break;
    }
  }

  return channel_info.getTable();
}

std::string LogServerChannel::getTable(au::CommandLine *cmdLine) {
  // Mutex protection is not necessary any more since it is based on files
  // au::TokenTaker tt(&token);

  // Get formats from
  std::string format = cmdLine->GetFlagString("format");
  int limit = cmdLine->GetFlagInt("limit");
  bool is_table = cmdLine->GetFlagBool("table");
  bool is_reverse = cmdLine->GetFlagBool("reverse");
  bool is_multi_session = cmdLine->GetFlagBool("multi_session");
  bool file = cmdLine->GetFlagBool("file");

  std::string pattern = cmdLine->GetFlagString("pattern");
  std::string str_time = cmdLine->GetFlagString("time");
  std::string str_date = cmdLine->GetFlagString("date");
  std::string str_type = cmdLine->GetFlagString("type");
  std::string channel = cmdLine->GetFlagString("channel");
  // Formatter to create table
  TableLogFormatter table_log_formater(format);

  // Setup of the table log formatter
  table_log_formater.set_pattern(pattern);
  table_log_formater.set_time(str_time);
  table_log_formater.set_date(str_date);
  table_log_formater.set_as_table(is_table);
  table_log_formater.set_reverse(is_reverse);
  table_log_formater.set_as_multi_session(is_multi_session);
  table_log_formater.set_limit(limit);
  table_log_formater.set_channel(channel);
  table_log_formater.set_type(str_type);

  au::ErrorManager error;
  table_log_formater.init(error);

  if (error.IsActivated()) {
    return au::str("Error: %s", error.GetMessage().c_str());
  }

  // Get current log file
  int tmp_file_counter = file_counter;

  // Table based on on-memory logs
  // --------------------------------------------------------
  if (!file) {
    std::vector< au::SharedPointer<Log> > logs = log_container.logs();
    for (size_t i = 0; i  < logs.size(); i++) {
      // Add the log to the table formatter
      table_log_formater.add(logs[i]);

      if (table_log_formater.enougthRecords()) {
        break;
      }
    }

    return table_log_formater.str();
  }

  // Table based on files
  // --------------------------------------------------------
  while (!table_log_formater.enougthRecords()) {
    au::ErrorManager error;
    std::string file_name = getFileNameForLogFile(tmp_file_counter);
    std::vector< au::SharedPointer<Log> > logs = readLogFile(file_name, error);

    if (!error.IsActivated()) {
      // Read logs from this file
      size_t num_logs = logs.size();
      for (size_t i = 0; i < num_logs; i++) {
        table_log_formater.add(logs[ num_logs - i - 1]);
      }
    }

    // Move to the previous log file
    tmp_file_counter--;

    // Finish if no more files to scan
    if (tmp_file_counter < 0) {
      break;
    }
  }

  return table_log_formater.str();
}
}
