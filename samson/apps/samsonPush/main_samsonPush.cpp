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

/*
 *
 * samsonPush
 *
 * Example app using samsonClient lib
 * It listen std input and push content to the samson cluster indicated by parameters
 *
 * AUTHOR: Andreu Urruela
 *
 */


#include <iostream>     // std::cout
#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy

#include "parseArgs/paConfig.h"
#include "parseArgs/parseArgs.h"

#include "logMsg/logMsg.h"

#include "au/network/NetworkListener.h"
#include "au/string/StringUtilities.h"                  // au::str()

#include "samson/client/SamsonClient.h"  // samson::SamsonClient
#include "samson/client/SamsonPushBuffer.h"
#include "samson/common/coding.h"       // KVHeader
#include "samson/common/samsonVersion.h"

#include "SamsonPushConnectionsManager.h"

size_t buffer_size;
char user[1024];
char password[1024];
char breaker_sequence[1024];
int port_node;
char queue_name[1024];
char host[1024];
char cluster_id[1024];
bool lines;                         // Flag to indicate that input is read line by line
int push_memory;                    // Global memory used as a bffer
int max_rate;                       // Max rate
int port;

static const char *manShortDescription =
  "samsonPush is a easy-to-use client to send data to a particular queue in a "
  "SAMSON system. Just push data into the standard input\n";

static const char *manSynopsis =
  "[-help] [-node str_samson_node] [-port_node int_port] [-timeout int_t] [-buffer_size int_size] [-breaker_sequence str_pattern] [-lines bool] queue\n";

int default_buffer_size = 64 * 1024 * 1024 - sizeof(samson::KVHeader);

PaArgument paArgs[] =
{
  { "-node",             host,             "", PaString, PaOpt,
    _i "localhost",
    PaNL,
    PaNL, "SAMSON worker node"                              },
  { "-user",             user,             "", PaString, PaOpt,
    _i "anonymous", PaNL,
    PaNL, "User to connect to SAMSON cluster"               },
  { "-password",         password,         "", PaString, PaOpt,
    _i "anonymous", PaNL,
    PaNL, "Password to connect to SAMSON cluster"           },
  { "-buffer_size",      &buffer_size,     "", PaInt,    PaOpt,
    default_buffer_size, 1,
    default_buffer_size, "Buffer size in bytes"                            },
  { "-mr",               &max_rate,        "", PaInt,    PaOpt,
    10000000, 100,
    100000000, "Max rate in bytes/s"                             },
  { "-breaker_sequence", breaker_sequence, "", PaString, PaOpt,
    _i "\n", PaNL,
    PaNL, "Breaker sequence ( by default \\n )"             },
  { "-lines",            &lines,           "", PaBool,   PaOpt,
    false, false,
    true, "Read std-in line by line"                        },
  { "-memory",           &push_memory,     "", PaInt,    PaOpt,
    1000, 1,
    1000000, "Memory in Mb used to push data ( default 1000)"  },
  { "-port",             &port,            "", PaInt,    PaOpt,
    0, 0,
    99999, "Port to listen from"                             },
  { " ",                 queue_name,       "", PaString, PaReq,
    _i "null", PaNL,
    PaNL, "name of the queue to push data"                  },
  PA_END_OF_ARGS
};

int logFd = -1;

void FindAndReplaceInString(std::string &source, const std::string find, std::string replace) {
  size_t j;

  for (; (j = source.find(find)) != std::string::npos; ) {
    source.replace(j, find.length(), replace);
  }
}

void literal_string(std::string& txt) {
  std::string slash = "\n";
  std::string replace_slash = "\\n";

  FindAndReplaceInString(txt, slash, replace_slash);
}

size_t full_read(int fd, char *data, size_t size) {
  size_t read_size = 0;

  while (read_size < size) {
    ssize_t t = read(fd, data + read_size, size - read_size);

    if (t == -1) {
      LM_X(1, ("Error reading input data"));
    }
    if (t == 0) {
      break;
    } else {
      read_size += t;
    }
  }

  return read_size;
}

void receive_data_from_port() {
  LM_V(("Opening port %d", port));

  SamsonPushConnectionsManager manager;
  au::NetworkListener listener(&manager);
  listener.InitNetworkListener(port);

  while (true) {
    sleep(5);
    manager.review_connections();
    LOG_SM(("samsonPush listening from port %d with %lu active connections", port, manager.getNumConnections()));
  }
}

// Instance of the client to connect to SAMSON system
samson::SamsonClient *samson_client;


int main(int argC, const char *argV[]) {
  paConfig("usage and exit on any warning", (void *)true);

  paConfig("log to screen", (void *)true);
  paConfig("log to file", (void *)true);
  paConfig("screen line format", (void *)"TYPE:EXEC: TEXT");
  paConfig("man shortdescription", (void *)manShortDescription);
  paConfig("man synopsis", (void *)manSynopsis);
  paConfig("man reportingbugs", SAMSON_BUG_REPORTING);
  paConfig("man author", SAMSON_AUTHORS);
  paConfig("man copyright", SAMSON_COPYRIGHT);
  paConfig("man version", SAMSON_VERSION);

  // Parse input arguments
  paParse(paArgs, argC, (char **)argV, 1, false);
  logFd = lmFirstDiskFileDescriptor();

  // Random initialization
  struct timeval tp;
  gettimeofday(&tp, NULL);
  int rand_seq = tp.tv_sec * 1000 + tp.tv_usec % 1000;
  srand(rand_seq);

  // Init log system
  au::LogCentral::InitLogSystem(argV[0]);
  samson::RegisterLogChannels();   // Add all log channels for samson project ( au,zoo libraries included )
  au::log_central->AddScreenPlugin("screen", "[type] text");  // Temporal plugin

  if (buffer_size == 0) {
    LM_X(1, ("Wrong buffer size %lu", buffer_size));  // Check queue is specified
  }
  if (strcmp(queue_name, "null") == 0) {
    LM_X(1, ("Please, specify a queue to push data to"));  // Create samson client
  }
  size_t total_memory = push_memory * 1024 * 1024;
  LM_V(("Setting memory for samson client %s", au::str(total_memory, "B").c_str()));
  samson::SamsonClient::general_init(total_memory);
  samson_client = new samson::SamsonClient("push");

  if (!samson_client->Connect(host)) {
    LM_X(1, ("Unable to connect to %s", host));
  }
  LM_V(("Trying to connect to %s", host));
  while (true) {
    if (samson_client->connection_ready()) {
      break;
    } else {
      usleep(100000);
    }
  }
  LM_V(("Connected to %s", host));

  if (port != 0) {
    receive_data_from_port();
    LM_V(("Exit after listenning from port %d", port));
    exit(0);
  }


  // Create the push buffer to send data to a queue in buffer-mode
  samson::SamsonPushBuffer *pushBuffer = new samson::SamsonPushBuffer(samson_client, queue_name);


  // Read data in blocks, lock the separator backward
  // --------------------------------------------------------------------------------

  char *data = (char *)malloc(buffer_size);
  if (!data) {
    LM_X(1, ("Error allocating %lu bytes", buffer_size));
  }
  size_t size = 0;                                                                           // Bytes currently contained in the buffer

  std::string tmp_separator = breaker_sequence;
  literal_string(tmp_separator);

  LM_V(("Input parameter buffer_size %s", au::str(buffer_size).c_str()));
  LM_V(("Input parameter break_sequence '%s' ( length %d ) ", tmp_separator.c_str(), strlen(breaker_sequence)));

  // Statistics about stdin rate ( it also controls max rate )
  au::Rate rate_stdin;

  while (true) {
    // --------------------------------------------------------------------
    // Read content from stdin
    // --------------------------------------------------------------------

    size_t read_bytes = 0;

    if (lines) {
      char *string = fgets(data, buffer_size, stdin);

      if (string) {
        read_bytes = strlen(string);
      } else {
        read_bytes = 0;
      }
    } else {
      read_bytes = full_read(0, data + size, buffer_size - size);
    }

    // Statistics about input size
    rate_stdin.Push(read_bytes);

    // Information about current status....
    size_t memory = engine::Engine::memory_manager()->memory();
    size_t used_memory = engine::Engine::memory_manager()->used_memory();
    double memory_usage = engine::Engine::memory_manager()->memory_usage();

    if (read_bytes == 0) {
      break;
    }
    LM_V(("Stdin info: %s", rate_stdin.str().c_str()));

    if (memory_usage > 0.8) {
      LM_V(("Memory used %s / %s ( %s )",
            au::str(used_memory, "B").c_str(),
            au::str(memory, "B").c_str(),
            au::str_percentage(memory_usage).c_str()
            ));  // Increase the size of data contained in the buffer
    }
    size += read_bytes;

    // Processing data contained in "data" with size "size"
    // -----------------------------------------------------------------

    // Find backward a particular string...
    const char *last_pos = au::laststrstr(data, size, breaker_sequence);

    if (!last_pos) {
      LM_X(1, ("Not found breaker sequence '%s' in a buffer of %lu bytes", breaker_sequence, size));
    }
    size_t output_size = last_pos - data + strlen(breaker_sequence);

    // LOG_SM(("Processing buffer with %s --> %s block push to queue" , au::str(size).c_str() , au::str(output_size).c_str() ));

    // Emit this block of data ( always flushing to the platform )
    LM_V(("Pushing a buffer of %s bytes to our push buffer", au::str(output_size).c_str()));
    pushBuffer->push(data, output_size, false);

    // -----------------------------------------------------------------

    // Sleep if necessary
    if (rate_stdin.rate() > (double)max_rate) {
      LM_V(("Stdin info: %s", rate_stdin.str().c_str()));
      LM_V(("Sleeping respect max rate %s", au::str(max_rate).c_str()));
      sleep(1);
    }

    // Move rest of data to the beginning of the buffer
    if (output_size < size) {
      memmove(data, data + output_size, size - output_size);
      size = size - output_size;
    } else {
      size = 0;
    }
  }

  // Last push

  LM_V(("Flushing push buffer"));
  pushBuffer->flush();

  LM_V(("--------------------------------------------------------------------------------"));
  LM_V(("Stdin info:        %s", rate_stdin.str().c_str()));
  LM_V(("PushBuffer info:   %s", pushBuffer->rate_.str().c_str()));
  LM_V(("SamsonClient info: %s", samson_client->push_rate().str().c_str()));
  LM_V(("--------------------------------------------------------------------------------"));


  // Wait until all operations are complete
  LM_V(("Waiting for all the push operations to complete..."));
  samson_client->waitFinishPushingData();
}

