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

#include <iostream>     // std::cout
#include <signal.h>
#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy


#include "au/ThreadManager.h"
#include "au/console/Console.h"
#include "au/daemonize.h"
#include "au/log/LogCentralPluginConsole.h"
#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"
#include "au/string/StringUtilities.h"                  // au::str()

#include "logMsg/logMsg.h"
#include "parseArgs/paBuiltin.h"  // paLsHost, paLsPort
#include "parseArgs/paConfig.h"
#include "parseArgs/parseArgs.h"
#include "samson/client/SamsonClient.h"  // samson::SamsonClient
#include "samson/client/SamsonPushBuffer.h"

#include "samson/common/Logs.h"
#include "samson/common/coding.h"       // KVHeader
#include "samson/common/samsonVersion.h"


#include "LogManager.h"
#include "StreamConnector.h"
#include "StreamConnectorService.h"
#include "common.h"

size_t buffer_size;
char input[1024];
char output[1024];
char input_splitter_name[1024];
char file_name[1024];
char working_directory[1024];

bool interactive;
bool run_as_daemon;

static const char *manShortDescription =
  "streamConnector is a easy-to-use network tool used for small processing and data transportation in a SAMSON system.\n"
  "Several inputs and outputs can be specified as follows:\n"
  "\n"
  "------------------------------------------------------------------------------------\n"
  " Inputs\n"
  "------------------------------------------------------------------------------------\n"
  "      stdin                        Data is read from stdin\n"
  "      port:X                       Opens a port waiting for incoming connections that push data to us\n"
  "      connection[:host]:port       We connect to this host and port and read data\n"
  "      samson[:host][:port]:queue   We connect to this SAMSON cluster and pop data from queue\n"
  "\n"
  "------------------------------------------------------------------------------------\n"
  " Outputs\n"
  "------------------------------------------------------------------------------------\n"
  "      stdout                       Data is write to stdout\n"
  "      port:X                       Opens a port waiting for output connections. Data is pushed to all connections\n"
  "      connection:host:port         We connect to this host and port and write data\n"
  "      samson[:host][:port]:queue   We connect to this SAMSON cluster and push data from queue\n"
  "\n"
  "------------------------------------------------------------------------------------\n"
  " Examples:\n"
  "------------------------------------------------------------------------------------\n"
  "\n"
  " streamConnector -input stdin -output samson:samson01:input\n"
  "\n"
  "        Data is read from stdin and pushed to queue 'input' in a SAMSON cluster deployed in server 'samson01'\n"
  "\n"
  " streamConnector -input port:10000 -output \"port:20000 samson:samson01:input\"\n"
  "\n"
  "        Data is read from incoming connections to port 10000.\n"
  "        Data is then pushed to queue 'input' in a SAMSON cluster deployed in server 'samson01' and to any connection to port 20000\n"

;

static const char *manSynopsis =
  "[-input input_description]  [-output output_description]  [-buffer_size size] [-splitter splitter_name]\n";


int sc_console_port;
int sc_web_port;
int default_buffer_size = 64 * 1024 * 1024 - sizeof(samson::KVHeader);
size_t memory;
char log_command[1024];

PaArgument paArgs[] =
{
  { "-input",        input,               "", PaString, PaOpt,
    _i "stdin",
    PaNL,
    PaNL,
    "Input sources "                                                          },
  { "-output",       output,              "", PaString, PaOpt,
    _i "stdout", PaNL, PaNL,
    "Output sources "                                                         },
  { "-buffer_size",  &buffer_size,        "", PaInt,    PaOpt,
    default_buffer_size, 1,
    default_buffer_size, "Buffer size in bytes"                                                    },
  { "-splitter",     input_splitter_name, "", PaString, PaOpt,
    _i "", PaNL, PaNL,
    "Splitter to be used ( only valid for the default channel )"              },
  { "-i",            &interactive,        "", PaBool,   PaOpt,
    false, false, true,
    "Interactive console"                                                     },
  { "-daemon",       &run_as_daemon,      "", PaBool,   PaOpt,
    false, false, true,
    "Run in background. Remove connection & REST interface activated"         },
  { "-console_port", &sc_console_port,    "", PaInt,    PaOpt,
    SC_CONSOLE_PORT, 1, 9999,
    "Port to receive new console connections"                                 },
  { "-web_port",     &sc_web_port,        "", PaInt,    PaOpt,
    SC_WEB_PORT, 1, 9999,
    "Port to receive REST connections"                                        },
  { "-f",            file_name,           "", PaString, PaOpt,
    _i "", PaNL, PaNL,
    "Input file with commands to setup channels and adapters"                 },
  { "-working",      working_directory,   "", PaString, PaOpt,
    _i ".", PaNL, PaNL,
    "Directory to store persistance data if necessary"                        },
  { "-memory",       &memory,             "", PaIntU64, PaOpt,
    1000000000, 1, 10000000000ULL,
    "Port to receive REST connections"                                        },
  { "-log",          log_command,         "", PaString,
    PaOpt, _i "", PaNL,
    PaNL, "log server host"                          },
  PA_END_OF_ARGS
};

// Log fg for traces
int logFd = -1;


// Network connections ( input and output )
stream_connector::StreamConnector *main_stream_connector;


// Instance of the client to connect to SAMSON system
samson::SamsonClient *samson_client;



void captureSIGPIPE(int s) {
  // Nothing
}

int main(int argC, const char *argV[]) {
  paConfig("usage and exit on any warning", (void *)true);

  paConfig("log to screen", (void *)true);
  paConfig("log to file", (void *)true);    // In production it will be false?
  paConfig("screen line format", (void *)"TYPE:EXEC: TEXT");
  paConfig("man shortdescription", (void *)manShortDescription);
  paConfig("man synopsis", (void *)manSynopsis);
  paConfig("log to stderr", (void *)true);
  paConfig("man reportingbugs", SAMSON_BUG_REPORTING);
  paConfig("man author", SAMSON_AUTHORS);
  paConfig("man copyright", SAMSON_COPYRIGHT);
  paConfig("man version", SAMSON_VERSION);

  // Parse input arguments
  paParse(paArgs, argC, (char **)argV, 1, false);
  logFd = lmFirstDiskFileDescriptor();

  // Random initialization
  srand(time(NULL));
  // Init log system
  au::LogCentral::InitLogSystem(argV[0]);
  samson::RegisterLogChannels();   // Add all log channels for samson project ( au,zoo libraries included )

  // Add plugins to report logs to file, server and console
  au::log_central->AddFilePlugin("file", std::string(paLogDir) + "/samsonWorker.log");
  au::log_central->AddFilePlugin("file2", samson::SharedSamsonSetup()->samson_working() + "/samsonWorker.log");

  au::LogCentralPluginConsole *log_plugin_console = new au::LogCentralPluginConsole(NULL, "[type][channel] text", true);
  au::log_central->AddPlugin("console", log_plugin_console);

  if (lmVerbose) {
    au::LogCentral::Shared()->EvalCommand("log_set system V");
  }
  if (lmDebug) {
    au::LogCentral::Shared()->EvalCommand("log_set system D");
  }

  // Additional log-command provided in command line
  au::log_central->EvalCommand(log_command);

  LOG_SV(("Setup memory at engine %s", au::str(memory).c_str()));

  // Capturing SIGPIPE
  if (signal(SIGPIPE, captureSIGPIPE) == SIG_ERR) {
    LOG_SW(("SIGPIPE cannot be handled"));
  }
  if (buffer_size == 0) {
    LM_X(1, ("Wrong buffer size %lu", buffer_size));  // Run in background if required
  }
  if (run_as_daemon) {
    Daemonize();
    Deamonize_close_all();
  }

  // Engine and its associated elements
  engine::Engine::InitEngine(2, memory, 1);

  // Load modules
  au::ErrorManager error;
  au::Singleton<samson::ModulesManager>::shared()->AddModulesFromDefaultDirectory(error);
  if (error.HasErrors()) {
    LOG_SW(("Error loading modules: %s", error.GetLastError().c_str()));
    // Do not stop the process for this error
  }

  // Ignore verbose mode if interactive is activated
  if (interactive) {
    lmVerbose = false;  // Init streamConnector
  }
  main_stream_connector = new stream_connector::StreamConnector();

  if (strcmp(file_name, "") != 0) {
    // Common error messages

    // Read commands from file
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
      LM_E(("Error opening commands file %s", file_name));
      exit(0);
    }

    int num_line = 0;
    char buffer_line[1024];

    std::string message = au::str("Setup file %s. Opening...", file_name);
    main_stream_connector->log("StreamConnector", "Message", message);

    while (fgets(buffer_line, sizeof(buffer_line), file) != NULL) {
      std::string line = ::au::StripString(buffer_line);

      // LOG_SM(("Processing line: %s", line ));
      ++num_line;

      if ((line.length() > 0) && (line[0] != '#')) {
        message = au::str("%s ( File %s )", line.c_str(), file_name);
        main_stream_connector->log("StreamConnector", "Message", message);

        au::ErrorManager error;
        main_stream_connector->process_command(line, &error);

        // Show only if an error happen there
        if (error.HasErrors()) {
          std::cerr << error.str();
        }
      }
    }

    // Print the error on screen
    message = au::str("Setup file %s. Finished", file_name);
    main_stream_connector->log("StreamConnector", "Message", message);

    fclose(file);
  } else {
    // Create default channel
    {
      au::ErrorManager error;
      main_stream_connector->process_command(au::str("add_channel default %s", input_splitter_name), &error);
      if (error.HasErrors()) {
        main_stream_connector->log("Init", "Error", error.GetLastError().c_str());
      }
    }

    size_t adapter_id = 1;

    // Add outputs
    std::vector<std::string> output_components = au::split(output, ' ');
    for (size_t i = 0; i < output_components.size(); i++) {
      std::string name = au::str("adapter_%lu", adapter_id++);
      std::string command = au::str("add_output_adaptor default.%s %s", name.c_str(), output_components[i].c_str());

      au::ErrorManager error;
      main_stream_connector->process_command(command, &error);
      if (error.HasErrors()) {
        main_stream_connector->log("Init", "Error", error.GetLastError().c_str());
      }
    }

    // Add inputs
    std::vector<std::string> input_components = au::split(input, ' ');
    for (size_t i = 0; i < input_components.size(); i++) {
      std::string name = au::str("adapter_%lu", adapter_id++);
      std::string command = au::str("add_input_adaptor default.%s %s", name.c_str(), input_components[i].c_str());

      au::ErrorManager error;
      main_stream_connector->process_command(command, &error);
      if (error.HasErrors()) {
        main_stream_connector->log("Init", "Error", error.GetLastError().c_str());
      }
    }
  }

  // Run console if interactive mode is activated
  if (run_as_daemon) {
    // Add service to accept monitor connections from streamConnectorClient
    main_stream_connector->init_remove_connections_service();
    // Add REST service to accept REST-full connections
    main_stream_connector->init_reset_service();
    // Add service to accept inter-channel connections
    main_stream_connector->init_inter_channel_connections_service();

    while (true) {
      sleep(1000);
    }
  } else if (interactive) {
    // Add service to accept monitor connections from streamConnectorClient
    main_stream_connector->init_remove_connections_service();
    // Add REST service to accept REST-full connections
    main_stream_connector->init_reset_service();
    // Add service to accept inter-channel connections
    main_stream_connector->init_inter_channel_connections_service();

    log_plugin_console->SetConsole(main_stream_connector);
    main_stream_connector->StartConsole();
    log_plugin_console->SetConsole(NULL);
  } else {
    au::Cronometer cronometer_notification;
    while (true) {
      // Close finish items and connections
      au::ErrorManager error;
      main_stream_connector->remove_finished_items_and_connections(&error);
      // std::cerr << error.str();

      size_t num_input_items = main_stream_connector->getNumInputItems();
      size_t pending_size =  main_stream_connector->getOutputConnectionsBufferedSize();

      if (cronometer_notification.seconds() > 1) {
        cronometer_notification.Reset();

        std::ostringstream message;

        size_t memory = engine::Engine::memory_manager()->memory();
        size_t used_memory = engine::Engine::memory_manager()->used_memory();

        message << "Memory " << au::str(used_memory) << "/" << au::str(memory);
        message << "(" << au::str_percentage(used_memory, memory) << ") ";

        LOG_SV(("%s", message.str().c_str()));

        /*
         * if (lmVerbose) {
         * std::cout << engine::Engine::memory_manager()->getTableOfBuffers().str() << std::endl;
         * }
         *
         * if (lmVerbose) {
         * au::tables::Table *table = main_stream_connector->getConnectionsTable();
         * std::cerr << table->str();
         * delete table;
         * }
         *
         * if (lmVerbose) {
         * au::tables::Table *table = main_stream_connector->getConnectionsTable("data");
         * std::cerr << table->str();
         * delete table;
         * }
         */
      }

      // Verify if can exit....
      if (num_input_items == 0) {  // Verify no input source is connected
        if (pending_size == 0) {  // Check no pending data to be send....
          LM_X(0, ("Finish correctly. No more inputs data"));
        }
      }
      usleep(100000);
    }
  }

  // Stop the log system
  au::LogCentral::StopLogSystem();

  return 0;
}

