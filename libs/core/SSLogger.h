#ifndef SAMSON_LGGER_H
#define SAMSON_LGGER_H

#include <string>
#include <fstream>

namespace ss {

	class Task;
	class KVSet;
	
	class SSLogger
	{
		SSLogger();
		
	public:

		typedef enum { message, warning, error  } LogType;
		
		static SSLogger * shared();
		
		//std::string homeDirectory;			// Working directory
		//std::string logDirectory;				// Directory for logging
		std::string logFile;
		std::ofstream logStream;
		
		// Functions to log messages and errors from tasks
		static void log( LogType type , std::string message );
		static void log( Task * task , LogType type , std::string message );
		static void log( KVSet * set , LogType type , std::string message );

		static void log( std::string header , LogType type , std::string message);
		
	private:
		
		/**
		 Generic function to log with a particular reference
		 */
		
		void _log( std::string &header , LogType type , std::string &message);
	};
}

#endif
