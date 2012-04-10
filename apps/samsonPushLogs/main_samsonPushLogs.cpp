
/*

 samsonPushLogs

 Example app using samsonClient lib
 It listen std input and push content to the samson cluster indicated by parameters

 AUTHOR: Andreu Urruela

 */


#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <iostream>     // std::cout

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"

#include "logMsg/logMsg.h"

#include "au/string.h"              // au::str()

#include "samson/client/SamsonClient.h"         // samson::SamsonClient
#include "samson/client/SamsonPushBuffer.h"

#include "samson/common/coding.h"               // KVHeader

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
int port;
char*          dir_list[32];
char*          file_list[32];

static const char* manShortDescription = 
    "samsonPushLogs is a easy-to-use client to send data to a SAMSON system. It emulates real_time streaming from any number of datasets, stored in local directories\n";

static const char* manSynopsis =
    "[-help] [-node str_samson_node] [-port_node int_port] [-timeout int_t] [-buffer_size int_size] [-breaker_sequence str_pattern] [-lines bool] dir,extension,timepos,timetype,queue\n";

int default_buffer_size = 64*1024*1024 - sizeof(samson::KVHeader);

PaArgument paArgs[] =
{   
    { "-node",        controller,            "",    PaString,  PaOpt, _i "localhost"  , PaNL, PaNL,       "SAMSON node to connect with "         },
    { "-port_node",    &port_node,           "",                       PaInt,    PaOpt, SAMSON_WORKER_PORT,  1,    99999, "SAMSON server port"                     },
    { "-user",             user,                  "",       PaString, PaOpt,  _i "anonymous", PaNL, PaNL, "User to connect to SAMSON cluster"  },
    { "-password",         password,              "",       PaString, PaOpt,  _i "anonymous", PaNL, PaNL, "Password to connect to SAMSON cluster"  },
    { "-buffer_size",      &buffer_size,          "",  PaInt,     PaOpt,       default_buffer_size,         1,   default_buffer_size,  "Buffer size in bytes"    },
    { "-mr",               &max_rate,             "",  PaInt,     PaOpt,       10000000,      100,  100000000,  "Max rate in bytes/s"                            },
    { "-nr",               &ntimes_real_time,     "",  PaFloat,     PaOpt,       1.0,      0.01,  10000.0,  "Number of time real time"                            },
    { "-breaker_sequence", breaker_sequence, "",  PaString,  PaOpt,        _i "\n",   PaNL,         PaNL,  "Breaker sequence ( by default \\n )"            },
    { "-lines",            &lines,                "",  PaBool,    PaOpt,          false,  false,         true,  "Read std-in line by line"                       },
    { "-memory",           &push_memory,          "",  PaInt,     PaOpt,           1000,      1,      1000000,  "Memory in Mb used to push data ( default 1000)" },
    { "-port",             &port,                 "",  PaInt,     PaOpt,              0,      0,        99999,  "Port to listen from" },
    { "-dl",               dir_list,            "",  PaSList,  PaOpt, PaND,     PaNL,    PaNL,    "string list with all the datasets directories to be pushed, format: dir,extension,time_pos,time_type,queue_name"         },
    { "-fl",               file_list,            "",  PaSList,  PaOpt, PaND,     PaNL,    PaNL,    "string list with all the datasets files to be pushed, format: file,time_pos,time_type,queue_name"         },
    PA_END_OF_ARGS
};

int logFd = -1;

// Instance of the client to connect to SAMSON system
samson::SamsonClient *samson_client;


int main( int argC , const char *argV[] )
{
  paConfig("usage and exit on any warning", (void*) true);

  paConfig("log to screen",                 (void*) true);
  paConfig("log to file",                   (void*) false);
  paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
  paConfig("man shortdescription",          (void*) manShortDescription);
  paConfig("man synopsis",          (void*) manSynopsis);

  // Parse input arguments
  paParse(paArgs, argC, (char**) argV, 1, false);
  logFd = lmFirstDiskFileDescriptor();

  // Random initialization
  srand( time(NULL) );



  if( buffer_size == 0)
    LM_X(1,("Wrong buffer size %lu", buffer_size ));

  // Check queue is specified
  if (dir_list[0] != NULL)
    LM_X(1,("Please, specify at least a dataset and a queue to push data to"));

  // Create samson client
  size_t total_memory = push_memory*1024*1024;
  LM_V(("Setting memory for samson client %s" , au::str(total_memory,"B").c_str() ));
  samson::SamsonClient::general_init( total_memory );
  samson_client = new samson::SamsonClient("push");


  LM_V(("Connecting to %s ..." , controller));

  // Init connection
  if( !samson_client->initConnection( controller , port_node , user , password ) )
  {
    fprintf(stderr, "Error connecting with samson cluster: %s, port:%d\n" , samson_client->getErrorMessage().c_str(), port_node );
    exit(0);
  }
  LM_V(("Conection to %s OK" , controller));

  SamsonPushLogsConnectionsManager manager;


  std::vector<LogsDataSet *>datasets_vector;

  if (dir_list[0] != NULL)
  {
    int ix;

    printf("got %ld items in string-list\n", (long) dir_list[0]);

    for (ix = 1; ix <= (long long) dir_list[0]; ix++)
    {
      std::vector<char*> fields;
      char separator = ',';

      SplitInWords(dir_list[ix] , fields , separator);

      int timestamp_position = atoi(fields[2]);
      int timestamp_type = atoi(fields[3]);

      LogsDataSet *dataset = new LogsDataSet(fields[0], fields[1], timestamp_position, timestamp_type, fields[4]);
      datasets_vector.push_back(dataset);
    }
  }

  time_t first_timestamp = static_cast<time_t>(0x0ffffffffffffffff);
  for (unsigned int ix = 0; (ix < datasets_vector.size()); ix++)
  {
    time_t dataset_timestamp = 0;
    if ((dataset_timestamp = datasets_vector[ix]->GetFirstTimestamp()) < first_timestamp)
    {
      first_timestamp = dataset_timestamp;
    }

  }

  for (unsigned int ix = 0; (ix < datasets_vector.size()); ix++)
  {
    datasets_vector[ix]->SetFirstTimestamp(first_timestamp);
  }

  for (unsigned int ix = 0; (ix < datasets_vector.size()); ix++)
  {
    SamsonPushLogsConnection *connection = new SamsonPushLogsConnection(datasets_vector[ix], datasets_vector[ix]->GetQueueName(), ntimes_real_time, samson_client);
    manager.NewLogDataset(connection);
  }



  while( true )
  {
    sleep(5);
    manager.ReviewDatasets();
    LM_M(("samsonPush listening from port %d with %lu active datasets" , port ,  manager.GetNumDataSets() ));

  }

  // Wait until all operations are complete
  LM_V(("Waiting for all the push operations to complete..."));
  samson_client->waitUntilFinish();




}
