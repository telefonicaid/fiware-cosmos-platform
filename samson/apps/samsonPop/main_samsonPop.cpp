
/*
 *
 * samsonPop
 *
 * Example app using samsonClient lib
 * It connect to a SAMSON system to download contents from a queue
 *
 *
 * AUTHOR: Andreu Urruela
 *
 */


#include <iostream>     // std::cout
#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy

#include "logMsg/logMsg.h"
#include "parseArgs/parseArgs.h"

#include "au/string/StringUtilities.h"                  // au::str()

#include "samson/client/SamsonClient.h"  // samson::SamsonClient


size_t buffer_size;
size_t timeOut;

char host[1024];

int port_node;
char queue_name[1024];
char format[81];
char user[1024];
char password[1024];
bool show_header;
bool flag_new;
bool flag_remove;
int limit;
int max_size;  // Max size downloading in bytes ( just for testing )

PaArgument paArgs[] =
{
  { "-node",      host,                "",                PaString,                PaOpt,
    _i "localhost",
    PaNL,
    PaNL,
    "SAMSON Worker node"                                },
  { "-port_node", &port_node,          "",                PaInt,                   PaOpt,
    SAMSON_WORKER_PORT,            1,                 99999,
    "SAMSON server port"                                },
  { "-user",      user,                "",                PaString,                PaOpt,
    _i "anonymous",
    PaNL,              PaNL,
    "User to connect to SAMSON cluster"           },
  { "-password",  password,            "",                PaString,                PaOpt,
    _i "anonymous",
    PaNL,              PaNL,
    "Password to connect to SAMSON cluster"       },
  { "-header",    &show_header,        "",                PaBool,                  PaOpt,                  false,
    false,             true,
    "Show only header of blocks"                  },
  { "-remove",    &flag_remove,        "",                PaBool,                  PaOpt,                  false,
    false,             true,
    "Remove downloaded stuff"                     },
  { "-new",       &flag_new,           "",                PaBool,                  PaOpt,                  false,
    false,             true,               "Get only new data"                             },
  { "-limit",     &limit,              "",                PaInt,                   PaOpt,                  0,
    0,                 10000,
    "number of kvs to be shown for each block"    },
  { "-format",    format,              "",                PaString,                PaOpt,                  _i "plain",
    PaNL,              PaNL,
    "type of output format: [plain|json|xml]"     },
  { "-max_size",  &max_size,           "",                PaInt,                   PaOpt,                  0,
    0,                 100000000,          "Max size to download"                          },
  { " ",          queue_name,          "",                PaString,                PaReq,                  (long)"null",
    PaNL,              PaNL,
    "name of the queue to pop data from"          },
  PA_END_OF_ARGS
};

static const char *manSynopsis =
  "[-help] [-node str_samson_node] [-port_node int_port] [-header bool] [-limit int_n] [-format plain|json|xml] queue\n";

static const char *manShortDescription =
  "samsonPop is an easy-to-use client to receive data from a particular queue in a SAMSON system.\n";


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

int main(int argC, const char *argV[]) {
  paConfig("usage and exit on any warning", (void *)true);

  paConfig("log to screen",         (void *)true);
  paConfig("log to file",           (void *)true);
  paConfig("screen line format",    (void *)"TYPE:EXEC: TEXT");
  paConfig("man shortdescription",  (void *)manShortDescription);
  paConfig("man synopsis",          (void *)manSynopsis);
  paConfig("log to stderr",         (void *)true);


  // Parse input arguments
  paParse(paArgs, argC, (char **)argV, 1, false);

  if ((strcmp(format, "plain") != 0) && (strcmp(format, "json") != 0) && (strcmp(format, "xml") != 0)) {
    fprintf(stderr, "Format '%s' not supported. Please select \"plain\", \"json\" or \"xml\"\n", format);
    exit(-1);
  }
  logFd = lmFirstDiskFileDescriptor();

  // Random initialization
  struct timeval tp;
  gettimeofday(&tp, NULL);
  int rand_seq = tp.tv_sec * 1000 + tp.tv_usec % 1000;
  srand(rand_seq);

  // Set 1G RAM for uploading content
  samson::SamsonClient::general_init(1024 * 1024 * 1024);

  // Instance of the client to connect to SAMSON system
  samson::SamsonClient client("pop");

  if (!client.connect(host)) {
    LM_X(1, ("Not possible to connect with %s", host ));  // Init connection
  }
  au::ErrorManager error;
  if (error.IsActivated()) {
    fprintf(stderr, "Error connecting with samson cluster: %s\n", error.GetMessage().c_str());
    exit(0);
  }


  // Connect to a particular queue
  LM_V(("Connecting to queue %s", queue_name ));
  client.connect_to_queue(queue_name, flag_new, flag_remove);
  LM_V(("Connected to queue %s", queue_name ));

  size_t downloaded_content = 0;

  while (true) {
    samson::SamsonClientBlockInterface *block = client.getNextBlock(queue_name);
    if (block) {
      if (show_header) {
        std::cout << block->get_header_content();
      } else {
        std::string content = block->get_content(limit, format);
        std::cout << content;
        downloaded_content += content.length();
      }

      // Flush output
      std::cout.flush();

      delete block;

      if (max_size > 0) {
        if (downloaded_content >= (size_t)max_size) {
          LM_V(("Exit infinite loop because downloaded_content(%d) >= max_size(%d)", downloaded_content, max_size));
          break;
        }
      }
    } else {
      usleep(100000);
    }
  }

  // Wait until all operations are complete
  LM_V(("Waiting for client to finish..."));
  client.waitUntilFinish();
  LM_V(("Client finished"));
}

