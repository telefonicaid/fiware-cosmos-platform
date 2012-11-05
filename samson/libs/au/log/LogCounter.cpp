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

#include "au/log/LogCounter.h"  // Own interface

#include "au/log/LogCentral.h"

namespace au {
void LogCounter::Process(LogPointer log) {

  // Size of the log in bytes
  size_t size = log->SerialitzationSize();

  rate_.Push(size);     // Global rate counter

  // Rate per channel channel
  au::rate::Rate *rate = rates.findOrCreate(log->channel());
  rate->Push(size);     // Individual rate counter
  
  // Type of messages per channel
  types_per_channel_.findOrCreate(log->channel())->Add(Log::GetLogLevel(log->level()));

  // General descriptor
  descriptors_.findOrCreate("type")->Add( log->Get("type") );
  descriptors_.findOrCreate("channels")->Add( log->Get("channel_name") );
  descriptors_.findOrCreate("pids")->Add( log->Get("pid") );
  descriptors_.findOrCreate("tids")->Add( log->Get("tids") );
  descriptors_.findOrCreate("execs")->Add( log->Get("exec") );
  descriptors_.findOrCreate("hosts")->Add( log->Get("host") );
  descriptors_.findOrCreate("dates")->Add( log->Get("date") );
  descriptors_.findOrCreate("files")->Add( log->Get("files") );
  descriptors_.findOrCreate("functions")->Add( log->Get("function") );
 
  // Min-max time
  time_t time = log->log_data().tv.tv_sec;
  if( ( min_time_ == (time_t) -1 ) || ( time < min_time_ ) )
    min_time_ = time;
  if( ( max_time_ == (time_t) -1 ) || ( time > max_time_ ) )
    max_time_ = time;
  
  std::string channel = log->Get("channel");
  logs_per_channel_.findOrCreate(channel)->Process(log);
}
 
  std::string LogCounter::GetChannelsTable() const
  {
    au::tables::Table table("Channel|#logs|size|Time|Types");
    table.setTitle("Log info");
    
    au::map<std::string,SimpleLogCounter>::const_iterator iter;
    for ( iter = logs_per_channel_.begin() ; iter != logs_per_channel_.end() ; iter++ )
    {
      au::StringVector values;
      
      values.Push( iter->first );
      values.Push( au::str(iter->second->num()) );
      values.Push( au::str(iter->second->size(),"B"));
      values.Push( iter->second->GetTimeRange() );
      values.Push( iter->second->GetTypes() );
      
      table.addRow(values);
      
    }


    return table.str();
  }

  
  std::string LogCounter::GetInfoTable() const {
    
    au::tables::Table table("Concept|Value,left");
    table.setTitle("Log info");
    
    // Number of logs
    {
      au::StringVector values;
      values.Push("Number of logs");
      values.Push(au::str( rate_.hits() ) );
      table.addRow(values);
    }
    
    // Size of logs
    {
      au::StringVector values;
      values.Push("Size");
      values.Push(au::str(rate_.size(), "B"));
      table.addRow(values);
    }
    
    // All collected info
    au::map<std::string,au::Descriptors>::const_iterator it;
    for ( it = descriptors_.begin() ; it != descriptors_.end() ; ++it )
    {
      au::StringVector values;
      values.Push(it->first);
      values.Push(it->second->str());
      table.addRow(values);
    }

    if ( min_time_ != (time_t) -1)
    {
      au::StringVector values;
      values.Push("Time range");
      values.Push( au::GetTimeStampString(min_time_) + " " + au::GetTimeStampString(max_time_) );
      table.addRow(values);
    }
    
    return table.str();
  }
  
  std::string LogCounter::str_global() const
  {
    std::ostringstream output;
    output << "Total logs: " << rate_.hits() << " " << au::str( rate_.hits()) << std::endl;
    au::map<std::string,au::Descriptors>::const_iterator it;
    for ( it = descriptors_.begin() ; it != descriptors_.end() ; ++it )
    {
      output << it->first << ": " << it->second->str() << "\n";
    }

    return output.str();
  }
  
std::string LogCounter::str(int c) const {
  au::rate::Rate *rate = rates.findInMap(c);

  if (!rate) {
    return "";
  } else {
    return au::str("%s %s"
                   , au::str(rate->hits() , "logs").c_str()
                   , au::str(rate->size(), "B").c_str()
                   );
  }
}

std::string LogCounter::str_rate(int c) const {
  au::rate::Rate *rate = rates.findInMap(c);
  if (!rate) {
    return "";
  } else {
    return au::str("%s %s"
                   , au::str(rate->hit_rate() , "logs/s").c_str()
                   , au::str(rate->rate(), "B/s").c_str()
                   );
  }
}

std::string LogCounter::str_types_per_channel( int c ) const {
    au::Descriptors *my_descriptors = types_per_channel_.findInMap(c);
    if (!my_descriptors) {
      return "";
    }
    else
      return my_descriptors->str();
  }
  
  
  void LogCounter::Clear()
  {
    min_time_ = (size_t) -1;
    max_time_ = (size_t) -1;
    descriptors_.clearMap();
    rates.clearMap();
    types_per_channel_.clearMap();
    rates.clearMap();
    rate_.Clear();
    logs_per_channel_.clearMap();
    
  }

size_t LogCounter::logs() const {
  return rate_.hits();
}

size_t LogCounter::size() const {
  return rate_.size();
}

size_t LogCounter::rate() const {
  return rate_.rate();
}
}