
#ifndef _H_AU_LOG_PLUGIN_SCREEN
#define _H_AU_LOG_PLUGIN_SCREEN

#include "au/containers/SharedPointer.h"
#include "au/log/Log.h"
#include "au/log/LogChannelFilter.h"
#include "au/log/LogFormatter.h"
#include "au/log/LogPlugin.h"

namespace au {
  
  class LogPluginScreen : public LogPlugin
  {
    
  public:
    LogPluginScreen( const std::string& format_definition = AU_LOG_DEFAULT_FORMAT , bool output_stderror = false )
    : LogPlugin("Screen")
    , log_formatter_( format_definition )
    {
      output_stderror_ = output_stderror;
    }
    
    virtual void Emit(au::SharedPointer<Log> log)
    {
      if( output_stderror_)
        fprintf(stdout, "%s\n" , log_formatter_.get(log).c_str());
      else
        fprintf(stderr, "%s\n" , log_formatter_.get(log).c_str());
    }
    
    virtual std::string status()
    {
      std::ostringstream output;
      if( output_stderror_)
        output << "[STDERR]";
      output << log_formatter_.definition();
      return output.str();
    }
    
    void set_format( const std::string format)
    {
      log_formatter_.set_format( format );
    }
    
  private:
    bool output_stderror_;
    LogFormatter log_formatter_;
    
  };
  
}

#endif