

#ifndef _H_AU_LOG_FILE
#define _H_AU_LOG_FILE

#include <string>
#include <vector>

#include "au/ErrorManager.h"
#include "au/Status.h"
#include "au/containers/SharedPointer.h"

namespace au {
class Log;

// Read a file of logs
std::vector< au::SharedPointer<Log> > readLogFile(std::string file_name, au::ErrorManager& error);
}

#endif
