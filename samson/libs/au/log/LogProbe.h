
#ifndef _H_AU_LOG_PROBE
#define _H_AU_LOG_PROBE

#include "au/containers/SharedPointer.h"
#include "au/ErrorManager.h"
#include "au/log/Log.h"
#include "au/console/Console.h"
#include "au/log/LogFormatter.h"
#include "au/network/SocketConnection.h"

/*
 
 Local collector of logs
 
 Several plugins can be attached to process recevied logs
 - Printer: to display logs in std::cerr
 - Console: to display logs on a console
 - Count: To take statistics on collected logs
 
 
 */

namespace au {
  
  class LogProbePlugin
  {
  public:
    virtual ~LogProbePlugin(){};
    virtual void Process( au::LogPointer log )=0;
    virtual void ProceassFinish(){};
  };
  
  class LogProbe
  {
    
  public:
    
    LogProbe(){
    }
    
    ~LogProbe() {
      Clear();
    }
    
    // Disconnect from any source and remove all plugins
    void Clear();

    // Connection management
    void ConnectAsProbe( const std::string& host , const std::string& filter , au::ErrorManager& error );
    void ConnectAsQuery( const std::string& host , const std::string& filter , bool count , au::ErrorManager& error );
    bool IsConnected();
    void Disconnect();
    
    // Error management
    au::ErrorManager& error(){
      return  error_;
    }

    // Process received logs with all provided plugins
    void Process( au::SharedPointer<au::Log> log );

    // Plugins management
    void AddPlugin( const std::string name , LogProbePlugin* plugin );
    void AddFilePlugin( const std::string& plugin_name , const std::string& file_name, au::ErrorManager &error );
    
  private:
    
    friend void* RunLogProbe( void* p );
    void Run();
    
    au::SharedPointer<au::SocketConnection> socket_connection_; // Socket connection to connect as probe or query
    std::string host_;
    std::string filter_;
    pthread_t thread_id_;
    au::ErrorManager error_; // Internal error to get why we quit
    au::map<std::string,LogProbePlugin> plugins_; // List of plugins to process income logs
    
  };
  
  /*
   LogProbeCounter 
   
   Plugin to take statistics
   
   */
  
  class LogProbeCounter : public LogProbePlugin
  {

  public:
    
    LogProbeCounter( ){
    }
    
    virtual void Process( au::SharedPointer<au::Log> log )
    {
      log_counter_.Process(log);
    }
    
    virtual void ProceassFinish()
    {
      // Print out on screen
      std::cerr << log_counter_.str_global();
    }
    
  private:
    au::LogCounter log_counter_;
    
  };
  
  /*
   LogProbePriter
   Plugin to print logs on screen
   */
  
  class LogProbePriter : public LogProbePlugin
  {
    
  public:
    
    LogProbePriter( const std::string& format , bool colored , bool output_stderr  ) : log_formatter_(format , colored )
    {
      output_stderr_ = output_stderr;
    }
    
    virtual void Process( au::SharedPointer<au::Log> log )
    {
      if ( output_stderr_ )
        std::cerr << log_formatter_.get(log) << "\n";
      else
        std::cout << log_formatter_.get(log) << "\n";
    }
    
  private:
    
    au::LogFormatter log_formatter_;
    bool output_stderr_;
    
  };
  
  /*
   LogProbeConsole
   Plugin to print logs on a console
   
   */
  
  class LogProbeConsole : public LogProbePlugin
  {
    
  public:
    
    LogProbeConsole( Console * console , const std::string& format )
    : log_formatter_(format)
    {
      console_ = console;
    }
    
    virtual void Process( au::SharedPointer<au::Log> log )
    {
      console_->writeOnConsole( log_formatter_.get(log) + "\n"  );
    }

  private:

    Console * console_;
    au::LogFormatter log_formatter_;
  };
  
  
  
  /*
   LogProbeFileDescriptor
   Plugin to save content on a file
   */
  
  class LogProbeFileDescriptor : public LogProbePlugin
  {
    
  public:
    
    LogProbeFileDescriptor( int fd )
    {
      file_descriptor_ = new au::FileDescriptor("log_save" , fd );
    }
    
    ~LogProbeFileDescriptor()
    {
      delete file_descriptor_;
    }

    virtual void Process( au::SharedPointer<au::Log> log )
    {
      if( file_descriptor_)
        log->Write(file_descriptor_);
    }
    
    au::ErrorManager& error()
    {
      return error_;
    }
    
    private:
      
    au::ErrorManager error_;
    au::FileDescriptor* file_descriptor_;
    
  };
    
}

#endif
