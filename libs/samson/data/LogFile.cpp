
#include "LogFile.h"			// Own interface
#include <sys/time.h>			// gettimeofday

namespace samson
{

	bool LogFile::openToRead( )
	{
        if ( openMode != undefinied )
        {
            LM_W(("LogFile can only be open once..."));
            return false;
        }
        
		LM_T(LmtFile, ("opening '%s' to read", fileName.c_str()));
        openMode = open_to_read;
		
		input.open( fileName.c_str() );
		return input.is_open();
	}
	
	bool LogFile::openToAppend( )
	{
        if ( openMode != undefinied )
        {
            LM_W(("LogFile can only be open once..."));
            return false;
        }
        
		LM_T(LmtFile, ("opening '%s' to append", fileName.c_str()));
        openMode = open_to_append;
		
		output.open( fileName.c_str() , std::ios::app );
		return output.is_open();

	}
	
	void LogFile::close()
	{
		if( input.is_open() )
			input.close();
		
		if( output.is_open() )
			output.close();
	}
	
	bool LogFile::read( data::Command &c )
	{
        if( openMode != open_to_read )
        {
            LM_W(("LogFile not open correctly"));
            return false;
        }
        
		if( !input.is_open() )
			LM_X(1, ("Error in log-file processing in read operation"));
		
		if( input.eof() )
			return false;
		
		size_t message_size;
		input.read((char*)&message_size, sizeof(size_t));
		
		if( input.eof() )
			return false;
		
		checkBuffer(message_size);
		
		input.read( buffer , message_size );
		c.ParseFromArray(buffer,message_size);
		
		return true;
	}
	
	void LogFile::write( data::Command &c )
	{
        if( openMode != open_to_append )
        {
            LM_W(("LogFile not open correctly"));
            return;
        }
        
		if ( !output.is_open() )
			LM_X(1, ("Error in log-file processing in write operation"));

		
		size_t message_size = c.ByteSize();
		size_t packet_size = c.ByteSize() + sizeof( size_t );
				
		// Make sure we have space int the buffer
		checkBuffer( packet_size );
		
		// Write the size first
		*((size_t*)buffer) = message_size ;	// Write the length
		
		// Write the message itself in the buffer
		c.SerializeToArray( buffer + sizeof( size_t ) , message_size	);
		
		// Write the buffer to disk
		if( output.write(buffer, packet_size)  )
			output.flush();
		else
		{
			std::cerr << "Error writing in log file of data manager: " << fileName << std::endl;
			exit(0);
		}
	}	
	
	void LogFile::write( size_t task_id , std::string command , data::Command::Action action )
	{
		data::Command c;
		
		time_t _t =  time(NULL);
		
		c.set_task_id( task_id );
		c.set_command( command );
		c.set_action( action );
		c.set_time( _t );
		
		write( c );
	}	
    
	void LogFile::checkBuffer( size_t size ) 
	{
		if( buffer_size < size )
		{
			if( buffer )
				free(buffer);
			
			buffer =  (char *) malloc(size);
			buffer_size = size;
		}
	}
    
    std::string LogFile::getFileName()
    {
        return fileName;
    }

	


}
