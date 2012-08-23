

#ifndef _H_AU_LOG_PLUGIN
#define _H_AU_LOG_PLUGIN

#include "au/containers/SharedPointer.h"
#include "au/log/Log.h"

namespace au {
class LogPlugin {
  // TODO: Allow to filter logs to be processed
public:
  void virtual process_log(au::SharedPointer<Log> log) = 0;
};
}

#endif