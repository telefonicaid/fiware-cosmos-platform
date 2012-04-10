

#include "au/ThreadManager.h"
#include "au/network/SocketConnection.h"

#include "SamsonPushLogsConnectionsManager.h" // Own interface
#include "samson/client/SamsonPushBuffer.h"
#include "samson/client/SamsonClient.h"         // samson::SamsonClient


extern size_t buffer_size;
extern samson::SamsonClient *samson_client;
extern char queue_name[1024];

void* RunSamsonPushLogsConnection( void* p)
    {
  SamsonPushLogsConnection* connection = ( SamsonPushLogsConnection* ) p;
  connection->Run();
  return NULL;
    }


SamsonPushLogsConnection::SamsonPushLogsConnection( LogsDataSet *dataset, const char *queue_name, float ntimes_real_time, samson::SamsonClient *samson_client)
{
  dataset_ = dataset;
  queue_name_ = strdup(queue_name);
  ntimes_real_time_ = ntimes_real_time;

  thread_running_ = true;

  // Create the push buffer to send data
  pushBuffer_ = new samson::SamsonPushBuffer( samson_client , queue_name_ );

  // Create the thread
  pthread_t t;
  au::ThreadManager::shared()->addThread("SamsonPushLogsConnection",&t, NULL, RunSamsonPushLogsConnection, this);
}

SamsonPushLogsConnection::~SamsonPushLogsConnection()
{
  delete dataset_;
  free (queue_name_);
  delete pushBuffer_;
}


void SamsonPushLogsConnection::Run()
{
  if (dataset_->InitDir() == false)
  {
    thread_running_ = false;
    return;
  }

  au::Cronometer cronometer;

  while( true )
  {
    char *log_line;
    time_t timestamp;

    time_t first_timestamp = dataset_->GetFirstTimestamp();

    if ((dataset_->GetLogLineEntry(&log_line, &timestamp)) == false)
    {
      thread_running_ = false;
      return;
    }

    while (timestamp > first_timestamp + ntimes_real_time_ * cronometer.diffTime())
    {
      // sleeps for 10 milliseconds
      usleep(10000);
    }

    // Pushing this log to SAMSON system
    pushBuffer_->push( log_line , strlen(log_line) , true );
    free(log_line);
  }
}
