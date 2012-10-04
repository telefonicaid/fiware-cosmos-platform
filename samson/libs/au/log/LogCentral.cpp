
#include "log.h"
#include "au/log/LogPlugin.h"
#include "au/log/LogPluginScreen.h"
#include "au/log/LogPluginServer.h"

#include "LogCentral.h" // Own interface

namespace au {
  
  // Global instance of LogCentral
  LogCentral log_central;
  
  
  void* run_LogAppServer( void* p )
  {
    LogCentral* log_central = (LogCentral*) p;
    log_central->run();
    return NULL;
  }
  
  LogCentral::LogCentral()
  {
    fds_[0] = -1;
    fds_[1] = -1;
    
    fd_read_logs_ = NULL;
    fd_write_logs_ = NULL;
    
    // Init plugins list
    for ( int i=0;i< AU_LOG_MAX_PLUGINS;i++)
      plugins[i] = NULL;
    
    screen_plugin_= new LogPluginScreen();
    file_plugin_= new LogPluginFile();
    server_plugin_= new LogPluginServer();

    AddPlugin(screen_plugin_);
    AddPlugin(file_plugin_);
    AddPlugin(server_plugin_);
    
  }
  
  
  void LogCentral::Init( std::string exec )
  {
    // keep the name of the executable
    exec_ = exec;
    
    if( fds_[0] != -1 )
    {
      // Already init
      return;
    }
    
    int r = pipe(fds_);
    if( r != 0 )
    {
      fprintf(stderr,"Not possible to create pipe for logs\n");
      exit(1);
    }
    
    // Create file descriptor to write logs
    fd_write_logs_ = new au::FileDescriptor( "fd for writting logs", fds_[1] );
    fd_read_logs_ = new au::FileDescriptor( "fd for reading logs", fds_[0] );
    
    // Create background process for logs
    au::ThreadManager* tm = au::Singleton<au::ThreadManager>::shared();
    tm->addThread("log_thread",&t_,NULL,run_LogAppServer,this);
  }
  
  void LogCentral::run()
  {
    // Background thread
    
    while( true )
    {
      LogPointer log( new Log() );
      bool real_log = log->Read(fd_read_logs_);
      if( !real_log )
        continue;
      
      // Additional information for logs
      log->Set("channel_name", log_channels_.channel_name(log->log_data().channel ));
      log->Set("channel_alias", log_channels_.channel_alias(log->log_data().channel ));
      log->Set("exec",  exec_ );
      
      // Emission cahnnel
      int channel = log->log_data().channel;
      
      for ( int i=0;i< AU_LOG_MAX_PLUGINS;i++)
        if( plugins[i] && plugins[i]->CheckLogChannel(channel) )
          if( plugins[i]->activated())
            plugins[i]->Emit(log);
    }
    
  }
  
  void LogCentral::Emit( Log* log )
  {
    // Write to the pipe
    log->Write(fd_write_logs_);
  }
  
  void LogCentral::AddPlugin(LogPlugin *log_plugin) {
    
    for ( int i=0;i< AU_LOG_MAX_PLUGINS;i++)
      if( plugins[i] == NULL)
      {
        plugins[i] = log_plugin;
        return;
      }
    // Log this is not possible? ;)
    delete log_plugin;
  }
  
  void LogCentral::evalCommand(std::string command)
  {
    au::ErrorManager error;
    evalCommand(command , error);
  }

  void LogCentral::evalCommand(std::string command , au::ErrorManager& error )
  {
    // Catalogue to parse input commands ( separated by commas )
    LogCentralCatalogue log_central_catalogue;
    
    std::vector<std::string> commands = au::split(command, ',');
    for ( size_t i = 0 ; i < commands.size() ; i++ )
    {
      au::ErrorManager parse_error;
      au::console::CommandInstance *command_instance = log_central_catalogue.parse(command, &parse_error);

      if( error.IsActivated())
      {
        error.set( "Log command error: " + parse_error.GetMessage() );
        return;
      }
      
      // "show_fields"
      if( command_instance->main_command() == "show_fields" )
      {
        error.AddMessage( getTableOfFields()->str() );
        return;
      }

      if( command_instance->main_command() == "show_plugins" )
      {
        
        au::tables::Table table("Name|Active|Info");

        for ( int i=0;i< AU_LOG_MAX_PLUGINS;i++)
          if( plugins[i] != NULL)
          {
            
            au::StringVector values;

            values.Push( plugins[i]->name() );
            if( plugins[i]->activated())
              values.Push( "YES" );
            else
              values.Push( "NO" );
            values.Push( plugins[i]->status() );
            
            table.addRow(values);
          }
        
        error.AddMessage(table.str());
                         
        return;
      }

      
      // screen command
      if( command_instance->main_command() == "screen" )
      {
        std::string command = command_instance->get_string_argument("command");
 
        if( command == "on")
          screen_plugin_->set_activated(true);
        else if( command == "off")
          screen_plugin_->set_activated(false);
        else
        {
          error.set(au::str("Unknown argument (%s) for command screen. Only on/off are valid vakuearguments", command.c_str()));
          return;
        }

        screen_plugin_->set_format( command_instance->get_string_argument("format") );
      }
      
      // screen command
      if( command_instance->main_command() == "file" )
      {
        std::string command = command_instance->get_string_argument("command");
        
        if( command == "on")
          file_plugin_->set_activated(true);
        else if( command == "off")
          file_plugin_->set_activated(false);
        else
        {
          error.set(au::str("Unknown argument (%s) for command file. Only on/off are valid vakuearguments", command.c_str()));
          return;
        }
        
        std::string file_name = command_instance->get_string_argument("file");
        if( file_name == "" )
          error.set("Missing file name in file command");
        file_plugin_->set_local_file( file_name );
      }

      if( command_instance->main_command() == "add")
      {
        std::string pattern = command_instance->get_string_argument("pattern");
        main_log_channel_filter_.Add( log_channels_.Get( pattern ) );
        
      }
      
      if( command_instance->main_command() == "remove")
      {
        std::string pattern = command_instance->get_string_argument("pattern");
        main_log_channel_filter_.Remove( log_channels_.Get( pattern ) );
        
      }

      if( command_instance->main_command() == "verbose")
      {
        main_log_channel_filter_.Add( AU_LOG_V );
      }
      if( command_instance->main_command() == "verbose2")
      {
        main_log_channel_filter_.Add( AU_LOG_V );
        main_log_channel_filter_.Add( AU_LOG_V2 );
      }
      if( command_instance->main_command() == "verbose3")
      {
        main_log_channel_filter_.Add( AU_LOG_V );
        main_log_channel_filter_.Add( AU_LOG_V2 );
        main_log_channel_filter_.Add( AU_LOG_V3 );
      }
      if( command_instance->main_command() == "verbose3")
      {
        main_log_channel_filter_.Add( AU_LOG_V );
        main_log_channel_filter_.Add( AU_LOG_V2 );
        main_log_channel_filter_.Add( AU_LOG_V3 );
        main_log_channel_filter_.Add( AU_LOG_V4 );
      }
      if( command_instance->main_command() == "verbose3")
      {
        main_log_channel_filter_.Add( AU_LOG_V );
        main_log_channel_filter_.Add( AU_LOG_V2 );
        main_log_channel_filter_.Add( AU_LOG_V3 );
        main_log_channel_filter_.Add( AU_LOG_V4 );
        main_log_channel_filter_.Add( AU_LOG_V4 );
      }


      
      
    }
  }


  
  
}