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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>               /* time, gmtime_r, ...    */
#include <unistd.h>             /* getopt()               */

#define TIME_FORMAT  "%Y-%m-%d:%H:%M:%S"
const int kRandModulus = 10;
const int kProbToKeepSensor = 8; // of kRandModulus

int main(int argc, char *argv[]) {
  int i;

  int num_users = 100000;
  int num_sensors = 1000;
  int beat_period = 5;
  double max_dim_X = 1000;
  double max_dim_Y = 1000;
  char info_pos_filename[81];
  FILE *info_pos = NULL;

  double incX;
  double incY;

  int num_pos_X;
  int *last_position;

  extern char *optarg;
  int c;

  strncpy(info_pos_filename, "info_pos_sensors.tab", sizeof(info_pos_filename));
  while((c=getopt(argc, argv, "n:N:b:X:Y:")) != EOF) {
    char *endptr;
    switch(c) {
      case 'n':
        num_users = strtoul(optarg, &endptr, 10);
        break;
      case 'N':
        num_sensors = strtoul(optarg, &endptr, 10);
        break;
      case 'b':
        beat_period = strtoul(optarg, &endptr, 10);
        break;
      case 'X':
        max_dim_X = strtod(optarg, &endptr);
        break;
      case 'Y':
        max_dim_Y = strtod(optarg, &endptr);
        break;
      case 'f':
        strncpy(info_pos_filename, optarg, sizeof(info_pos_filename));
        break;
      default:
        fprintf(stderr, "%s. Usage:\n\t%s [-n max_num_users(%d)] [-N max_num_sensors(%d)] [-b beat_period(%d)] ",
                argv[0], argv[0], num_users, num_sensors, beat_period);
        fprintf(stderr, "[-X max_dim_X(%.2lf)] [-Y max_dim_Y(%.2lf)] [-f filename for sensor position table(\"%s\")]\n",
                max_dim_X, max_dim_Y, info_pos_filename);
        exit(1);
    }
  }

  incX = max_dim_X/sqrt(num_sensors);
  incY = max_dim_Y/sqrt(num_sensors);
  num_pos_X = max_dim_X / incX;

  if ((info_pos = fopen(info_pos_filename, "w")) == NULL) {
    fprintf(stderr, "Error opening output file:'%s'", info_pos_filename);
    perror(info_pos_filename);
    exit(2);
  }
  for (i = 0; i < num_sensors; ++i) {
    fprintf(info_pos, "%d|%d,%d\n", i, (int)((i/num_pos_X)*incX), (int)((i%num_pos_X)*incY));
  }
  fclose(info_pos);

  last_position = static_cast<int *>(malloc(num_users * sizeof(*last_position)));
  for (i = 0; i < num_users; ++i) {
    last_position[i] = rand()%num_sensors;
  }

  while (1) {
    char time_string[81];
    time_t secondsNow = time(NULL);
    struct tm  time_calendar;
    gmtime_r(&secondsNow, &time_calendar);
    strftime(time_string, sizeof(time_string), TIME_FORMAT, &time_calendar);

    for (i = 0; i < num_users; ++i) {
      fprintf(stdout, "%d|%d|%s\n", i, last_position[i], time_string);
      int jump = rand()%kRandModulus;
      if (jump > kProbToKeepSensor) {
        last_position[i] = rand()%num_sensors;
      }
    }
    sleep(beat_period);
  }
}
