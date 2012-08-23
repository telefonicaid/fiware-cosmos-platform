
/*
 *
 * samsonPushLogs
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

#include "au/string.h"                  // au::str()

#include "samson/client/SamsonClient.h"  // samson::SamsonClient
#include "samson/client/SamsonPushBuffer.h"

#include "samson/common/coding.h"       // KVHeader

#include "au/network/NetworkListener.h"

#include "LogsDataSet.h"
#include "SamsonPushLogsConnectionsManager.h"

size_t buffer_size;
char user[1024];
char password[1024];
char breaker_sequence[1024];
char controller[1024];
int port_node;
char queue_name[1024];
bool lines;                         // Flag to indicate that input is read line by line
int push_memory;                    // Global memory used as a bffer
int max_rate;                       // Max rate
float ntimes_real_time;             // Multiplicative factor respect real time
char *initial_timestamp_commandline_str[32];
int port;
char *dir_list[32];
char *file_list[32];

static const char *manShortDescription =
  "samsonPushLogs is a easy-to-use client to send data to a SAMSON system. It emulates real_time streaming from any number of datasets, stored in local directories\n";

static const char *manSynopsis =
  "[-help] [-node str_samson_node] [-port_node int_port] [-timeout int_t] [-buffer_size int_size] [-breaker_sequence str_pattern] [-lines bool] dir,extension,timepos,timetype,queue\n";

int default_buffer_size = 64 * 1024 * 1024 - sizeof(samson::KVHeader);

PaArgument paArgs[] =
{
  { "-node",             controller,                                      "",
    PaString,
    PaOpt,
    _i "localhost",            PaNL,      PaNL,                  "SAMSON node to connect with "                                                                                                       },
  { "-port_node",        &port_node,                                      "",
    PaInt,                                               PaOpt,
    SAMSON_WORKER_PORT,        1,         99999,                 "SAMSON server port"                                                                                                                 },
  { "-user",             user,                                            "",
    PaString,                                            PaOpt,
    _i "anonymous",            PaNL,      PaNL,                  "User to connect to SAMSON cluster"                                                                                                  },
  { "-password",         password,                                        "",
    PaString,                                            PaOpt,
    _i "anonymous",            PaNL,      PaNL,                  "Password to connect to SAMSON cluster"                                                                                              },
  { "-buffer_size",      &buffer_size,                                    "",
    PaInt,                                               PaOpt,
    default_buffer_size,       1,         default_buffer_size,   "Buffer size in bytes"                                                                                                               },
  { "-mr",               &max_rate,                                       "",
    PaInt,                                               PaOpt,
    10000000,                  100,       100000000,             "Max rate in bytes/s"                                                                                                                },
  { "-nr",               &ntimes_real_time,                               "",
    PaFloat,                                             PaOpt,                       1.0,
    0.01,      10000.0,               "Number of time real time"                                                                                                           },
  { "-ti",               initial_timestamp_commandline_str,               "",
    PaSList,                                             PaOpt,
    PaND,                      PaNL,      PaNL,
    "Initial timestamp (by default, first timestamp in input data"                                                                       },
  { "-breaker_sequence", breaker_sequence,                                "",
    PaString,                                            PaOpt,
    _i "\n",                   PaNL,      PaNL,                  "Breaker sequence ( by default \\n )"                                                                                                },
  { "-lines",            &lines,                                          "",
    PaBool,                                              PaOpt,
    false,                     false,     true,                  "Read std-in line by line"                                                                                                           },
  { "-memory",           &push_memory,                                    "",
    PaInt,                                               PaOpt,
    1000,                      1,         1000000,               "Memory in Mb used to push data ( default 1000)"                                                                                     },
  { "-port",             &port,                                           "",
    PaInt,                                               PaOpt,                       0,
    0,         99999,                 "Port to listen from"                                                                                                                },
  { "-dl",               dir_list,                                        "",
    PaSList,                                             PaOpt,
    PaND,                      PaNL,      PaNL,
    "string list with all the datasets directories to be pushed, format: dir,extension,nfileds,time_pos,time_type,queue_name"            },
  { "-fl",               file_list,                                       "",
    PaSList,                                             PaOpt,
    PaND,                      PaNL,      PaNL,
    "string list with all the datasets files to be pushed, format: file,nfields,time_pos,time_type,queue_name"                           },
  PA_END_OF_ARGS
};

int logFd = -1;

// Instance of the client to connect to SAMSON system
samson::SamsonClient *samson_client;


int main(int argC, const char *argV[]) {
  paConfig("usage and exit on any warning", (void *)true);

  paConfig("log to screen",                 (void *)true);
  paConfig("log to file",                   (void *)true);
  paConfig("screen line format",            (void *)"TYPE:EXEC: TEXT");
  paConfig("log file line format",              (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("man shortdescription",          (void *)manShortDescription);
  paConfig("man synopsis",          (void *)manSynopsis);

  // Parse input arguments
  paParse(paArgs, argC, (char **)argV, 1, false);
  logFd = lmFirstDiskFileDescriptor();

  // Random initialization
  struct timeval tp;
  gettimeofday(&tp, NULL);
  int rand_seq = tp.tv_sec * 1000 + tp.tv_usec % 1000;
  srand(rand_seq);




  if (buffer_size == 0) {
    LM_X(1, ("Wrong buffer size %lu", buffer_size ));  // Check queue is specified
  }
  if (dir_list[0] == NULL) {
    LM_X(1, ("Please, specify at least a dataset and a queue to push data to"));  // Create samson client
  }
  size_t total_memory = push_memory * 1024 * 1024;
  LM_V(("Setting memory for samson client %s", au::str(total_memory, "B").c_str()));
  samson::SamsonClient::general_init(total_memory);

  LM_V(("Connecting to %s ...", controller));

  samson_client = new samson::SamsonClient("push");
  if (!samson_client->connect(controller)) {
    LM_X(1, ("Not possible to connect with %s", controller ));
  }
  SamsonPushLogsConnectionsManager manager;


  std::vector<LogsDataSet *>datasets_vector;

  if (dir_list[0] != NULL) {
    int ix;

    printf("got %ld items in string-list for datasets\n", (long)dir_list[0]);

    for (ix = 1; ix <= (long long)dir_list[0]; ix++) {
      std::vector<char *> fields;
      char separator = ',';

      SplitInWords(dir_list[ix], fields, separator);

      if (fields.size() != 6) {
        LM_E((
               "Wrong input dataset format:'%s'. Expected format for input datasets: dir_path,extension,nfields,time_pos,time_type,queue_name",
               dir_list[ix]));
        exit(-1);
      }


      int timestamp_position = 0;
      int timestamp_position_alt = 0;
      char *p_sep;

      int num_fields = atoi(fields[2]);
      if ((p_sep = strchr(fields[3], ':')) != NULL) {
        *p_sep = '\0';
        timestamp_position = atoi(fields[3]);
        timestamp_position_alt = atoi(p_sep + 1);
      } else {
        timestamp_position = atoi(fields[3]);
        timestamp_position_alt = atoi(fields[3]);
      }
      int timestamp_type = atoi(fields[4]);

      LogsDataSet *dataset =
        new LogsDataSet(fields[0], fields[1], num_fields, timestamp_position, timestamp_position_alt, timestamp_type,
                        fields[5]);
      if (dataset->InitDir() == true) {
        datasets_vector.push_back(dataset);
      }
    }
  }

  time_t first_timestamp;
  if (initial_timestamp_commandline_str[0] != NULL) {
    int ix;

    printf("got %ld items in string-list for initial timestamp\n", (long)initial_timestamp_commandline_str[0]);

    char initial_timestamp_str[1024];

    initial_timestamp_str[0] = '\0';
    for (ix = 1; ix <= (long long)initial_timestamp_commandline_str[0]; ix++) {
      strcat(initial_timestamp_str, initial_timestamp_commandline_str[ix]);
      strcat(initial_timestamp_str, " ");
    }
    first_timestamp = GetTimeFromStrTimeDate_YYYY_mm_dd_24H(initial_timestamp_str);
    LM_M(("From command line, initial_time_stamp:%s, %lu = '%s'", initial_timestamp_str, first_timestamp,
          ctimeUTC(&first_timestamp)));
  } else {
    LM_M(("Reading first_timestamp from datasets"));
    first_timestamp = time(NULL);

    for (unsigned int ix = 0; (ix < datasets_vector.size()); ix++) {
      time_t dataset_timestamp = 0;
      char *log_line;
      if (datasets_vector[ix]->LookAtNextLogLineEntry(&log_line, &dataset_timestamp) == true) {
        LM_M(("For dataset:%d, dataset_timestamp:%s", ix, ctimeUTC(&dataset_timestamp)));
        LM_M(("For dataset:%d, first_timestamp:%s", ix, ctimeUTC(&first_timestamp)));
        free(log_line);
        if (dataset_timestamp < first_timestamp) {
          LM_M(("Updating dataset_timestamp:%s", ctimeUTC(&dataset_timestamp)));
          LM_M(("Updating first_timestamp:%s", ctimeUTC(&first_timestamp)));
          first_timestamp = dataset_timestamp;
        }
      }
    }
  }

  char *time_init_str = strdup(ctimeUTC(&first_timestamp));
  time_init_str[strlen(time_init_str) - 1] = '\0';
  LM_M(("Final first_timestamp: %s", time_init_str));

  for (unsigned int ix = 0; (ix < datasets_vector.size()); ix++) {
    datasets_vector[ix]->SetFirstTimestamp(first_timestamp);

    time_t dataset_first_timestamp = datasets_vector[ix]->GetFirstTimestamp();
    char *dataset_first_timestamp_str = strdup(ctimeUTC(&dataset_first_timestamp));
    dataset_first_timestamp_str[strlen(dataset_first_timestamp_str) - 1] = '\0';

    LM_M(("Checking %s with first_timestamp:%s, read %s", datasets_vector[ix]->GetQueueName(), time_init_str,
          dataset_first_timestamp_str));
    free(dataset_first_timestamp_str);

    datasets_vector[ix]->Synchronize(first_timestamp);
  }

  for (unsigned int ix = 0; (ix < datasets_vector.size()); ix++) {
    SamsonPushLogsConnection *connection = new SamsonPushLogsConnection(datasets_vector[ix],
                                                                        datasets_vector[ix]->GetQueueName(),
                                                                        ntimes_real_time,
                                                                        samson_client);
    manager.NewLogDataset(connection);
  }

  while (true) {
    sleep(100);
    manager.ReviewDatasets();
    LM_M(("samsonPushLogs reading and waiting as times go by with %lu  datasets and %lu active", datasets_vector.size(),
          manager.GetNumDataSets()));

    if (manager.GetNumDataSets() == 0) {
      LM_M(("Read all datasets"));
      break;
    }
  }

  // Wait until all operations are complete
  LM_V(("Waiting for all the push operations to complete..."));
  samson_client->waitFinishPushingData();
}

