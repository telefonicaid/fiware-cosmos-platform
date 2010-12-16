
#include "LogFile.h"			// Own interface


namespace ss
{

	bool LogFile::openToRead( )
	{
		LM_T(LMT_FILE, ("opening '%s' to read", fileName.c_str()));
		
		input.open( fileName.c_str() );
		return input.is_open();
	}
	
	bool LogFile::openToAppend( )
	{
		LM_T(LMT_FILE, ("opening '%s' to append", fileName.c_str()));
		
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
		assert(input.is_open());
		
		if(  input.eof() )
			return false;
		
		size_t message_size;
		input.read((char*)&message_size, sizeof(size_t));
		
		if(  input.eof() )
			return false;
		
		checkBuffer(message_size);
		
		input.read( buffer , message_size );
		c.ParsePartialFromArray(buffer,message_size);
		
		return true;
	}
	
	void LogFile::write( data::Command &c )
	{
		assert( output.is_open() );
		
		size_t message_size = c.ByteSize();
		size_t packet_size = c.ByteSize() + sizeof( size_t );
		
		//std::cout << "Packet " << message_size << std::endl;
		
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
	
	void LogFile::write( size_t task_id , std::string command , data::Command::Action action )
	{
		data::Command c;
		
		c.set_task_id( task_id );
		c.set_command( command );
		c.set_action( action );
		
		write( c );
	}	

}