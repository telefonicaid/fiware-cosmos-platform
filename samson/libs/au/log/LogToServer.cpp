

#include "LogToServer.h"  // Own interface
#include "au/log/LogCentral.h"

namespace au {
// Common log connection
au::LogCentral *log_central = NULL;


void add_log_plugin(LogPlugin *plugin) {
  if (!log_central) {
    fprintf(stderr, "No log_central initialised.\nPlease, init log system before adding plugins\n");
    return;
  }
  log_central->addPlugin(plugin);
}

void remove_log_plugin(LogPlugin *plugin) {
  if (!log_central) {
    fprintf(stderr, "No log_central initialised.\nPlease, init log system before removing plugins\n");
    return;
  }
  log_central->removePlugin(plugin);
}

void start_log_to_server(std::string log_host, int log_port, std::string local_log_file) {
  if (log_central) {
    LM_LW(("Please init log system once."));
    return;
  }

  // Local verbose trace to inform about this start...
  LM_LV(("Start log mechanism with host %s:%d ( local file %s )",
         log_host.c_str(),
         log_port,
         local_log_file.c_str()));

  // Create the permanent connection ( reconnect if necessary )
  log_central = new LogCentral(log_host, log_port, local_log_file);

  // Set this function as the hook function of the log library
  lmOutHookSet(logToLogServer, NULL);
}

void restart_log_to_server(std::string local_log_file) {
  // Only caled in single-thread ( after fork )
  if (log_central) {
    // Keep interesting information
    std::string log_host = log_central->getHost();
    int log_port = log_central->getPort();

    // Delete the old log central
    delete log_central;

    // Create  a new log central element
    log_central = new LogCentral(log_host, log_port, local_log_file);
  }
}

void set_log_direct_mode(bool flag) {
  if (log_central)
    log_central->set_direct_mode(flag);
}

int get_log_fd() {
  if (log_central)
    return log_central->getFd(); return -1;
}

void stop_log_to_server() {
  if (log_central) {
    delete log_central;
    log_central = NULL;
  }
}

void set_log_server(std::string log_host, int log_port) {
  if (!log_central) {
    fprintf(stderr, "Init log system before setting server name \n");
    return;
  }

  log_central->set_host_and_port(log_host, log_port);
}

/* ****************************************************************************
 *
 * logToLogServer -
 */

void logToLogServer(void *vP, char *text, char type, time_t secondsNow, int timezone, int dst, const char *file, int lineNo,
                    const char *fName, int tLev,
                    const char *stre) {
  if (!log_central) {
    fprintf(stderr, "Please start log system before sending traces");
    return;         // Nothing to do if connection is not activated
  }

  // Create the log to be sent
  Log *log = new Log();

  // Add "string" fields
  if (progName)
    log->add_field("progName", progName); if (text)
    log->add_field("text", text); if (file)
    log->add_field("file", file); if (fName)
    log->add_field("fName", fName); if (stre)
    log->add_field("stre", stre);
  log->log_data.lineNo      = lineNo;
  log->log_data.traceLevel  = tLev;
  log->log_data.type        = type;
  log->log_data.timezone    = timezone;
  log->log_data.dst         = dst;
  log->log_data.pid         = getpid();
  // log.log_data.tid         = gettid();
  log->log_data.tid         = 0;       // Not possible to obtain this in mac OS. Pending to be fixed

  // Fill log_data.tv
  gettimeofday(&log->log_data.tv, NULL);

  // Write over the log_server_connection
  log_central->write(log);

  // Release log
  log->Release();
}
}
