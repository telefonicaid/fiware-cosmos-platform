

#include "au/ThreadManager.h"
#include "au/network/SocketConnection.h"

#include "SamsonPushLogsConnectionsManager.h"  // Own interface
#include "samson/client/SamsonClient.h"         // samson::SamsonClient
#include "samson/client/SamsonPushBuffer.h"


void *RunSamsonPushLogsConnection(void *p) {
  SamsonPushLogsConnection *connection = ( SamsonPushLogsConnection * )p;

  connection->Run();
  return NULL;
}

SamsonPushLogsConnection::SamsonPushLogsConnection(LogsDataSet *dataset, const char *queue_name, float ntimes_real_time,
                                                   samson::SamsonClient *samson_client) {
  dataset_ = dataset;
  queue_name_ = strdup(queue_name);
  ntimes_real_time_ = ntimes_real_time;

  thread_running_ = true;

  // Create the push buffer to send data
  pushBuffer_ = new samson::SamsonPushBuffer(samson_client, queue_name_);

  // Create the thread
  pthread_t t;
  au::ThreadManager::shared()->addThread("SamsonPushLogsConnection", &t, NULL, RunSamsonPushLogsConnection, this);
}

SamsonPushLogsConnection::~SamsonPushLogsConnection() {
  delete dataset_;
  free(queue_name_);
  delete pushBuffer_;
}

void SamsonPushLogsConnection::Run() {
  //  if (dataset_->InitDir() == false)
  //  {
  //    thread_running_ = false;
  //    return;
  //  }

  au::Cronometer cronometer;

  time_t first_timestamp = dataset_->GetFirstTimestamp();
  char *time_init_str = strdup(ctimeUTC(&first_timestamp));

  time_init_str[strlen(time_init_str) - 1] = '\0';
  LM_M(("Checking %s with first_timestamp:%s", dataset_->GetQueueName(), time_init_str));

  int count_lines = 0;
  int count_skip  = 0;
  bool first_run = true;
  while (true) {
    char *log_line;
    time_t timestamp;

    if ((dataset_->GetLogLineEntry(&log_line, &timestamp)) == false) {
      // LM_W(("Skipping wrong entry in dataset: %s", dataset_->GetQueueName()));
      if (dataset_->finished()) {
        thread_running_ = false;
        free(time_init_str);
        return;
      }
      continue;
    }

    char *time_read_str = strdup(ctimeUTC(&timestamp));
    if (*time_read_str == '\0') {
      free(time_read_str);
      continue;
    }
    time_read_str[strlen(time_read_str) - 1] = '\0';


    if ((first_run) & (timestamp < first_timestamp)) {
      if (count_skip % 100000000 == 0)
        LM_M(("Skipping %s with read_timestamp:%s and first_timestamp:%s", dataset_->GetQueueName(), time_read_str,
              time_init_str)); free(time_read_str);
      free(log_line);
      count_skip++;
      continue;
    } else {
      first_run = false;
    }

    int count_sleeps = 0;
    bool first_sleep = true;
    while (timestamp > first_timestamp + ntimes_real_time_ * cronometer.seconds()) {
      if ((count_lines % 1000 == 0) && (count_sleeps % 100000 == 0))
        LM_M(("Sleeping %s with timestamp:%s, elapsed_time:%lf(%d%% realtime) since %s", dataset_->GetQueueName(),
              time_read_str,
              ntimes_real_time_ * cronometer.seconds(), int(100 * ntimes_real_time_), time_init_str));
      if ((count_sleeps % 10000 == 0) &&
          (timestamp > (first_timestamp + ntimes_real_time_ * cronometer.seconds() + 900))) {
        LM_M(("An inactivity long gap for %s with timestamp:%s, elapsed_time:%lf(%d%% realtime) since %s",
              dataset_->GetQueueName(),
              time_read_str, ntimes_real_time_ * cronometer.seconds(), int(100 * ntimes_real_time_), time_init_str));  // sleeps for 10 milliseconds
      }
      usleep(10000);
      count_sleeps++;
      if (first_sleep == true)
        pushBuffer_->flush(); first_sleep = false;
    }

    if (count_lines % 100000000 == 0) {
      LM_M(("Pushing with time:%s (first:%s, elapsed:%lf(%d%% real_time)) to SAMSON from dataset:%s", time_read_str,
            time_init_str,
            ntimes_real_time_ * cronometer.seconds(), int(100 * ntimes_real_time_), dataset_->GetQueueName()));  // Pushing this log to SAMSON system
    }
    pushBuffer_->push(log_line, strlen(log_line), false);
    free(log_line);
    free(time_read_str);
    count_lines++;
  }
  free(time_init_str);
}

