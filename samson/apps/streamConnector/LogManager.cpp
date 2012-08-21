
#include "LogManager.h"

namespace stream_connector {
Log::Log(std::string name, std::string type, std::string message) {
  time_ = ::time(NULL);
  type_ = type;
  name_ = name;
  message_ = message;
}

std::string Log::getNameAndMessage() {
  return au::str("%25s : %-s", name_.c_str(), message_.c_str());
}

std::string Log::getType() {
  return type_;
}

void Log::writeOnScreen() {
  if (type_ == "Warning")
    LM_W(( getNameAndMessage().c_str())); else if (type_ == "Error")
    LM_E(( getNameAndMessage().c_str())); else
    LM_M(( getNameAndMessage().c_str()));
}

LogManager::LogManager() : token_("LogManager") {
}

  void LogManager::log(au::SharedPointer<Log> log ) {
  au::TokenTaker tt(&token_);

  logs_.Push(log);
    logs_.LimitToLastItems(1000000);
}

au::tables::Table *LogManager::getLogsTable(size_t limit) {
  au::tables::Table *table = new au::tables::Table("Element|Time,left|Type,left|Description,left");

  table->setTitle("Logs");

  au::TokenTaker tt(&token_);

  std::vector<au::SharedPointer<Log> > logs = logs_.items();
  for (size_t i = 0 ; i< logs.size() ;i++ )
  {
    
    au::StringVector values;
    values.push_back(logs[i]->name() );

    values.push_back(au::str_timestamp(logs[i]->time()));

    values.push_back(logs[i]->type());
    values.push_back(logs[i]->message());

    table->addRow(values);
  }

  return table;
}
}