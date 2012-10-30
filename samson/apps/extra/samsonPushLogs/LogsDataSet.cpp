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


#include <iostream>             // std::cout
#include <stdio.h>              // printf
#include <stdlib.h>             // exit
#include <stdlib.h>             // free
#include <string.h>             // memcpy
#include <sys/types.h>          // types needed by socket include files
#include <time.h>               // strptime, struct tm
#include <dirent.h>             // opendir, scandir
#include <errno.h>              // errno
#include <errno.h>              // errno
#include <fcntl.h>              // O_RDONLY
#include <inttypes.h>           // uint64_t etc.
#include <sys/stat.h>           // stat
#include <unistd.h>             // close, lseek
#include <vector>               // std::vector

#include "LogsDataSet.h"  // Own interface
#include "logMsg/logMsg.h"      // traces LM_E, LM_W....
#include "au/string/StringUtilities.h"

char *fgetsFromFd(char *str, int line_max_size, int fd) {
  char *p_str = str;

  for (int i = 0; (i < line_max_size); i++, p_str++) {
    size_t nbytes_to_read = 1;
    ssize_t bytes_read = 0;

    if ((bytes_read = read(fd, (void *)p_str,  nbytes_to_read)) != (ssize_t)nbytes_to_read) {
      return NULL;
    }

    if (*p_str == '\n') {
      *(p_str + 1) = '\0';
      // LM_M(("Logline:'%s'", str));
      return str;
    }
  }
  return NULL;
}


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

  // int overflow = 0;
  // unsigned dst;

  /* Assume that when day becomes negative, there will certainly
   * be overflow on seconds.
   * The check for overflow needs not to be done for leapyears
   * divisible by 400.
   * The code only works when year (1970) is not a leapyear.
   */

  tm_year = tm->tm_year + YEAR0;

  // if (LONG_MAX / 365 < tm_year - year)
  //  overflow++;
  day = (tm_year - year) * 365;
  //        if (LONG_MAX - day < (tm_year - year) / 4 + 1)
  //          overflow++;
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
  //        if (day + yday < 0)
  //          overflow++;
  day += yday;

  tm->tm_yday = yday;
  tm->tm_wday = (day + 4) % 7;   /* day 0 was thursday (4) */

  seconds = ((tm->tm_hour * 60L) + tm->tm_min) * 60L + tm->tm_sec;

  //        if ((TIME_MAX - seconds) / SECS_DAY < day)
  //          overflow++;
  seconds += day * SECS_DAY;

  /* Now adjust according to timezone and daylight saving time */

  /* Not for us */
#ifdef ELIMINADO

  if (((_timezone > 0) && (TIME_MAX - _timezone < seconds))
      || ((_timezone < 0) && (seconds < -_timezone)))
  {
    overflow++;
  }
  seconds += _timezone;

  if (tm->tm_isdst < 0) {
    dst = _dstget(tm);
  } else if (tm->tm_isdst) {
    dst = _dst_off;
  } else {
    dst = 0;
  }

  if (dst > seconds) {
    overflow++;     // dst is always non-negative
  }
  seconds -= dst;

  if (overflow) {
    value = (time_t)-1;
    return value;
  }
#endif  /* de ELIMINADO */

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

LogsDataSet::LogsDataSet(const char *dir_path, const char *extension, int num_fields, int timestamp_position,
                         int timestamp_position_alt,
                         int timestamp_type,
                         const char *queue_name) {
  dir_path_ = strdup(dir_path);
  extension_ = strdup(extension);
  num_fields_ = num_fields;
  timestamp_position_ = timestamp_position;
  timestamp_position_alt_ = timestamp_position_alt;
  timestamp_type_ = timestamp_type;
  first_timestamp_ = 0;
  separator_ = '\t';
  if (queue_name == NULL) {
    LM_E(("Error, queue_name is a required parameter"));
    queue_name_ = NULL;
  } else {
    queue_name_ = strdup(queue_name);
  }
  finished_ = false;
}

LogsDataSet::~LogsDataSet() {
  free(dir_path_);
  free(extension_);
  free(queue_name_);
}

bool LogsDataSet::InitDir() {
  /*
   * //  if (chdir(dir_path_) == -1)
   * //  {
   * //    LM_E(("Error changing to directory '%s': %s", dir_path_, strerror(errno)));
   * //    return false;
   * //  }
   */

  DIR *dir = opendir(dir_path_);

  if (dir == NULL) {
    LM_E(("opendir(%s): %s", dir_path_, strerror(errno)));
    finished_ = true;
    return false;
  }


  //
  // Count number of files in the directory
  //
  num_files_ = 0;
  struct dirent *entry = NULL;

  while ((entry = readdir(dir)) != NULL) {
    const char *suff;

    if (entry->d_name[0] == '.') {
      continue;
    }
    suff = strrchr(entry->d_name, '.');
    if (suff == NULL) {
      continue;
    }
    ++suff;
    if (strcmp(suff, extension_) != 0) {
      continue;
    }
    ++num_files_;
  }
  LM_V(("Initially found %d files in '%s'", num_files_, dir_path_));

  if (num_files_ == 0) {
    finished_ = true;
    return false;
  }

  //
  // Create a vector to hold the files
  //
  file_vector_ = (File *)calloc(num_files_, sizeof(File));
  if (file_vector_ == NULL) {
    LM_E(("error allocating vector for %d files: %s", num_files_, strerror(errno)));
    closedir(dir);
    finished_ = true;
    return false;
  }


  //
  // Gather info on the files and fill the vector
  //
  rewinddir(dir);
  int ix = 0;
  while ((entry = readdir(dir)) != NULL) {
    char *suff;
    char filename_tmp[256];
    struct stat statBuf;

    if (entry->d_name[0] == '.') {
      continue;
    }
    suff = strrchr(entry->d_name, '.');
    if (suff == NULL) {
      continue;
    }
    ++suff;
    if (strcmp(suff, extension_) != 0) {
      continue;
    }
    snprintf(filename_tmp, 256, "%s/%s", dir_path_, entry->d_name);

    if (stat(filename_tmp, &statBuf) == -1) {
      LM_E(("stat(%s): %s", entry->d_name, strerror(errno)));
      continue;
    }

    if (!S_ISREG(statBuf.st_mode)) {
      continue;
    }
    if (strlen(filename_tmp) > sizeof(file_vector_[ix].name)) {
      LM_E(("File name too long: '%s'", entry->d_name));
      continue;
    }

    LM_V2(("Adding file '%s'", entry->d_name));

    strncpy(file_vector_[ix].name, filename_tmp, 256);
    file_vector_[ix].date         = statBuf.st_mtime;
    file_vector_[ix].size         = statBuf.st_size;
    file_vector_[ix].fd           = -1;
    file_vector_[ix].already_read = false;

    ++ix;
  }
  closedir(dir);

  num_files_ = ix;
  LM_V(("%d files in vector", num_files_));

  if (num_files_ == 0) {
    LM_W(("No file found at directory:%s with extension:%s", dir_path_, extension_));
    finished_ = true;
  }


  //
  // Sort the vector in name order (smallest first)
  //
  int first;
  int second;
  int smallest_index = -1;
  File *smallest;
  File copy;

  for (first = 0; first < num_files_ - 1; first++) {
    smallest = &file_vector_[first];

    for (second = first + 1; second < num_files_; second++) {
      if (strcmp(file_vector_[second].name, smallest->name) < 0) {
        smallest = &file_vector_[second];
        smallest_index = second;
      }
    }
    LM_V(("%05d: '%s'", first, smallest->name));

    // Swap
    if (smallest != &file_vector_[first]) {
      LM_VV(("Copying smallest from index %d to %d", smallest_index, first));
      memcpy(&copy, &file_vector_[first], sizeof(copy));
      memcpy(&file_vector_[first], smallest, sizeof(file_vector_[first]));
      memcpy(smallest, &copy, sizeof(copy));
    }
  }
  return true;
}

bool LogsDataSet::Synchronize(time_t time_init) {
  char *time_init_str = strdup(ctimeUTC(&time_init));

  time_init_str[strlen(time_init_str) - 1] = '\0';

  int count_skip  = 0;
  while (true) {
    char *log_line;
    time_t timestamp;

    if ((GetLogLineEntry(&log_line, &timestamp)) == false) {
      // LM_W(("Skipping wrong entry in dataset: %s", dataset_->GetQueueName()));
      if (finished_) {
        return false;
      }

      continue;
    }

    char *time_read_str = strdup(ctimeUTC(&timestamp));
    time_read_str[strlen(time_read_str) - 1] = '\0';

    if (timestamp > time_init) {
      free(time_read_str);
      free(log_line);
      return true;
    }
    if (count_skip % 100000000 == 0) {
      LM_M(("Skipping %s with read_timestamp:%s and first_timestamp:%s", GetQueueName(), time_read_str, time_init_str));
    }
    free(time_read_str);
    free(log_line);
    count_skip++;
  }
  return false;   // Impossible
}

bool LogsDataSet::GetLogLineEntry(char **log, time_t *timestamp) {
#define LOGSDATASET_LINE_MAX_LENGTH 1024
  char temporal_buffer[LOGSDATASET_LINE_MAX_LENGTH + 1];

  for (int file_index = 0; file_index < num_files_; file_index++) {
    if (file_vector_[file_index].already_read == true) {
      continue;
    }
    if (file_vector_[file_index].fd == -1) {
      if ((file_vector_[file_index].fp = fopen(file_vector_[file_index].name, "r")) == NULL) {
        LM_E(("Error opening file:'%s' (%d of %d), errno:%s %d", file_vector_[file_index].name, file_index, num_files_,
              strerror(errno),
              errno));
        file_vector_[file_index].already_read = true;
        continue;
      }
      file_vector_[file_index].fd = fileno(file_vector_[file_index].fp);
    }

    while (fgets(temporal_buffer, LOGSDATASET_LINE_MAX_LENGTH, file_vector_[file_index].fp) != NULL) {
      *log = strdup(temporal_buffer);
      if (timestamp_position_ != -1) {
        std::vector<char *> fields = au::SplitInWords(temporal_buffer, separator_);
        if (static_cast<int>(fields.size()) != num_fields_) {
          free(*log);
          *log = NULL;
          continue;
        }

        if (static_cast<int>(fields.size()) < (timestamp_position_ + 1)) {
          temporal_buffer[strlen(temporal_buffer) - 1] = '\0';
          // LM_W(("Error reading empty line:'%s' from fd:%d file_index:%d (%s)", temporal_buffer, file_vector_[file_index].fd, file_index, file_vector_[file_index].name));
          free(*log);
          *log = NULL;
          continue;
        }

        int temporal_position = 0;
        if (strlen(fields[timestamp_position_]) < 6) {
          if (strlen(fields[timestamp_position_alt_]) < 6) {
            // LM_W(("Error detecting timestamp in pos:%d and %d for line:%s", timestamp_position_, timestamp_position_alt_, temporal_buffer));
            free(*log);
            *log = NULL;
            continue;
          } else {
            temporal_position = timestamp_position_alt_;
          }
        } else {
          temporal_position = timestamp_position_;
        }

        if (timestamp_type_ == 1) {
          *timestamp = GetTimeFromStrTimeDate_YYYY_mm_dd_24H(fields[temporal_position]);
        } else if (timestamp_type_ == 2) {
          *timestamp = GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[temporal_position]);
        }
      } else {
        *timestamp = -1;
      }
      return true;
    }

    // If it arrives here, the input file has been finished
    fclose(file_vector_[file_index].fp);
    file_vector_[file_index].fp = NULL;
    file_vector_[file_index].fd = -1;
    file_vector_[file_index].already_read = true;
  }
  LM_W(("End of files in directory '%s' reached after scanning %d valid files", dir_path_, num_files_));
  finished_ = true;
  return false;
}

bool LogsDataSet::LookAtNextLogLineEntry(char **log, time_t *timestamp) {
#define LOGSDATASET_LINE_MAX_LENGTH 1024
  char temporal_buffer[LOGSDATASET_LINE_MAX_LENGTH + 1];

  for (int file_index = 0; file_index < num_files_; file_index++) {
    if (file_vector_[file_index].already_read == true) {
      LM_M(("Skipping already read file:%d (%s)", file_index, file_vector_[file_index].name));
      continue;
    }

    if (file_vector_[file_index].fp == NULL) {
      if ((file_vector_[file_index].fp = fopen(file_vector_[file_index].name, "r")) == NULL) {
        LM_E(("Error opening file:'%s' (%d of %d), errno:%s %d", file_vector_[file_index].name, file_index, num_files_,
              strerror(errno),
              errno));
        file_vector_[file_index].already_read = true;
        continue;
      }
    }
    file_vector_[file_index].fd = fileno(file_vector_[file_index].fp);

    size_t bytes_read_acum = 0;
    while (fgets(temporal_buffer, LOGSDATASET_LINE_MAX_LENGTH, file_vector_[file_index].fp) != NULL) {
      bytes_read_acum += strlen(temporal_buffer);
      *log = strdup(temporal_buffer);



      if (timestamp_position_ != -1) {
        std::vector<char *> fields = au::SplitInWords(temporal_buffer, separator_);

        if (static_cast<int>(fields.size()) != num_fields_) {
          free(*log);
          continue;
        }

        if (static_cast<int>(fields.size()) < (timestamp_position_ + 1)) {
          // LM_W(("Error reading line:'%s' from fd:%d file_index:%d (%s)", temporal_buffer, file_vector_[file_index].fd, file_index, file_vector_[file_index].name));
          free(*log);
          continue;
        }

        int temporal_position = 0;
        if (strlen(fields[timestamp_position_]) < 6) {
          if (strlen(fields[timestamp_position_alt_]) < 6) {
            // LM_W(("Error detecting timestamp in pos:%d(strlen:%d) and %d(strlen(%d) for line:%s", timestamp_position_, strlen(fields[timestamp_position_]), timestamp_position_alt_, strlen(fields[timestamp_position_alt_]), temporal_buffer));
            free(*log);
            *log = NULL;
            return false;
          } else {
            temporal_position = timestamp_position_alt_;
          }
        } else {
          temporal_position = timestamp_position_;
        }

        if (timestamp_type_ == 1) {
          *timestamp = GetTimeFromStrTimeDate_YYYY_mm_dd_24H(fields[temporal_position]);
        } else if (timestamp_type_ == 2) {
          *timestamp = GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[temporal_position]);
        }
      } else {
        *timestamp = -1;
      }

      if ((lseek(file_vector_[file_index].fd, -1 * bytes_read_acum, SEEK_CUR)) == -1) {
        LM_E(("Error rewinding file descriptor for file '%s'", file_vector_[file_index].name));
      }
      return true;
    }

    // If it arrives here, the input file has been finished
    // but now, we don't want to jump to another file
    if ((lseek(file_vector_[file_index].fd, -1 * bytes_read_acum, SEEK_CUR)) == -1) {
      LM_E(("Error rewinding file descriptor for file '%s'", file_vector_[file_index].name));
    }
  }
  LM_W(("End of files in directory '%s' reached after scanning %d valid files", dir_path_, num_files_));
  finished_ = true;
  return false;
}

