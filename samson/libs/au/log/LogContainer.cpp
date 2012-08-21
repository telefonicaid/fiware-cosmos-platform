

#include "LogContainer.h" // Own interface

namespace au {

  LogContainer::LogContainer() : token_("LogContainer") {
    max_num_logs_  = 100000000;    // Limit of logs
  }
  
  void LogContainer::Push( au::SharedPointer<Log> log) {
    // Put in the list & check not too many lgos are inside
    au::TokenTaker tt(&token_);
    logs_.Push(log);
    logs_.LimitToLastItems(max_num_logs_);
  }
  
  void LogContainer::Clear() {
    au::TokenTaker tt(&token_);
    logs_.Clear();
  }
  
  size_t LogContainer::size() const {
    au::TokenTaker tt(&token_);
    return logs_.size();
  }
  
  std::string LogContainer::getInfo() const {
    
    // Accumulated information
    au::Descriptors types;
    au::Descriptors channels;
    size_t size=0;
    
    au::TokenTaker tt(&token_);
    std::vector< SharedPointer<Log> > logs = logs_.items();
    for (size_t i = 0 ; i < logs.size() ;i++ ) {
      types.Add(logs[i]->Get("TYPE"));
      channels.Add(logs[i]->Get("channel"));
      size += logs[i]->SerialitzationSize();
    }
    
    au::tables::Table table("Concept|Value,left");
    table.setTitle("Log info");
    
    // Number of logs
    {
      au::StringVector values;
      values.Push("Number of logs");
      values.Push(au::str( logs_.size() ) + " / " + au::str(max_num_logs_));
      table.addRow(values);
    }
    
    // Size of logs
    {
      au::StringVector values;
      values.Push("Size");
      values.Push(au::str(size, "B"));
      table.addRow(values);
    }
    
    // Type of logs
    {
      au::StringVector values;
      values.Push("Types");
      values.Push(types.str());
      table.addRow(values);
    }
    
    // Channels
    {
      au::StringVector values;
      values.Push("Channels");
      size_t num_channels = channels.size();
      if (num_channels < 5)
        values.Push(channels.str());
      else
        values.Push(au::str("%lu channels", num_channels));
      table.addRow(values);
    }
    
    if (logs.size() > 0) {
      table.addRow(au::StringVector("From", (*logs.begin())->Get("timestamp")));
      table.addRow(au::StringVector("To", (*logs.rbegin())->Get("timestamp")));
    }
    
    return table.str();
  }
  
  std::vector< au::SharedPointer<Log> > LogContainer::logs()
  {
    au::TokenTaker tt(&token_);
    return logs_.items();
  }

}