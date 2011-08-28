#ifndef _H_LOG_FILE
#define _H_LOG_FILE

#include <iostream>
#include <fstream>			    // ifstream , ofstream

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "samson/data/data.pb.h"
#include "samson/common/traces.h"
#include "au/Lock.h"				// au::Lock
#include "au/map.h"				// au::map
#include <sstream>				// std::ostringstream

namespace samson {

	class LogFile
	{
		std::string fileName;
		
		std::ofstream output;	// Stream used to append contect to the file
		std::ifstream input;	// Stream used to recover previous elements of a file
		
		char *buffer;			// Buffer to serialize GPB elements
		size_t buffer_size;		// Size of the previous buffer

        enum OpenMode
        {
            undefinied,
            open_to_read,
            open_to_append
        };
        
        OpenMode openMode;
        
        
	public:
		
		LogFile( std::string _fileName )
		{
			fileName = _fileName;
			
			buffer = NULL;
			buffer_size = 0;
            
            openMode = undefinied;
		}
		
		~LogFile()
		{
			if( buffer )
				free( buffer );
		}
		
		bool openToRead( );
		bool openToAppend( );
		
		void close();
		
        // Read a new command from file ( if open as roRead )
		bool read( data::Command &c );		

        // Write a command to file ( if open as toAppend )
		void write( data::Command &c );
		void write( size_t task_id , std::string command , data::Command::Action action );

		// Get the fileName used with this LogFile
		std::string getFileName();
		
	private:
		
        // Check that we have enougth space to write in the buffer the provided command
		void checkBuffer( size_t size ) ;
		
	};

}

#endif
