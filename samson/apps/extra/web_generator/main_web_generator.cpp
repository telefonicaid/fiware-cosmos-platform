

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "parseArgs/paConfig.h"
#include "parseArgs/parseArgs.h"

#include "logMsg/logMsg.h"

#include "au/CommandLine.h"  // au::CommandLine
#include "au/Cronometer.h"  // au::Cronometer
#include "au/string.h"  // au::str()

#include "au/CommandLine.h"     // au::CommandLine


static const char *manShortDescription =
  "web_generator is a simple tool to generate radom sequences of web navigation logs\n";

static const char *manSynopsis =   "  [-rate XX]\n";
int rate;

PaArgument paArgs[] =
{
  { "-rate", &rate, "", PaInt,        PaOpt,              10000,  1,     100000000,     "Rate in events per second"          },
  PA_END_OF_ARGS
};

int logFd = -1;

au::Cronometer cronometer;


void getIp(char *txt) {
  sprintf(txt, "%d.%d.%d.%d", 10, 95, rand() % 256, rand() % 256);
}

int main(int argC, const char *argV[]) {
  paConfig("usage and exit on any warning", (void *)true);

  paConfig("log to screen",                 (void *)true);
  paConfig("log to file",                   (void *)false);
  paConfig("screen line format",            (void *)"TYPE:EXEC: TEXT");
  paConfig("man shortdescription",          (void *)manShortDescription);
  paConfig("man synopsis",                  (void *)manSynopsis);
  paConfig("log to stderr",                 (void *)true);

  // Parse input arguments
  paParse(paArgs, argC, (char **)argV, 1, false);
  logFd = lmFirstDiskFileDescriptor();


  char ip_from[1024];
  char ip_to[1024];

  size_t counter = 0;
  while (true) {
    size_t time = cronometer.seconds();

    if (time == 0)
      if (counter >= (size_t)rate) {
        LM_V(("Sleeping after generating %lu samples", counter));
        sleep(1);
      }
    size_t t_time = counter / (size_t)rate;

    if (t_time > time) {
      LM_V(("Sleeping to generate an ouput rate of %d samples/s", rate ));
      sleep(1);
    }



    const char *protocol;
    if (rand() % 10 > 5)
      protocol = "HTTP/1.0"; else
      protocol = "HTTP/1.1"; const char *operation;
    if (rand() % 10 > 5)
      operation = "GET"; else
      operation = "HEAD"; getIp(ip_to);
    getIp(ip_from);

    printf("%lu  %s -> %s HTTP %s /path_to_page_%d  %s\n"
           , counter++
           , operation
           , ip_from
           , ip_to
           , rand() % 100
           , protocol
           );
  }
}

