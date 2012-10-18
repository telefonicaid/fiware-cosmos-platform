

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "parseArgs/paConfig.h"
#include "parseArgs/parseArgs.h"

#include "logMsg/logMsg.h"

#include "au/CommandLine.h"  // au::CommandLine
#include "au/statistics/Cronometer.h"  // au::Cronometer
#include "au/string/string.h"  // au::str()



static const char *manShortDescription =  "Simple command line tool to generate fake data for simple_mobility demo\n";
static const char *manSynopsis =
  "simple_mobility_generator [-commands] [-rate X] [-users X] [-period X] [-progressive] [-fix_position]\n";

int users;
int rate;
int period;
bool progresive;
bool commands;
bool fix_position;

PaArgument paArgs[] =
{
  { "-users",        &users,        "",         PaInt,          PaOpt,          100000,          100,
    100000000,
    "Number of users"                                                                     },
  { "-rate",         &rate,         "",         PaInt,          PaOpt,          10000,           100,
    1000000,
    "CDRs per second"                                                                     },
  { "-period",       &period,       "",         PaInt,          PaOpt,          100,             10,
    86400,
    "Period in seconds"                                                                   },
  { "-progressive",  &progresive,   "",         PaBool,         PaOpt,          false,           false,
    true,
    "Generate cdrs in sequential order for all the users"                    },
  { "-commands",     &commands,     "",         PaBool,         PaOpt,          false,           false,
    true,
    "Generate commands to create user-areas (instead of cdrs)"               },
  { "-fix_position", &fix_position, "",         PaBool,         PaOpt,          false,           false,
    true,
    "Use fix positions for all users"                                                     },
  PA_END_OF_ARGS
};

int logFd = -1;

int current_user;


class Position {
public:

  double x;
  double y;

  Position() {
    x = 0;
    y = 0;
  }

  Position(double _x, double _y) {
    x = _x;
    y = _y;
  }

  void set(double _x, double _y) {
    x = _x;
    y = _y;
  }

  void set_in_limits() {
    set_in_limits(&x);
    set_in_limits(&y);
  }

private:

  void set_in_limits(double *var) {
    if (*var <  0) {
      *var = 0;
    }
    if (*var > 1000) {
      *var = 1000;
    }
  }
};

Position getHome(size_t user) {
  if (fix_position) {
    return Position(10, 10);
  }

  return Position(10 * ( user % 100 ), 1000 - 10 * ( user % 100 ));
}

Position getWork(size_t user) {
  if (fix_position) {
    return Position(900, 900);
  }

  return Position(1000 - 10 * ( user % 100 ), 10 * ( user % 100 ));
}

Position getPosition(size_t user) {
  int p = time(NULL) % period;

  if (p < (period / 2)) {
    return getHome(user);
  } else {
    return getWork(user);
  }

  return Position(rand() % 1000, rand() % 1000);
}

size_t getUser() {
  if (progresive) {
    current_user++;
    if (current_user >= users) {
      current_user = 0;
    }
    return current_user;
  }

  return rand() % users;
}

int main(int argC, const char *argV[]) {
  // Random sequence generated
  srand(time(NULL));

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

  current_user = 0;

  LM_V(("--------------------------------------------------"));
  LM_V(("Setup"));
  LM_V(("--------------------------------------------------"));
  LM_V(("Users %d", users));
  LM_V(("Rate %d", rate));
  LM_V(("--------------------------------------------------"));

  size_t total_num = 0;
  size_t total_size = 0;

  if (commands) {
    for (int i = 0; i < users; i++) {
      Position home = getHome(i);
      Position work = getWork(i);
      printf("%d AREA_CREATE home %f %f 200 \n", i,  home.x, home.y);
      printf("%d AREA_CREATE work %f %f 200 \n", i, work.x, work.y);
    }

    LM_M(("Generated %d messages", users ));
    return 0;
  }

  au::Cronometer cronometer;
  size_t theoretical_seconds = 0;

  while (true) {
    // Generate messages for the next second....
    theoretical_seconds += 1;
    for (int i = 0; i < rate; i++) {
      int user = getUser();
      Position p = getPosition(user);

      total_size += printf("%d CDR %f %f %lu\n", user, p.x, p.y, time(NULL));
      total_num++;
    }

    size_t total_seconds = cronometer.seconds();

    if (total_seconds < theoretical_seconds) {
      int seconds_to_sleep = (int)theoretical_seconds - total_seconds;

      LM_V(("Sleeping %d seconds to keep rate %s", seconds_to_sleep, au::str(rate, "Events/sec").c_str()));
      sleep(seconds_to_sleep);
    }

    if ((theoretical_seconds % 10) == 0) {
      LM_V(("Generated %s lines ( %s bytes ) in %s. Rate: %s / %s."
            , au::str(total_num).c_str()
            , au::str(total_size).c_str()
            , au::str_time(total_seconds).c_str()
            , au::str((double)total_num / (double)total_seconds, "Lines/s").c_str()
            , au::str((double)total_size / (double)total_seconds, "Bps").c_str()
            ));
    }
  }
}

