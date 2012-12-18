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
 * samsonDataSerializer
 *
 * Tool to send data synchronously to stdin
 * It reads stdin or a text file, and writes to stdout either keeping a fix rate in lines per second,
 * or according the log internal timestamp.
 * This stdout can be piped to streamConnector and inject thus logs into SAMSON controlling their rate
 *
 * AUTHOR: Gregorio Escalada
 *
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <unistd.h>

#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "parseArgs/paConfig.h"
#include "parseArgs/parseArgs.h"

#define TMP_LINE_MAX_LENGTH 1024

int max_rate; // Max rate
float ntimes_real_time; // Multiplicative factor respect real time
char initial_timestamp_commandline_str[81];
int time_field;
char time_format[32];
char separator;
char filename[81];

#define YYYY_mm_dd_24H "YYYY_mm_dd_24H"
#define dd_monthlett_YY_12H_AMPM "dd_monthlett_YY_12H_AMPM"

static const char
    *manShortDescription =
        "samsonDataSerializer  is a tool to send data synchronously to stdin,\n"
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
  { "-init_time", initial_timestamp_commandline_str, "", PaString, PaOpt, PaND, PaNL, PaNL,
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
#define  MONTH_ABB_LEN  3
#define  NUM_MONTHS 12

const char kMonthAbreviations[NUM_MONTHS][MONTH_ABB_LEN+1] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP",
                                                     "OCT", "NOV", "DEC"};

inline int CharToInt(char x) {
  return static_cast<int>(x - '0');
}

/**
 * \brief Interprets the contents of the tm structure pointed by tm as a calendar time expressed in UTC.
 * This calendar time is used to adjust the values of the members of tm (yday and wday)
 * accordingly and returned as an object of type time_t.
 * Similar to the C library mktime, but avoids using the local time
 */
time_t mktimeUTC(struct tm *tm) {
  register long day = 0;
  register long year = EPOCH_YR;
  register int tm_year;
  int yday, month;
  register unsigned long seconds = 0;

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
    return static_cast<time_t>(-1);
  }
  return static_cast<time_t>(seconds);
}

time_t GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(const char *strTimeDate) {
  struct tm tm;
  const char *p_month;
  time_t value;

  if ((strchr(strTimeDate, '/') != NULL) || (strchr(strTimeDate, '-') != NULL)) {
    // DD/MMM/YY
    tm.tm_year = 100 + CharToInt(strTimeDate[7]) * 10 + CharToInt(strTimeDate[8]);
    p_month = &(strTimeDate[3]);
    tm.tm_mday = CharToInt(strTimeDate[0]) * 10 + CharToInt(strTimeDate[1]);
  } else {
    // YYYYMMMDD
    tm.tm_year = 100 + CharToInt(strTimeDate[2]) * 10 + CharToInt(strTimeDate[3]);
    p_month = &(strTimeDate[4]);
    tm.tm_mday = CharToInt(strTimeDate[7]) * 10 + CharToInt(strTimeDate[8]);
  }
  tm.tm_mon = 12;

  for (unsigned int ix = 0; ix < NUM_MONTHS; ++ix) {
    if (strncmp(kMonthAbreviations[ix], p_month, MONTH_ABB_LEN) == 0) {
      tm.tm_mon = ix;
      break;
    }
  }

  tm.tm_hour = CharToInt(strTimeDate[10]) * 10 + CharToInt(strTimeDate[11]);
  tm.tm_min = CharToInt(strTimeDate[13]) * 10 + CharToInt(strTimeDate[14]);
  tm.tm_sec = CharToInt(strTimeDate[16]) * 10 + CharToInt(strTimeDate[17]);

  // change hour from AM/PM to 24H
  // We are receiving time "12:00:00 PM" being 12:00:00 in 24H format
  if (tm.tm_hour == 12) {
    tm.tm_hour = 0;
  }
  const char *am_pm = NULL;
  if (strTimeDate[18] == ' ') {
    am_pm = &(strTimeDate[19]);
  } else {
    am_pm = &(strTimeDate[26]);
  }
  if ((strncmp(am_pm, "pm", strlen("pm")) == 0) || (strncmp(am_pm, "PM", strlen("PM")) == 0)) {
    tm.tm_hour += 12;
  }

  value = mktimeUTC(&tm);
  return value;
}

time_t GetTimeFromStrTimeDate_YYYY_mm_dd_24H(const char *strTimeDate) {
  struct tm tm;
  time_t value;

  // YYYY_mm-dd
  tm.tm_year = 100 + CharToInt(strTimeDate[2]) * 10 + CharToInt(strTimeDate[3]);
  tm.tm_mon = CharToInt(strTimeDate[5]) * 10 + CharToInt(strTimeDate[6]) - 1;
  tm.tm_mday = CharToInt(strTimeDate[8]) * 10 + CharToInt(strTimeDate[9]);

  tm.tm_hour = CharToInt(strTimeDate[11]) * 10 + CharToInt(strTimeDate[12]);
  tm.tm_min = CharToInt(strTimeDate[14]) * 10 + CharToInt(strTimeDate[15]);
  tm.tm_sec = CharToInt(strTimeDate[17]) * 10 + CharToInt(strTimeDate[18]);

  value = mktimeUTC(&tm);
  return value;
}

char *ctimeUTC(const time_t& timestamp) {
  struct tm tm_calendar;
  static char time_str[81];

  gmtime_r(&timestamp, &tm_calendar);
  asctime_r(&tm_calendar, time_str);
  return time_str;
}

time_t GetTimeFromStrTimeDate(const char *timestamp, const char *time_format) {
  if (*timestamp == '\0') {
    return 0;
  }

  if (strcmp(time_format, YYYY_mm_dd_24H) == 0) {
    return GetTimeFromStrTimeDate_YYYY_mm_dd_24H(timestamp);
  } else if (strcmp(time_format, dd_monthlett_YY_12H_AMPM) == 0) {
    return GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(timestamp);
  } else {
    fprintf(stderr, "Wrong timestamp format:'%s'\n", time_format);
    fprintf(stderr, "Known formats:'%s', '%s'\n", YYYY_mm_dd_24H, dd_monthlett_YY_12H_AMPM);
    exit(-2);
  }
}

time_t RecoverTimeFromLog(const char *tmp_line, int time_field, const char *time_format) {
  std::vector<std::string> fields;
  std::string sep_str(1, separator);
  boost::split(fields, tmp_line, boost::is_any_of(sep_str));
  if (fields.size() < (time_field + 1)) {
    return 0;
  }
  return GetTimeFromStrTimeDate(fields[time_field].c_str(), time_format);
}

void UpdateReferenceTimeAtSeconds(time_t log_timestamp, time_t first_timestamp, time_t initial_time,
    float ntimes_real_time) {
  struct timeb act_timebuffer;
  ftime(&act_timebuffer);
  time_t updated_time =  first_timestamp + ntimes_real_time * (act_timebuffer.time - initial_time);
  if (log_timestamp > updated_time) {
    useconds_t time_to_sleep = 1000000*(log_timestamp - updated_time)/ntimes_real_time;
    int rc = usleep(time_to_sleep);
    if (rc != 0) {
      fprintf(stderr, "Error(%d)('%s') in usleep with %u microseconds\n", rc, strerror(rc), time_to_sleep);
    }
  } else {
    fprintf(stderr, "Not able to keep up with the desired ntimes real time. log_ts:%lu, update:%lu\n", log_timestamp, updated_time);
  }
}

void UpdateReferenceTimeSleepingToNextSecond(struct timeb *prev_timebuffer) {
  struct timeb act_timebuffer;
  ftime(&act_timebuffer);
  useconds_t elapsed_microseconds = 1000000*(act_timebuffer.time - prev_timebuffer->time) +
      1000*(act_timebuffer.millitm - prev_timebuffer->millitm);
  if (elapsed_microseconds > 1000000) {
    fprintf(stderr, "Warning, elapsed_microseconds(%u) > 1 second\n", elapsed_microseconds);
  } else {
    useconds_t time_to_sleep = 1000000 - elapsed_microseconds;
    int rc = usleep(time_to_sleep);
    if (rc != 0) {
      fprintf(stderr, "Error(%d)('%s') in usleep with %u microseconds\n", rc, strerror(rc), time_to_sleep);
    }
  }
  ftime(prev_timebuffer);
}

int main(int argC, char **argV) {
  paConfigActions(true);

  paConfig("usage and exit on any warning", (void *) true);

  paConfig("log to screen", (void *) true);
  paConfig("log to file", (void *) true);
  paConfig("screen line format", (void *) "TYPE:EXEC: TEXT");
  paConfig("log file line format", (void *) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("man shortdescription", (void *) manShortDescription);
  paConfig("man synopsis", (void *) manSynopsis);

  // Parse input arguments
  paParse(paArgs, argC, argV, 1, false);
  //logFd = lmFirstDiskFileDescriptor();

  FILE *input_file = NULL;
  // If filename is "stdin", no need to open the stream
  // Just in case the users doesn't assign an input filename,
  // and default value is changed to "", we check also this
  // option in the if statement
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
  first_timestamp = first_timestamp_cmdline = GetTimeFromStrTimeDate(initial_timestamp_commandline_str, time_format);

  if (*initial_timestamp_commandline_str != '\0') {
    fprintf(stderr, "Decoded initial timestamp:%s",ctimeUTC(first_timestamp));
  }

  struct timeb prev_timebuffer;
  ftime(&prev_timebuffer);
  int num_logs = 0;
  int num_skipped = 0;

  char tmp_line[TMP_LINE_MAX_LENGTH+1];
  while (fgets(tmp_line, TMP_LINE_MAX_LENGTH, input_file) != NULL) {
    if (time_field != -1) {
      // Logs are emitted following their own timestamps, as time goes by
      time_t log_timestamp = RecoverTimeFromLog(tmp_line, time_field, time_format);

      if (log_timestamp == 0) {
        // Badly formatted log. Ignore it
        continue;
      }
      // Probably we want to skip logs before the initial time
      if (log_timestamp < first_timestamp_cmdline) {
        if (++num_skipped == 1000000) {
          fprintf(stderr, "Skipping log with ts:%s", ctimeUTC(log_timestamp));
          num_skipped = 0;
        }
        continue;
      }
      if (first_timestamp == 0) {
        // If no initial timestamp selected by the user, take the first in the file
        first_timestamp = log_timestamp;
      }

      // Finally we emit the log. Probably it should be done after checking the reference time,
      // but we have decided to just sample this reference time to avoid the ftime() overhead
      fprintf(stdout, "%s", tmp_line);

      if (++num_logs == 1000) {
        UpdateReferenceTimeAtSeconds(log_timestamp, first_timestamp, prev_timebuffer.time, ntimes_real_time);
        num_logs = 0;
      }

    } else {
      // If no timestamp field specified, logs are emitted in line bursts,
      // keeping max_rate globally
      fprintf(stdout, "%s", tmp_line);

      if (++num_logs > max_rate) {
        UpdateReferenceTimeSleepingToNextSecond(&prev_timebuffer);
        num_logs = 0;
      }
    }
  }

  if (input_file != stdin) {
    int rc = fclose(input_file);
    if (rc != 0) {
      fprintf(stderr, "Error(%d)('%s') in closing file:'%s'\n", rc, strerror(rc), filename);
    }
  }
  return 0;
}

