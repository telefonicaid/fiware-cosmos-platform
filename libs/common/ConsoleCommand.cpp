
#include "ConsoleCommand.h"		// au::ConsoleCommand

namespace au
{
	ConsoleCommand::ConsoleCommand()
	{
		init();
		pos_history = 0;
		
	}
	
	std::string ConsoleCommand::confirmCommand()
	{
		command[length]='\0';
		std::string cmd = std::string( command );
		
		history.push_front( cmd );
		if( history.size() > 100 )
			history.pop_back();
		
		init();
		
		return cmd;
	}
	
	void ConsoleCommand::notifyString( std::string txt )
	{
		
		if( cursor_pos < length )
			for (int i = length-1 ; i >= cursor_pos  ; i--)
				command[i+txt.length()] = command[i];
		
		for (size_t i = 0 ; i < txt.length() ; i++)
		{
			command[cursor_pos]=txt[i];
			cursor_pos++;
			length++;
		}
		
		command[length]='\0';
		
		pos_history = 0;
		
	}
	
	void ConsoleCommand::notifyChar( char c )
	{
		if( cursor_pos < length )
			for (int i = length-1 ; i >= cursor_pos  ; i--)
				command[i+1] = command[i];
		
		command[cursor_pos]=c;
		
		cursor_pos++;
		length++;
		
		// End of string mark
		command[length]='\0';
		
		pos_history = 0;
		
	}
	
	void ConsoleCommand::init()
	{
		length = 0;
		cursor_pos = 0;
		command[0]='\0';
	}
	
	void ConsoleCommand::moveLeft()
	{
		if( cursor_pos > 0)
			cursor_pos--;
		
		pos_history = 0;
	}
	
	void ConsoleCommand::moveRigth()
	{
		if( cursor_pos < length )
			cursor_pos++;
		
		pos_history = 0;
	}
	
	void ConsoleCommand::delete_character()
	{
		if( cursor_pos > 0 )
		{
			for (int i = cursor_pos ; i < length ; i++)
				command[i] = command[i+1];
			
			length--;
			cursor_pos--;
			
			// End of string mark
			command[length]='\0';
			
		}
		
		pos_history = 0;
		
	}
	
	
	void ConsoleCommand::previous()
	{
		
		std::string previous_command = history[pos_history];
		length = previous_command.length();
		cursor_pos = length;
		memcpy(command, previous_command.c_str(), length );
		command[length]='\0';
		
		if( pos_history < (int)(history.size()-1) )
			pos_history++;
		
	}
	
	void ConsoleCommand::next()
	{
		if ( pos_history > 0)
			pos_history--;
		
		std::string previous_command = history[pos_history];
		length = previous_command.length();
		cursor_pos = length;
		memcpy(command, previous_command.c_str(), length );
		command[length]='\0';
		
	}
	
}