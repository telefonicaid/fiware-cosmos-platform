#include <stdio.h>
#include <unistd.h>
#include <time.h>               // time_t
#include <string.h>

#define  YEAR0   1900
#define  EPOCH_YR   1970
#define  SECS_DAY   (24L * 60L * 60L)
#define  LEAPYEAR(year)   (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define  YEARSIZE(year)   (LEAPYEAR(year) ? 366 : 365)
#define  FIRSTSUNDAY(timp)   (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define  FIRSTDAYOF(timp)   (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define  TIME_MAX   ULONG_MAX
#define  ABB_LEN   3

time_t GetTimeUTCFromCalendar(struct tm *tm)
{
  register long day = 0;
  register long year = EPOCH_YR;
  register int tm_year;
  int yday, month;
  register unsigned long seconds = 0;
  time_t value;
  //int overflow = 0;
  //unsigned dst;

  /* Assume that when day becomes negative, there will certainly
   * be overflow on seconds.
   * The check for overflow needs not to be done for leapyears
   * divisible by 400.
   * The code only works when year (1970) is not a leapyear.
   */

  tm_year = tm->tm_year + YEAR0;

  //if (LONG_MAX / 365 < tm_year - year)
  //  overflow++;
  day = (tm_year - year) * 365;
  //        if (LONG_MAX - day < (tm_year - year) / 4 + 1)
  //          overflow++;
  day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
  day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
  day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

  yday = month = 0;
  switch (tm->tm_mon)
  {
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
  //        if (day + yday < 0)
  //          overflow++;
  day += yday;

  tm->tm_yday = yday;
  tm->tm_wday = (day + 4) % 7; /* day 0 was thursday (4) */

  seconds = ((tm->tm_hour * 60L) + tm->tm_min) * 60L + tm->tm_sec;

  //        if ((TIME_MAX - seconds) / SECS_DAY < day)
  //          overflow++;
  seconds += day * SECS_DAY;

  /* Now adjust according to timezone and daylight saving time */

  /* Not for us */
#ifdef ELIMINADO

  if (((_timezone > 0) && (TIME_MAX - _timezone < seconds))
      || ((_timezone < 0) && (seconds < -_timezone)))
    overflow++;
  seconds += _timezone;

  if (tm->tm_isdst < 0)
    dst = _dstget(tm);
  else if (tm->tm_isdst)
    dst = _dst_off;
  else
    dst = 0;

  if (dst > seconds)
    overflow++; // dst is always non-negative
  seconds -= dst;

  if (overflow)
  {
    value = (time_t) -1;
    return value;
  }
#endif /* de ELIMINADO */

  if ((time_t) seconds != (signed) seconds)
  {
    value = (time_t) - 1;
  }
  else
  {
    value = (time_t) seconds;
  }
  return value;
}


time_t GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(const char *strTimeDate)
{
  struct tm tm;
  const char *p_month;
  time_t value;

#define Char_to_int(x) ((x)-48)

  if ((strchr(strTimeDate, '/') != NULL) || (strchr(strTimeDate, '-') != NULL))
  {
    // DD/MMM/YY
    tm.tm_year = 100 + Char_to_int( strTimeDate[7] ) * 10 + Char_to_int( strTimeDate[8] );
    p_month = &(strTimeDate[3]);
    tm.tm_mday = Char_to_int( strTimeDate[0] ) * 10 + Char_to_int( strTimeDate[1] );
  }
  else
  {
    // YYYYMMMDD
    tm.tm_year = 100 + Char_to_int( strTimeDate[2] ) * 10 + Char_to_int( strTimeDate[3] );
    p_month = &(strTimeDate[4]);
    tm.tm_mday = Char_to_int( strTimeDate[7] ) * 10 + Char_to_int( strTimeDate[8] );
  }
  tm.tm_mon = 12;
  if (strncmp(p_month, "JAN", strlen("JAN")) == 0)
  {
    tm.tm_mon = 0;
  }
  else if (strncmp(p_month, "FEB", strlen("FEB")) == 0)
  {
    tm.tm_mon = 1;
  }
  else if (strncmp(p_month, "MAR", strlen("MAR")) == 0)
  {
    tm.tm_mon = 2;
  }
  else if (strncmp(p_month, "APR", strlen("APR")) == 0)
  {
    tm.tm_mon = 3;
  }
  else if (strncmp(p_month, "MAY", strlen("MAY")) == 0)
  {
    tm.tm_mon = 4;
  }
  else if (strncmp(p_month, "JUN", strlen("JUN")) == 0)
  {
    tm.tm_mon = 5;
  }
  else if (strncmp(p_month, "JUL", strlen("JUL")) == 0)
  {
    tm.tm_mon = 6;
  }
  else if (strncmp(p_month, "AUG", strlen("AUG")) == 0)
  {
    tm.tm_mon = 7;
  }
  else if (strncmp(p_month, "SEP", strlen("SEP")) == 0)
  {
    tm.tm_mon = 8;
  }
  else if (strncmp(p_month, "OCT", strlen("OCT")) == 0)
  {
    tm.tm_mon = 9;
  }
  else if (strncmp(p_month, "NOV", strlen("NOV")) == 0)
  {
    tm.tm_mon = 10;
  }
  else if (strncmp(p_month, "DEC", strlen("DEC")) == 0)
  {
    tm.tm_mon = 11;
  }

  tm.tm_hour = Char_to_int(strTimeDate[10]) * 10 + Char_to_int(strTimeDate[11]);
  tm.tm_min = Char_to_int(strTimeDate[13]) * 10 + Char_to_int(strTimeDate[14]);
  tm.tm_sec = Char_to_int(strTimeDate[16]) * 10 + Char_to_int(strTimeDate[17]);

  //change hour from AM/PM to 24H
  const char *am_pm = &(strTimeDate[26]);
  if ((strncmp(am_pm, "pm", strlen("pm")) == 0) || (strncmp(am_pm, "PM", strlen("PM")) == 0))
  {
    tm.tm_hour += 12;
  }

#undef Char_to_int
  value = GetTimeUTCFromCalendar(&tm);
  return value;
}

time_t GetTimeFromStrTimeDate_YYYY_mm_dd_24H(const char *strTimeDate)
{
  struct tm tm;
  time_t value;

#define Char_to_int(x) ((x)-48)
  // YYYY_mm-dd
  tm.tm_year = 100 + Char_to_int( strTimeDate[2] ) * 10 + Char_to_int( strTimeDate[3] );
  tm.tm_mon = Char_to_int( strTimeDate[5] ) * 10 + Char_to_int( strTimeDate[6] ) - 1;
  tm.tm_mday = Char_to_int( strTimeDate[8] ) * 10 + Char_to_int( strTimeDate[9] );

  tm.tm_hour = Char_to_int(strTimeDate[11]) * 10 + Char_to_int(strTimeDate[12]);
  tm.tm_min = Char_to_int(strTimeDate[14]) * 10 + Char_to_int(strTimeDate[15]);
  tm.tm_sec = Char_to_int(strTimeDate[17]) * 10 + Char_to_int(strTimeDate[18]);

#undef Char_to_int
  value = GetTimeUTCFromCalendar(&tm);
  return value;
}


extern char *optarg;
extern int optind, opterr, optopt;

int main (int argc, char **argv)
{
  int time_field_number = 1;
  int time_field_number_alt = 0;
  int time_field_type = 2;
  time_t timestamp;
  char separator='\t';

  FILE *fp;
  char *p_sep = NULL;
  int i = 0;
  int num_fields = 0;
  int temporal_position = 0;
  int c;


#define MAX_LENGTH_LINE 1024
  char line[MAX_LENGTH_LINE+1];

#define MAX_NUM_FIELDS 256
  char *fields[MAX_NUM_FIELDS];

  opterr = 0;

  while ((c = getopt (argc, argv, "f:a:t:s:")) != -1)
  {
    switch (c)
    {
    case 'f':
      time_field_number = atoi(optarg);
      break;
    case 'a':
      time_field_number_alt = atoi(optarg);
      break;
    case 't':
      time_field_type = atoi(optarg);
      break;
    case 's':
      separator = optarg[0];
      break;
    default:
      fprintf(stderr, "Unknown option\n");
      fprintf(stderr, "Usage:%s [-f time_field] [-a alt_time_field] [-t time_type] [-s separator] input_file\n");
      return -1;
      break;
    }
  }

  if (time_field_number_alt == 0)
  {
    time_field_number_alt = time_field_number;
  }
  if ((argc - optind) != 1)
  {
    fprintf(stderr, "Error, wrong number of arguments: %d\n", (argc-optind));
    fprintf(stderr, "Usage:%s [-f time_field] [-a alt_time_field] [-t time_type] [-s separator] input_file\n");
    return -1;
  }

  if ((fp = fopen(argv[optind], "r")) == NULL)
  {
    fprintf(stderr, "Cannot open file:'%s' for reading\n", argv[optind]);
    perror(argv[optind]);
  }

  while (fgets(line, MAX_LENGTH_LINE, fp) != NULL)
  {
    p_sep = line;
    fields[0] = p_sep;
    i = 1;
    while (((p_sep = strchr(p_sep, separator)) != NULL) && (i < MAX_NUM_FIELDS))
    {
      fields[i++] = p_sep+1;
      p_sep = p_sep+1;
    }
    num_fields = i;

    if (time_field_number > num_fields)
    {
      fprintf(stderr, "Skipping line:'%s'\n", line);
      continue;
    }

    if ((num_fields >= time_field_number) && (fields[time_field_number] - fields[time_field_number-1]) > 6)
    {
      temporal_position = time_field_number-1;
    }
    else if ((num_fields >= time_field_number_alt) && (fields[time_field_number_alt] - fields[time_field_number_alt-1]) > 6)
    {
      temporal_position = time_field_number_alt-1;
    }
    else
    {
      fprintf(stderr, "fields[time_field_number:%p, fields[time_field_number-1]:%p, (fields[time_field_number] - fields[time_field_number-1]):%d\n", fields[time_field_number], fields[time_field_number-1], (fields[time_field_number] - fields[time_field_number-1]));
      fprintf(stderr, "Wrong time_field_number:%d and time_field_number_alt:%d, num_fields:%d\n", time_field_number, time_field_number_alt, num_fields);
      continue;
    }
    if (time_field_type == 1)
    {
      timestamp = GetTimeFromStrTimeDate_YYYY_mm_dd_24H(fields[temporal_position]);
    }
    else if (time_field_type == 2)
    {
      timestamp = GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[temporal_position]);
    }

    fprintf(stdout, "%lu¬%s", timestamp, line);
  }
  fprintf(stderr, "File ended\n");
  
  fclose(fp);
}