

#include "LogClient.h"  // Own interface

namespace au {
/*
 * LogClient::LogClient()
 * {
 *  // No init by default...
 *  socket_connection = NULL;
 * }
 *
 * LogClient::~LogClient()
 * {
 *  if( socket_connection )
 *  {
 *      socket_connection->close();
 *      delete socket_connection;
 *  }
 * }
 *
 * void LogClient::connect( std::string _host , int _port , au::ErrorManager * error )
 * {
 *
 *  if( socket_connection )
 *  {
 *      error->set( au::str("Already connected with logServer at %s" , socket_connection->host_and_port().c_str() ) );
 *      return;
 *  }
 *
 *  host = _host;
 *  port = _port;
 *
 *  au::Status s = au::SocketConnection::Create(host, port, &socket_connection );
 *  if (s != au::OK )
 *      error->set( au::str("No possible to connect with log server at %s:%d (%s)" , host.c_str() ,  port , au::status(s)  ) );
 *
 * }
 *
 * void LogClient::closeConnection( ErrorManager * error )
 * {
 *  if( socket_connection )
 *  {
 *      delete socket_connection;
 *      socket_connection = NULL;
 *  }
 *  else
 *  {
 *      if( error )
 *          error->set("Not connected to any logServer");
 *  }
 * }
 *
 *
 * // au::Console
 * std::string LogClient::getPrompt()
 * {
 *  if( socket_connection )
 *      return au::str("logClient [%s]> " , socket_connection->host_and_port().c_str() );
 *  else
 *      return "logClient [ Disconnected ]> ";
 * }
 *
 * void LogClient::sent_command( std::string command , au::ErrorManager * error )
 * {
 *
 *  if( !socket_connection )
 *  {
 *      error->set("Not conneted with any log server");
 *      return;
 *  }
 *
 *  // Take the command and send to the socket
 *  command.append("\n");
 *  au::Status s = socket_connection->writeLine(command.c_str(), 100, 1, 0);
 *  if( s != au::OK )
 *  {
 *      closeConnection( NULL);
 *      error->set( au::str( "Error sending command %s" , command.c_str() ) );
 *  }
 * }
 *
 *
 * std::string LogClient::getMessageFromLogServer( au::ErrorManager * error )
 * {
 *  if( !socket_connection )
 *  {
 *      error->set("Not conneted with any log server");
 *      return "";
 *  }
 *
 *  // Read answer
 *  char header[1024];
 *  char* message = (char*) malloc( 1000000 );
 *
 *  // Read header of answer
 *  au::Status s = socket_connection->readLine(header, sizeof(header), 100);
 *
 *  if( s != OK )
 *  {
 *      error->set( str("Error reading header from logServer" ) );
 *      closeConnection(NULL);
 *      return "";
 *  }
 *
 *  // Remove returns at the end of the command
 *  remove_return_chars( header );
 *
 *  // Check protocol
 *  au::CommandLine cmdLine;
 *  cmdLine.parse(header);
 *
 *  if( cmdLine.get_num_arguments() != 2 )
 *  {
 *      error->set("Error connecting with log server (Wrong header)" );
 *      closeConnection(NULL);
 *      return "";
 *  }
 *  if( cmdLine.get_argument(0) != "LogServer")
 *  {
 *      error->set("Error connecting with log server (Wrong header)" );
 *      closeConnection(NULL);
 *      return "";
 *  }
 *
 *  // Get length from the header
 *  size_t message_size = atoll( cmdLine.get_argument(1).c_str() );
 *  if ( message_size > 1000000 )
 *  {
 *      error->set(au::str("Error connecting with log server (Excesive message size %s)" , au::str(message_size).c_str() ) );
 *      closeConnection(NULL);
 *      return "";
 *  }
 *
 *  // Read the rest of the message
 *  //writeWarningOnConsole(au::str("Reading message of %lu bytes" , message_size ));
 *  s = socket_connection->partRead(message, message_size, "LogServer message", 300);
 *
 *  if( s != OK )
 *  {
 *      error->set(au::str("Error connecting with log server (%s)" , au::status(s) ));
 *      closeConnection(NULL);
 *      return "";
 *  }
 *
 *  // End of message
 *  message[message_size] = '\0';
 *
 *  // Return generated message
 *  return message;
 *
 * }
 *
 * void LogClient::evalCommand( std::string command )
 * {
 *  if( command == "quit" )
 *  {
 *      quitConsole();
 *      return;
 *  }
 *
 *  evalCommand( command , this );
 * }
 *
 *
 * // Generic evalCommand to work inside another console ( delilah )
 * void LogClient::evalCommand( std::string command , Console * console )
 * {
 *  if( command.length() == 0 )
 *      return; // Nothing to do if it just press return
 *
 *  // Spetial commands first
 *  au::CommandLine cmdLine;
 *
 *  cmdLine.parse( command );
 *  if( cmdLine.get_num_arguments() == 0 )
 *      return; //Nothing to be processes
 *
 *  std::string  main_command = cmdLine.get_argument(0);
 *
 *  if( main_command == "help" )
 *  {
 *
 *      std::string message =
 *      "---------------------------------------------\n"\
 *      "Help logClient                               \n"\
 *      "------------------------------------------------------------------------------------------------------------------\n"\
 *      "logClient is the console client for a logServer where traces from different systems are collected.\n"\
 *      "\n \n"\
 *      "Main commands:\n"\
 *      "\n \n"\
 *      " * connect host: Connect to a logServer locate at provided host\n"\
 *      "\n \n"\
 *      " * disconnect: Disconnect from a logServer\n"\
 *      "\n \n"\
 *      " * show: Show logs on screen\n"\
 *      "\n \n"\
 *      "        [-format str_format]   Define format of how logs are displayed on screes   \n"\
 *      "        [-limit N]             Define the maximum number of logs to be displayed ( default 10000 ) \n"\
 *      "        [-type T]              Show only logs of a certain type: W M T X V ...\n"\
 *      "        [-time HH:MM::SS]      Show only logs generated before given time stamp\n"\
 *      "        [-date DD/MM/YY]       Show only logs generated before given date\n"\
 *      "        [-pattern str_pattern] Show only logs that match a particular regular experssion\n"\
 *      "        [-reverse]             Show records in reverse order\n"\
 *      "        [-multi_session]       Show logs from any session\n"\
 *      "        [-table]               Show records in a table instead on line by line\n"\
 *      "\n \n"\
 *      " * show_connections: Show current connections with this logServer"\
 *      "\n \n"\
 *      " * show_format_fiels : Show available format fields to be used in show command\n"\
 *      "\n\n"\
 *      " * new_session: Create a mark in the logs, so future show commands only show logs starting here.\n"\
 *      "\n \n"\
 *      "------------------------------------------------------------------------------------------------------------------\n"\
 *      "";
 *
 *      console->writeOnConsole( message );
 *
 *      return;
 *  }
 *
 *  if( main_command == "show_format_fiels" )
 *  {
 *      au::tables::Table table( "Field|Description,left" );
 *
 *      table.addRow( au::StringVector("HOST","Host where trace was generated") );
 *      table.addRow( au::StringVector("TYPE","Type of trace: Warning, Error, Message") );
 *      table.addRow( au::StringVector("PID","Process identifier of the executable that generated the trace") );
 *      table.addRow( au::StringVector("TID","Thread identifier of the executable that generated the trace") );
 *
 *      table.addRow( au::StringVector("date","Date when trace was generated") );
 *      table.addRow( au::StringVector("DATE","More verbose date") );
 *
 *      table.addRow( au::StringVector("time","Timestamp when trace was generated") );
 *      table.addRow( au::StringVector("TIME","More verbose timestamp") );
 *      table.addRow( au::StringVector("unix_time","Timestamp in seconds since epoc 1 January 1970") );
 *
 *      table.addRow( au::StringVector("FILE","Source file where trace was generated") );
 *      table.addRow( au::StringVector("LINE","Line of code in the source file") );
 *
 *      table.addRow( au::StringVector("TLEV","Trace level ( if the it is a trace )") );
 *      table.addRow( au::StringVector("EXEC","Name of the executable that generated the trace") );
 *      table.addRow( au::StringVector("AUX","Completementary name of the executable") );
 *      table.addRow( au::StringVector("TEXT","Body of the trace") );
 *      table.addRow( au::StringVector("text","Body of the trace limited to 100 characters") );
 *      table.addRow( au::StringVector("FUNC","Name of the funciton where the trace was generated") );
 *      table.addRow( au::StringVector("STRE","Error description") );
 *
 *      console->writeOnConsole( table.str() );
 *  }
 *
 *
 *  if( main_command == "status" )
 *  {
 *      if( socket_connection )
 *          writeWarningOnConsole( au::str("Connected to %s" , socket_connection->host_and_port().c_str() ) );
 *      else
 *          writeWarningOnConsole("Not connected to any logServer" );
 *      return;
 *  }
 *
 *  if (main_command == "connect")
 *  {
 *      if( cmdLine.get_num_arguments() < 2 )
 *      {
 *          console->writeErrorOnConsole( "Usage: connect host" );
 *      }
 *      else
 *      {
 *          ErrorManager error;
 *          std::string host = cmdLine.get_argument(1);
 *          connect(host, LOG_SERVER_DEFAULT_QUERY_CHANNEL_PORT  , &error);
 *
 *          if( error.IsActivated() )
 *              console->writeErrorOnConsole( error.GetMessage() );
 *          else
 *              console->writeWarningOnConsole( str("Connected to %s" , socket_connection->host_and_port().c_str() ));
 *
 *      }
 *
 *      return;
 *  }
 *
 *  if (main_command == "disconnect")
 *  {
 *      ErrorManager error;
 *      closeConnection( &error );
 *
 *      if( error.IsActivated() )
 *          console->writeErrorOnConsole( error.GetMessage() );
 *      else
 *          console->writeWarningOnConsole("Disconnected");
 *
 *      return;
 *  }
 *
 *
 *  au::ErrorManager error;
 *  sent_command( command , &error );
 *
 *  if( error.IsActivated() )
 *  {
 *      console->writeErrorOnConsole( error.GetMessage() );
 *      return;
 *  }
 *
 *  // Read answer message and write on screen
 *  std::string message = getMessageFromLogServer( &error );
 *
 *  if( error.IsActivated() )
 *  {
 *      console->writeErrorOnConsole( error.GetMessage() );
 *      return;
 *  }
 *
 *  // Write answer on screen
 *  console->writeOnConsole( message );
 *
 * }
 *
 * void LogClient::autoComplete( au::ConsoleAutoComplete* info )
 * {
 *  if( info->completingFirstWord() )
 *  {
 *      info->add("help");
 *      info->add("status");
 *      info->add("connect");
 *      info->add("disconnect");
 *      info->add("show");
 *      info->add("show_connections");
 *      info->add("show_format_fiels");
 *      info->add("new_session");
 *  }
 *
 *  if ( info->completingSecondWord("connect") )
 *      info->setHelpMessage("Provide hostname where logServer is located...");
 *
 *  if ( info->firstWord() == "show" )
 *  {
 *      std::string message =
 *      " * show: Show logs on screen\n"\
 *      "\n \n"\
 *      "        [-format str_format]   Define format of how logs are displayed on screes   \n"\
 *      "        [-limit N]             Define the maximum number of logs to be displayed ( default 10000 ) \n"\
 *      "        [-type T]              Show only logs of a certain type: W M T X V ...\n"\
 *      "        [-time HH:MM::SS]      Show only logs generated before given time stamp\n"\
 *      "        [-date DD/MM/YY]       Show only logs generated before given date\n"\
 *      "        [-pattern str_pattern] Show only logs that match a particular regular experssion\n"\
 *      "        [-reverse]             Show records in reverse order\n"\
 *      "        [-multi_session]       Show logs from any session\n"\
 *      "        [-table]               Show records in a table instead on line by line\n";
 *
 *      info->setHelpMessage( message );
 *
 *  }
 *
 * }
 *
 */
}
