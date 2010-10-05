/* ****************************************************************************
*
* FILE                main_client.cpp - main program for samsonClient
*/



//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>

#include "samson.pb.h"
#include "NetworkManager.h"
#include "KVFormat.h"
#include "KVSet.h"
#include "KVManager.h"
#include "AUConsole.h"         /* Console */


namespace ss {

	class Client : public au::Console
	{
		au::ClientSocket *client;

	public:	
		
		
		Client( bool ncurses ) : Console( ncurses ) 
		{
			
		}

		bool connect()
		{
			try {
				client = new au::ClientSocket( "localhost", 9999);
			}
			catch ( au::SocketException& e )
			{
				std::cout << "Error connecting with the host" << std::endl;
				return false;
			}
			
			return true;
			
		}
		
		ssize_t getSizeOfFile(std::string fileName)
		{
			struct stat info;
			int res = stat(fileName.c_str(), &info);
			
			if (res == -1)	//Error probably because file does not exist
				return -1;
			
			return info.st_size;
			
		}		
		
		void loadFile( std::string fileName , std::string queue  )
		{
			
			writeOnConsole( "Preparing general packet" );

			packet p;
			p.message.set_description( "Load file" );
			p.message.set_queue( queue );					// Set the queue name
			p.message.set_code( 0 );
			KVManager::shared()->addAvailableMemory( 400*1024*1024 );

			p.dataBuffer.buffer = (char*)  malloc( 200*1024*1024 );

			FILE *file;

			writeOnConsole( "File selected: " + fileName );
			
			
			size_t total_size = 0;
			
			if( fileName == "stdin" )
			{
				writeOnConsole("Reading stdin");
				file = stdin;
			}
			else
				file = fopen(fileName.c_str() , "r" );
			
			while( !feof(file) )
			{
				
				writeOnConsole( "Reading input file...." );
				p.dataBuffer.size = fread( p.dataBuffer.buffer , 1,  200*1024*1024 , file);
				total_size += p.dataBuffer.size;
				
				{
					std::stringstream o;
					o << "Sending file " << fileName << " with " << au::Format::string( p.dataBuffer.size ) << " bytes" << " total: " << au::Format::string( total_size ) ;
					writeOnConsole( o.str() );
				}
				
				p.send( client  );
			}
			
			
			delete p.dataBuffer.buffer;
			
			writeOnConsole( "Closing file" );
			fclose(file);

			
		}
		
		void evalCommand(std::string command)
		{
			// Do something with the command
			AUCommandLine cmdLine;
			cmdLine.parse(command);
			
			// Quit
			if( cmdLine.get_num_arguments() == 0)
				return;
			
			if( cmdLine.get_argument(0) == "quit")
			{
				au::Console::quit();
				return;
			}

			if( cmdLine.get_argument(0) == "help")
			{
				std::stringstream o;
				o << "Help for samson_client" << std::endl;
				o << "----------------------" << std::endl;
				o << std::endl;
				o << "load_txt file queue		Send content of a file as a txt (system.String system.Void) to a queue" << std::endl;
				o << std::endl;
				writeBlockOnConsole( o.str() );
				return;
			}
			
			
			if( cmdLine.get_argument(0) == "load_txt")
			{
				if( cmdLine.get_num_arguments() < 3)
				{
					writeErrorOnConsole( "Usage: load_txt file queue");
					return;
				}
				
				std::string fileName = cmdLine.get_argument(1);
				std::string queue = cmdLine.get_argument(2);
				
				loadFile( fileName , queue );
				return;
			}
			
			
			// Normal command send directly to the platform
			
			packet p;
			p.message.set_code( 1 ); // txt command
			p.message.set_description( "Command" + command );
			p.message.set_command( command );
			p.send( client );
			
			
			// recieve the answer of this packet
			packet p2;
			p2.read( client );
			
			assert( p2.message.code() == 2);
			assert( p2.message.has_answer() );
			writeBlockOnConsole( p2.message.answer() );
		}
	};
}

int main(int argc, const char* argv[])
{
	AUCommandLine cmd;
	cmd.set_flag_boolean("console");
	cmd.set_flag_boolean("c");
	cmd.parse(argc, argv);
	
	ss::Client c( !cmd.get_flag_bool("console") );
	if( c.connect() )
	{
		
		if( cmd.get_flag_bool("c") )
		{
			std::ostringstream o;
			bool flag_found = false;
			for ( int i = 0 ; i < argc ; i++ )
			{
				std::string p = argv[i];
				if( flag_found )
					o << p << " ";
				else if ( p == "-c" )
					flag_found = true;
			}
			// Run a single command
			std::cout << "Running command " << o.str() << std::endl;
			c.evalCommand(o.str());
				
				
		}
		else
			c.run();
	}
	else
		std::cerr << "Connection with SAMSON platform not possible" << std::endl;
}
