
/*
 *
 * passiveLocationPush
 *
 * Example app using samsonClient lib
 * It generates random xml documents simulating information from OSS Passive Location pilot
 *
 * AUTHOR: Andreu Urruela
 *
 */


#include <iostream>     // std::cout
#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <time.h>       // strptime, struct tm


#include "au/CommandLine.h"     // au::CommandLine
#include "au/statistics/Cronometer.h"      // au::Cronometer
#include "au/string/StringUtilities.h"                  // au::str()
#include "au/time.h"                    // au::todatString()

#include "samson/client/SamsonClient.h"  // samson::SamsonClient

size_t get_user_id(size_t pos) {
  return 666666666 + pos;
}

int main(int argc, const char *argv[]) {
  au::CommandLine cmd;

  cmd.SetFlagUint64("users", 100000);
  cmd.SetFlagUint64("num", 0);      // Number of messages
  cmd.SetFlagBoolean("random");        // Generate a message for all users
  cmd.Parse(argc, argv);

  size_t num_users = cmd.GetFlagUint64("users");
  size_t num       = cmd.GetFlagUint64("num");
  bool random_user = cmd.GetFlagBool("random");

  srand(time(NULL));


  if (cmd.get_num_arguments() < 2) {
    fprintf(stderr, "Usage %s rate_in_messages_per_second -users <num_users> -num <max_num_messages> -random \n",
            argv[0]);
    exit(0);
  }

  size_t rate = atoll(cmd.get_argument(1).c_str());


  fprintf(stderr, "Running generator with rate=%lu num=%lu users=%lu\n", (unsigned long int)rate,
          (unsigned long int)num,
          (unsigned long int)num_users);


  // Small mini-buffer to generate
  char *line = (char *)malloc(20000);

  // Control of time and size
  au::Cronometer cronometer;

  size_t last_user = 0;

  size_t total_size = 0;
  size_t num_messages = 0;

  size_t theoretical_seconds = 0;

  while (true) {
    // Generate 5 seconds data
    fprintf(stderr, "Generating %d messages ( messages in 5 seconds at %lu events/s )\n", (int)(5 * rate),
            (unsigned long int)rate);

    for (int i = 0; i < (int)(5 * rate); i++) {
      size_t user_id;
      if (random_user) {
        user_id = get_user_id(rand() % num_users);
      } else {
        last_user = get_user_id((last_user + 1) % num_users);
        user_id = last_user;
      }


      int cell = 65528;

      if ((time(NULL) % 200) > 100) {
        cell = 65534;
      }
      snprintf(
        line, 20000,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ns0:AMRReport xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'  xmlns:ns0='http://O2.arcanum.vitria.com'  xsi:schemaLocation='http://O2.arcanum.vitria.com AMR.xsd'>  <SubscriberReport>    <User>      <IMSI>%lu</IMSI>      <PTMSI>FB869371</PTMSI>  <CellID>%d</CellID>   <Paging>      <Location>        <LocationArea>12124</LocationArea>        <RoutingArea>134</RoutingArea>      </Location>    </Paging>  </SubscriberReport>  <Timestamp>2011-07-21T16:07:47</Timestamp></ns0:AMRReport>",
        (unsigned long int)user_id, cell);

      total_size += strlen(line);
      num_messages++;

      // Emit line to the output
      std::cout << line << "\n";

      if (num > 0) {
        if (num_messages >= num) {
          fprintf(stderr, "Generated %s messages", au::str(num_messages).c_str());
          exit(0);
        }
      }
    }

    // Detect if we need to sleep....
    theoretical_seconds += 5;

    size_t ellapsed_seconds = cronometer.seconds();

    // Sleep some time to simulate a particular rate
    if (ellapsed_seconds < theoretical_seconds) {
      int sleep_seconds = theoretical_seconds - ellapsed_seconds;
      std::cerr << "Sleeping " << sleep_seconds << " seconds... We have generate " << au::str(num_messages) <<
      " messages with size " <<
      au::str(total_size,
              "bytes") << " time " << au::str_time(ellapsed_seconds) << " theoretical time " << au::str_time(
        theoretical_seconds) << "\n";
      sleep(sleep_seconds);
    }
  }
}

