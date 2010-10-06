#include <sstream>

#include "samson.h"
#include "KVSet.h"
#include "Task.h"                /* Task                                     */
#include "SSLogger.h"            /* Own interface                            */



namespace ss {

	
	template <class T>
	inline std::string to_string (const T& t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}	

	SSLogger *ssLogger;
	
	SSLogger * SSLogger::shared()
	{
		if( !ssLogger )
			ssLogger = new SSLogger();
		
		return ssLogger;
	}
	
	SSLogger::SSLogger()
	{
		time_t rawtime;
		time ( &rawtime );
		
		// Define the directory where logs for all jobs will be written
	
		std::ostringstream o;
		o << SAMSON_LOG_DIRECTORY << "/session_" << rawtime << "_" << rand()%10000 << ".txt";				
		logFile = o.str();
		
		// Open the log directory	
		logStream.open( logFile.c_str() );
	}
	
#pragma mark LOG functions
	
	
	void SSLogger::log( std::string header , LogType type , std::string message)
	{
		SSLogger::shared()->_log( header , type , message );
		
	}
	
	void SSLogger::_log( std::string &header , LogType type , std::string &message )
	{
		std::ostringstream o;
		
		switch (type) {
			case SSLogger::message:
				o << "["<< header << "][msg] " << message << std::endl;
				break;
			case SSLogger::warning:
				o << "["<< header << "][wrn] " << message << std::endl;
				break;
			case SSLogger::error:
				o << "["<< header << "][error] " << message << std::endl;
				break;
		}
		
		logStream << o.str();
		
		// Flush every post?
		logStream.flush();
	}
	
	// Static functions to log
	
	void SSLogger::log( LogType type , std::string message )
	{
		std::string header = "-";
		SSLogger::shared()->_log( header , type , message );
	}

	void SSLogger::log( Task * task ,  LogType type ,  std::string message ) 
	{
		std::string header = "TASK " +  to_string<size_t>(task->getId());
		SSLogger::shared()->_log( header , type , message);
	}

	void SSLogger::log( KVSet * set ,  LogType type ,  std::string message ) 
	{
		std::string header = "KVSet " + to_string<size_t>(set->getId());
		SSLogger::shared()->_log( header , type , message);
	}
	
}

