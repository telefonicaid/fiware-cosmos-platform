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
 * samsonCatLogs
 *
 * Example app using samsonClient lib
 * It listen std input and push content to the samson cluster indicated by parameters
 *
 * AUTHOR: Gregorio Escalada
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <unistd.h>

#include "au/network/NetworkListener.h"
#include "au/string/StringUtilities.h"
#include "logMsg/logMsg.h"
#include "parseArgs/paConfig.h"
#include "parseArgs/parseArgs.h"
#include "samson/client/SamsonClient.h"
#include "samson/client/SamsonPushBuffer.h"
#include "samson/common/coding.h"

#define TMP_LINE_MAX_LENGTH 1024

int max_rate; // Max rate
float ntimes_real_time; // Multiplicative factor respect real time
char *initial_timestamp_commandline_str[32];
int time_field;
char time_format[32];
char separator;
char filename[81];

#define YYYY_mm_dd_24H "YYYY_mm_dd_24H"
#define dd_monthlett_YY_12H_AMPM "dd_monthlett_YY_12H_AMPM"

static const char
    *manShortDescription =
        "samsonCatLogs is a tool to send data synchronously to stdin,\n"
        "so streamConnector can inject it into a SAMSON system.\n"
        "It can emulate real_time streaming from any file.\n"
        "The output rate can be set according to the log internal timestamp\n"
        "or at a fixed number of lines per sencond\n";

static const char
    *manSynopsis =
        "[-help] [-time_field val] [-sep val] [-time_format val] [-init_time val] [-nr val] [-rate val] [-file val]\n";

PaArgument paArgs[] = {
  { "-time_field", &time_field, "", PaInt, PaOpt, _i -1, _i -1, _i 20,
    "Number of field (first is 0) with time information"},
  { "-sep", &separator, "", PaChar, PaOpt, _i '|', PaNL, PaNL,
    "Log field separator, to parse time info. (Only if time_field defined)"},
  { "-time_format", time_format, "", PaString, PaOpt, _i YYYY_mm_dd_24H, PaNL, PaNL,
    "Format of the time field. Available:YYYY_mm_dd_24H, dd_monthlett_YY_12H_AMPM. (Only if time_field defined)"},
  { "-init_time", initial_timestamp_commandline_str, "", PaSList, PaOpt, PaND, PaNL, PaNL,
    "Initial timestamp (by default, first timestamp in input data. (Only if time_field defined)"},
  { "-nr", &ntimes_real_time, "", PaFloat, PaOpt, _i 1.0, _i 0.01, _i 10000.0,
    "Number of times real time. (Only if time_field defined)"},
  { "-rate", &max_rate, "", PaInt, PaOpt, 10000, 1, 10000000000,
    "Rate in lines/s. (Only if time_field is not defined)"},
  { "-file", filename, "", PaString, PaOpt, _i "stdin", PaNL, PaNL,
    "Input filename with the logs to be pushed to samson queue"},
  PA_END_OF_ARGS};

int logFd = -1;

#define  YEAR0    1900
#define  EPOCH_YR 1970
#define  SECS_DAY (24L * 60L * 60L)
#define  LEAPYEAR(year)    (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define  YEARSIZE(year)    (LEAPYEAR(year) ? 366 : 365)
#define  FIRSTSUNDAY(timp) (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define  FIRSTDAYOF(timp)  (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define  TIME_MAX ULONG_MAX
#define  ABB_LEN  3

time_t GetTimeUTCFromCalendar(struct tm *tm) {
  register long day = 0;
  register long year = EPOCH_YR;
  register int tm_year;
  int yday, month;
  register unsigned long seconds = 0;
  time_t value;

  /* Assume that when day becomes negative, there will certainly
   * be overflow on seconds.
   * The check for overflow needs not to be done for leapyears
   * divisible by 400.
   * The code only works when year (1970) is not a leapyear.
   */

  tm_year = tm->tm_year + YEAR0;


  day = (tm_year - year) * 365;
  day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
  day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
  day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

  yday = month = 0;
  switch (tm->tm_mon) {
    case 0:
      break;
    case 1:
      yday += 31;
      break;
    case 2:
      yday += 59 + LEAPYEAR(tm_year);
      break;
    case 3:
      yday += 90 + LEAPYEAR(tm_year);
      break;
    case 4:
      yday += 120 + LEAPYEAR(tm_year);
      break;
    case 5:
      yday += 151 + LEAPYEAR(tm_year);
      break;
    case 6:
      yday += 181 + LEAPYEAR(tm_year);
      break;
    case 7:
      yday += 212 + LEAPYEAR(tm_year);
      break;
    case 8:
      yday += 243 + LEAPYEAR(tm_year);
      break;
    case 9:
      yday += 273 + LEAPYEAR(tm_year);
      break;
    case 10:
      yday += 304 + LEAPYEAR(tm_year);
      break;
    case 11:
      yday += 334 + LEAPYEAR(tm_year);
      break;
  }

  yday += (tm->tm_mday - 1);
  day += yday;

  tm->tm_yday = yday;
  tm->tm_wday = (day + 4) % 7;   /* day 0 was thursday (4) */

  seconds = ((tm->tm_hour * 60L) + tm->tm_min) * 60L + tm->tm_sec;

  seconds += day * SECS_DAY;

  /* Now adjust according to timezone and daylight saving time */
  /* Not for us */

  if ((time_t)seconds != (signed)seconds) {
    value = (time_t)-1;
  } else {
    value = (time_t)seconds;
  } return value;
}

time_t GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(const char *strTimeDate) {
  struct tm tm;
  const char *p_month;
  time_t value;

#define Char_to_int(x) ((x) - 48)

  if ((strchr(strTimeDate, '/') != NULL) || (strchr(strTimeDate, '-') != NULL)) {
    // DD/MMM/YY
    tm.tm_year = 100 + Char_to_int(strTimeDate[7]) * 10 + Char_to_int(strTimeDate[8]);
    p_month = &(strTimeDate[3]);
    tm.tm_mday = Char_to_int(strTimeDate[0]) * 10 + Char_to_int(strTimeDate[1]);
  } else {
    // YYYYMMMDD
    tm.tm_year = 100 + Char_to_int(strTimeDate[2]) * 10 + Char_to_int(strTimeDate[3]);
    p_month = &(strTimeDate[4]);
    tm.tm_mday = Char_to_int(strTimeDate[7]) * 10 + Char_to_int(strTimeDate[8]);
  }
  tm.tm_mon = 12;
  if (strncmp(p_month, "JAN", strlen("JAN")) == 0) {
    tm.tm_mon = 0;
  } else if (strncmp(p_month, "FEB", strlen("FEB")) == 0) {
    tm.tm_mon = 1;
  } else if (strncmp(p_month, "MAR", strlen("MAR")) == 0) {
    tm.tm_mon = 2;
  } else if (strncmp(p_month, "APR", strlen("APR")) == 0) {
    tm.tm_mon = 3;
  } else if (strncmp(p_month, "MAY", strlen("MAY")) == 0) {
    tm.tm_mon = 4;
  } else if (strncmp(p_month, "JUN", strlen("JUN")) == 0) {
    tm.tm_mon = 5;
  } else if (strncmp(p_month, "JUL", strlen("JUL")) == 0) {
    tm.tm_mon = 6;
  } else if (strncmp(p_month, "AUG", strlen("AUG")) == 0) {
    tm.tm_mon = 7;
  } else if (strncmp(p_month, "SEP", strlen("SEP")) == 0) {
    tm.tm_mon = 8;
  } else if (strncmp(p_month, "OCT", strlen("OCT")) == 0) {
    tm.tm_mon = 9;
  } else if (strncmp(p_month, "NOV", strlen("NOV")) == 0) {
    tm.tm_mon = 10;
  } else if (strncmp(p_month, "DEC", strlen("DEC")) == 0) {
    tm.tm_mon = 11;
  }
  tm.tm_hour = Char_to_int(strTimeDate[10]) * 10 + Char_to_int(strTimeDate[11]);
  tm.tm_min = Char_to_int(strTimeDate[13]) * 10 + Char_to_int(strTimeDate[14]);
  tm.tm_sec = Char_to_int(strTimeDate[16]) * 10 + Char_to_int(strTimeDate[17]);

  // change hour from AM/PM to 24H
  const char *am_pm = &(strTimeDate[26]);
  if ((strncmp(am_pm, "pm", strlen("pm")) == 0) || (strncmp(am_pm, "PM", strlen("PM")) == 0)) {
    tm.tm_hour += 12;
  }
#undef Char_to_int
  value = GetTimeUTCFromCalendar(&tm);
  return value;
}

time_t GetTimeFromStrTimeDate_YYYY_mm_dd_24H(const char *strTimeDate) {
  struct tm tm;
  time_t value;

#define Char_to_int(x) ((x) - 48)
  // YYYY_mm-dd
  tm.tm_year = 100 + Char_to_int(strTimeDate[2]) * 10 + Char_to_int(strTimeDate[3]);
  tm.tm_mon = Char_to_int(strTimeDate[5]) * 10 + Char_to_int(strTimeDate[6]) - 1;
  tm.tm_mday = Char_to_int(strTimeDate[8]) * 10 + Char_to_int(strTimeDate[9]);

  tm.tm_hour = Char_to_int(strTimeDate[11]) * 10 + Char_to_int(strTimeDate[12]);
  tm.tm_min = Char_to_int(strTimeDate[14]) * 10 + Char_to_int(strTimeDate[15]);
  tm.tm_sec = Char_to_int(strTimeDate[17]) * 10 + Char_to_int(strTimeDate[18]);

#undef Char_to_int
  value = GetTimeUTCFromCalendar(&tm);
  return value;
}

char *ctimeUTC(time_t *timestamp) {
  struct tm tm_calendar;
  static char time_str[81];

  gmtime_r(timestamp, &tm_calendar);
  asctime_r(&tm_calendar, time_str);

  return time_str;
}


int main(int argC, const char *argV[]) {
  paConfigActions(true);

  paConfig("usage and exit on any warning", (void *) true);

  paConfig("log to screen", (void *) true);
  paConfig("log to file", (void *) true);
  paConfig("screen line format", (void *) "TYPE:EXEC: TEXT");
  paConfig("log file line format", (void *) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("man shortdescription", (void *) manShortDescription);
  paConfig("man synopsis", (void *) manSynopsis);

  // Parse input arguments
  paParse(paArgs, argC, (char **) argV, 1, false);
  logFd = lmFirstDiskFileDescriptor();

  FILE *input_file = NULL;
  if ((filename[0] != '\0') && (strcmp(filename, "stdin") != 0)) {
    if ((input_file = fopen(filename, "r")) == NULL) {
      fprintf(stderr, "Error opening input file:'%s', error:'%s", filename, strerror(errno));
      exit(-1);
    }
  } else {
    input_file = stdin;
  }

  time_t first_timestamp = 0;
  time_t first_timestamp_cmdline = 0;
  if (initial_timestamp_commandline_str[0] != NULL) {
    int ix;
    char initial_timestamp_str[1024];

    initial_timestamp_str[0] = '\0';
    for (ix = 1; ix <= (long long) initial_timestamp_commandline_str[0]; ix++) {
      strcat(initial_timestamp_str, initial_timestamp_commandline_str[ix]);
      strcat(initial_timestamp_str, " ");
    }
    if (strcmp(time_format, YYYY_mm_dd_24H) == 0) {
      first_timestamp = first_timestamp_cmdline = GetTimeFromStrTimeDate_YYYY_mm_dd_24H(initial_timestamp_str);
    } else if (strcmp(time_format, dd_monthlett_YY_12H_AMPM) == 0) {
      first_timestamp = first_timestamp_cmdline = GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(initial_timestamp_str);
    } else {
      fprintf(stderr, "Wrong timestamp format:'%s'\n", time_format);
      fprintf(stderr, "Known formats:'%s', '%s'\n", YYYY_mm_dd_24H, dd_monthlett_YY_12H_AMPM);
      exit(-2);
    }
  }

  char *time_init_str = strdup(ctimeUTC(&first_timestamp));
  time_init_str[strlen(time_init_str) - 1] = '\0';
  LOG_SM(("Final first_timestamp: %s", time_init_str));

  struct timeb prev_timebuffer;
  struct timeb act_timebuffer;
  ftime(&prev_timebuffer);
  int num_logs = 0;
  int num_skipped = 0;

  char tmp_line[TMP_LINE_MAX_LENGTH+1];
  while (fgets(tmp_line, TMP_LINE_MAX_LENGTH, input_file) != NULL) {
    if (time_field != -1) {
      // Logs are emitted following their own timestamps, as time goes by
      time_t log_timestamp = 0;
      char *parsing_line = strdup(tmp_line);
      std::vector<char *> fields = au::SplitInWords(parsing_line, separator);

      if (static_cast<int>(fields.size()) < (time_field + 1)) {
        free(parsing_line);
        continue;
      }

      if (strcmp(time_format, YYYY_mm_dd_24H) == 0) {
        log_timestamp = GetTimeFromStrTimeDate_YYYY_mm_dd_24H(fields[time_field]);
      } else if (strcmp(time_format, dd_monthlett_YY_12H_AMPM) == 0) {
        log_timestamp = GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[time_field]);
      } else {
        fprintf(stderr, "Wrong timestamp format:'%s'\n", time_format);
        fprintf(stderr, "Known formats:'%s', '%s'\n", YYYY_mm_dd_24H, dd_monthlett_YY_12H_AMPM);
        exit(-2);
      }
      if (first_timestamp == 0) {
        first_timestamp = log_timestamp;
      }

      // Probably we want to skip badly sorted logs
      if (log_timestamp < first_timestamp_cmdline) {
        ++num_skipped;
        if (num_skipped%1000000 == 0) {
          fprintf(stderr, "Skipping log with ts:'%s'", ctimeUTC(&log_timestamp));
        }
        free(parsing_line);
        continue;
      }

      fprintf(stdout, "%s", tmp_line);
      ++num_logs;
      if (num_logs%10000 == 0) {
        ftime(&act_timebuffer);
        time_t updated_time =  first_timestamp + ntimes_real_time * (act_timebuffer.time - prev_timebuffer.time);
        if (log_timestamp > updated_time) {
          useconds_t time_to_sleep = 1000000*(log_timestamp - updated_time)/ntimes_real_time;
          int rc = usleep(time_to_sleep);
          if (rc != 0) {
            fprintf(stderr, "Error(%d)('%s') in usleep with %u microseconds\n", rc, strerror(rc), time_to_sleep);
          }
          num_logs = 0;
        }
      }
      free(parsing_line);
    } else {
      // If no timestamp field specified, logs are emitted in line bursts,
      // keeping max_rate globally
      fprintf(stdout, "%s", tmp_line);
      ++num_logs;

      if (num_logs > max_rate) {
        ftime(&act_timebuffer);
        useconds_t elapsed_microseconds = 1000000*(act_timebuffer.time - prev_timebuffer.time) +
            1000*(act_timebuffer.millitm - prev_timebuffer.millitm);
        if (elapsed_microseconds > 1000000) {
          fprintf(stderr, "Warning, elapsed_microseconds(%u) > 1 second\n", elapsed_microseconds);
        } else {
          useconds_t time_to_sleep = 1000000 - elapsed_microseconds;
          int rc = usleep(time_to_sleep);
          if (rc != 0) {
            fprintf(stderr, "Error(%d)('%s') in usleep with %u microseconds\n", rc, strerror(rc), time_to_sleep);
          }
        }
        num_logs = 0;
        ftime(&prev_timebuffer);
      }
    }
  }
  if ((filename[0] != '\0') && (strcmp(filename, "stdin") != 0)) {
    int rc = fclose(input_file);
    if (rc != 0) {
      fprintf(stderr, "Error(%d)('%s') in closing file:'%s'\n", rc, strerror(rc), filename);
    }
  }
  return 0;
}

