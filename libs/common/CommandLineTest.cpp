
#include "CommandLinetest.h"		// au::CommandLineTest

namespace au {

	int CommandLineTest::run()
	{
		if( test1() )
		{
			fprintf(stderr, "Test 1............ OK\n");
			return 0;
		}
		else
		{
			fprintf(stderr, "Test 1............ KO !!!!\n");
			return 1;
		}
		
	}
	
	
	bool CommandLineTest::test1()
	{
		std::string test_line = "command -n 1 -m 2 -flag_spetial -f file.txt arg1 -memory 1000 ";
		
		CommandLine commandLine;
		
		commandLine.set_flag_int("n", 10);
		commandLine.set_flag_int("m", 11);
		commandLine.set_flag_int("l", 12);
		commandLine.set_flag_uint64("memory", 1024*1024);
		commandLine.set_flag_uint64("memory2", 1024*1024);
		
		commandLine.set_flag_string("f", "unknown_file");
		
		
		commandLine.set_flag_boolean("flag_spetial");
		commandLine.set_flag_boolean("flag_spetial2");
		
		commandLine.parse( test_line );
		
		
		//Check values at the output
		
		if ( commandLine.get_flag_int("n") != 1)
			return false;
		if ( commandLine.get_flag_int("m") != 2)
			return false;
		if ( commandLine.get_flag_int("l") != 12)
			return false;
		
		if ( commandLine.get_flag_string("f") != "file.txt")
			return false;
		
		if ( commandLine.get_flag_uint64("memory") != 1000)
			return false;
		if ( commandLine.get_flag_uint64("memory2") != 1024*1024)
			return false;
		
		if( !commandLine.get_flag_bool("flag_spetial") )
			return false;
		
		if( commandLine.get_flag_bool("flag_spetial2") )
			return false;
		
		if ( commandLine.get_num_arguments() != 2)
			return false;
		
		if ( commandLine.get_argument(0) != "command" )
			return false;
		
		return true;
	}
	
}