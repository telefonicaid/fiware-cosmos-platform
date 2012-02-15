/* ****************************************************************************
*
* FILE                     status.cpp
*
* DESCRIPTION              Status values for most of samson methods
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jun 06 2011
*
*/
#include "status.h"



namespace samson
{



/* *******************************************************************************
*
* status - 
*/
const char* status(Status s)
{
	switch (s)
	{
	case OK:                   return "OK";
	case NotImplemented:       return "Not Implemented";

	case BadMsgType:           return "BadMsgType";
	case NullHost:             return "Null Host";
	case BadHost:              return "Bad Host";
	case NullPort:             return "Null Port";
	case Duplicated:           return "Duplicated";
	case KillError:            return "Kill Error";
	case NotHello:             return "Not Hello";
	case NotAck:               return "Not an Ack";
	case NotMsg:               return "Not a Msg";

	case Error:                return "Error";
	case ConnectError:         return "Connect Error";
	case AcceptError:          return "Accept Error";
	case NotListener:          return "Not a Listener";
	case SelectError:          return "Select Error";
	case SocketError:          return "Socket Error";
	case GetHostByNameError:   return "Get Host By Name Error";
	case BindError:            return "Bin dError";
	case ListenError:          return "Listen Error";
	case ReadError:            return "Read Error";
	case WriteError:           return "Write Error";
	case Timeout:              return "Timeout";
	case ConnectionClosed:     return "Connection Closed";
	case PThreadError:         return "Thread Error";
    case WrongPacketHeader:    return "Wrong Packet Header";        
    case ErrorParsingGoogleProtocolBuffers: return "Error parsing Google Protocol Buffer message";
	}

	return "Unknown Status";
}

    VisualitzationOptions getVisualitzationOptions( std::string command )
    {
        au::CommandLine cmdLine;
        cmdLine.set_flag_boolean("v");
        cmdLine.set_flag_boolean("vv");
        cmdLine.set_flag_boolean("vvvv");
        cmdLine.set_flag_boolean("all");
        cmdLine.parse(command);
        
        if( cmdLine.get_flag_bool("all") )
            return all;
        if( cmdLine.get_flag_bool("vvv") )
            return verbose3;
        if( cmdLine.get_flag_bool("vv") )
            return verbose2;
        if( cmdLine.get_flag_bool("v") )
            return verbose;
        
        return normal;
    }
    
}
