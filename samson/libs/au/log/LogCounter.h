
#ifndef _H_AU_LOG_COUNTER
#define _H_AU_LOG_COUNTER

#include "au/statistics/Rate.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/map.h"
#include "au/log/Log.h"
#include "au/time.h"
#include "au/log/LogFormatter.h"
#include "au/string/Descriptors.h"


/*

 LogCounter
 
 Class to take a lot of statistics about incomming logs
 No log is keep in memory for this
 
 */


namespace au {
  
  class SimpleLogCounter
  {
    
  public:

    SimpleLogCounter()
    {
      num_=0;
      size_=0;
      min_time = ( time_t) -1;
      max_time = ( time_t) -1;
    }
    
    void Process( au::LogPointer log )
    {
      num_++;
      size_ += log->SerialitzationSize();
      types_.Add(log->Get("type"));
      // Min-max time
      time_t time = log->log_data().tv.tv_sec;
      if( ( min_time == (time_t) -1 ) || ( time < min_time ) )
        min_time = time;
      if( ( max_time == (time_t) -1 ) || ( time > max_time ) )
        max_time = time;
    }
    
    std::string GetTimeRange()
    {
      if( min_time == (time_t) -1 )
        return "-";
      return au::GetTimeStampString( min_time ) + " " + au::GetTimeStampString(max_time);
    }
    
    std::string GetTypes()
    {
      return types_.str();
    }
    
    size_t num()
    {
      return num_;
    }
    
    size_t size()
    {
      return size_;
    }
    
  private:
    
    size_t num_;
    size_t size_;
    au::Descriptors types_;
    time_t min_time;
    time_t max_time;
    
    
  };
  
  
class LogCounter {
public:

  LogCounter()
  {
    Clear();
  }
  
  // Process a new log to count
  void Process(LogPointer log);

  // Clear information
  void Clear();
  
  // Accessors
  std::string str(int c) const;
  std::string str_rate(int c) const;
  std::string str_types_per_channel(int c) const;
  std::string str_global() const;
  std::string GetInfoTable() const;
  std::string GetChannelsTable() const;
  size_t logs() const;
  size_t size() const;
  size_t rate() const;

private:

  au::rate::Rate rate_;                                 // total rate of logs/second
  au::map<int, au::rate::Rate > rates;                  // Rate per channel
  au::map<int, au::Descriptors > types_per_channel_;  // Type of message per channel

  // global descriptor information
  au::map<std::string,au::Descriptors> descriptors_;

  // Info for channels
  au::map<std::string,SimpleLogCounter> logs_per_channel_;
  
  // Min and max observed time-stamps
  time_t min_time_;
  time_t max_time_;
  
};
  
}

#endif  // ifndef _H_AU_LOG_COUNTER