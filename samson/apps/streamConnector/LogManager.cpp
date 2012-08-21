
#include "LogManager.h"

namespace stream_connector {
Log::Log(std::string name, std::string type, std::string message) {
  time_ = time(NULL);
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

LogManager::LogManager() : token("LogManager") {
}

void LogManager::log(Log *l) {
  au::TokenTaker tt(&token);

  logs.push_back(l);

  while (logs.size() > 1000000) {
    Log *log = logs.front();
    delete log;
    logs.pop_front();
  }
}

au::tables::Table *LogManager::getLogsTable(size_t limit) {
  au::tables::Table *table = new au::tables::Table("Element|Time,left|Type,left|Description,left");

  table->setTitle("Logs");

  au::TokenTaker tt(&token);
  au::list<Log>::iterator it;

  for (it = logs.begin(); it != logs.end(); it++) {
    Log *log = *it;
    au::StringVector values;
    values.push_back(log->name_);

    values.push_back(au::str_timestamp(log->time_));

    values.push_back(log->type_);
    values.push_back(log->message_);

    table->addRow(values);
  }

  return table;
}
}