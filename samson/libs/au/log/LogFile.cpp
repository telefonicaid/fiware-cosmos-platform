

#include "Log.h"
#include "LogFile.h"  // Own interface

#include "logMsg/logMsg.h"                 // LM_T
#include "logMsg/traceLevels.h"            // LmtOtherFileDescriptors, etc.


namespace au {
  std::vector< au::SharedPointer<Log> > readLogFile(std::string file_name, au::ErrorManager& error )
  {
    std::vector< au::SharedPointer<Log> > logs;
  int fd = open(file_name.c_str(), O_RDONLY);

  LM_LT(LmtFileDescriptors, ("Open FileDescriptor fd:%d", fd));
  if (fd < 0)
  {
    error.set( au::str( "Not possible to open file %s" , file_name.c_str() ) );
    return logs;
  }

  // File descriptor to read logs
  FileDescriptor file_descriptor("reading log file", fd);

  while (true) {
    au::SharedPointer<Log> log( new Log() );
    if (log->Read(&file_descriptor))
      logs.push_back(log);
    else
      break;
  }

    return logs;
  }
}
