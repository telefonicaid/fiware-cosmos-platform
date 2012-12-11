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


#ifndef _H_LOGSDATASET
#define _H_LOGSDATASET

#include <time.h>
#include <vector>

#include "au/log/LogMain.h"
#include "logMsg/logMsg.h"

/* ****************************************************************************
 *
 * File -
 */
typedef struct File {
  char name[256];
  time_t date;
  uint64_t size;
  int fd;
  FILE *fp;
  bool already_read;
} File;

char *fgetsFromFd(char *str, int line_max_size, int fd);
time_t GetTimeUTCFromCalendar(struct tm *tm);
time_t GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(const char *strTimeDate);
time_t GetTimeFromStrTimeDate_YYYY_mm_dd_24H(const char *strTimeDate);
char *ctimeUTC(time_t *timestamp);



class LogsDataSet {
  char *dir_path_;
  char *extension_;
  int num_files_;
  File *file_vector_;
  int num_fields_;
  int timestamp_position_;
  int timestamp_position_alt_;
  char separator_;
  int timestamp_type_;
  time_t first_timestamp_;
  char *queue_name_;
  bool finished_;

public:
  LogsDataSet(const char *dir_path, const char *extension, int num_fields, int timestamp_position,
              int timestamp_position_alt,
              int timestamp_type,
              const char *queue_name);
  ~LogsDataSet();

  bool InitDir();
  bool GetLogLineEntry(char **log, time_t *timestamp);
  bool LookAtNextLogLineEntry(char **log, time_t *timestamp);
  bool Synchronize(time_t time_init);

  bool finished() const {
    return finished_;
  }

  time_t GetFirstTimestamp() {
    LOG_SM(("For queue:%s, Get first_timestamp:%s", queue_name_, ctimeUTC(&first_timestamp_))); return first_timestamp_;
  };
  void SetFirstTimestamp(time_t value) {
    first_timestamp_ = value; LOG_SM(("For queue:%s, Set first_timestamp:%s", queue_name_, ctimeUTC(&first_timestamp_)));
  };

  const char *GetQueueName() {
    return queue_name_;
  };
};

#endif  // ifndef _H_LOGSDATASET
