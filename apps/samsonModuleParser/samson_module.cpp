#include "au.h"
#include "DataType.h"
#include "DataContainer.h"
#include "samson_module.h"
#include "samson_lib.h"



void create_module_file( std::string module_name  , std::string fileName )
{
	
	ofstream o( fileName.c_str() );
	
	if( !o.is_open() )
	{
		std::cout << "Error creating " << fileName << std::endl;
		exit(0);
	}
	
	o <<"Module " << module_name << "\n";
	o <<"{\n";
	o <<"\ttitle   \"\"\n";
	o <<"\tauthor  \"\"\n";
	o <<"\tversion \"0.1\"\n";
	o <<"}\n";

	o << "\n\n";
		
	o<<"#data example_data\n";
	o<<"#{\n";
	o<<"#\tuint value;\n";
	o<<"#}\n";

	o << "\n\n";

	o<<"#map example_map\n";
	o<<"#{\n";
	o<<"#\tin base.UInt base.UInt\n";
	o<<"#\tout base.UInt base.UInt\n";
	o<<"#\n";
	o<<"#\tfile ModuleImpl.cpp\n";
	o<<"#}\n";
	
	o.close();
	
}	


void create_makefile( std::string fileName )
{
	
	ofstream o( fileName.c_str() );

	if( !o.is_open() )
	{
		std::cout << "Error creating " << fileName << std::endl;
		exit(0);
	}
	
	o << "all:\n\tmake build_local\n\tsudo make install\n\n";

	o << "build_local:\n\tmkdir -p build;\n\tcd source;samson_module Module;cd ..\n\tcd build;cmake ../source\n\tmake -C build\n\n";

	o << "prepare:\n\tmkdir -p build;\n\tcd build;cmake ../source\n\n";

	o << "install:\n\tmake -C build install\n\n";

	o << "clean:\n\tmake -C build clean\n\n";
	
	o << "xcode:\n\tmkdir -p xcode_proj\n\tcd xcode_proj;cmake -G Xcode ../source\n\n";
	
	o << "test:\n\tmake\n\tmake -C local_build test\n\n";

	o << "reset:\n\trm -Rf build\n\trm -Rf xcode_proj\n\n";
	
	o.close();
	

}

void create_file( std::string fileName )
{
	
	ofstream o( fileName.c_str() );
	
	if( !o.is_open() )
	{
		std::cout << "Error creating " << fileName << std::endl;
		exit(0);
	}
	
	o << "\n#include \"Module.h\"\n";

	
	o.close();
	
}


void create_CMakeFiles( std::string module_name , std::string fileName )
{
	
	ofstream o( fileName.c_str() );
	
	if( !o.is_open() )
	{
		std::cout << "Error creating file " << fileName << std::endl;
		exit(0);
	}
	
	
	o << "PROJECT( " << module_name <<  "  )\n\n";
		o << "cmake_minimum_required(VERSION 2.6)\n\n";
	
	o << "SET( SOURCES\nModule.cpp\nModuleImpl.cpp\n)\n";	
	o << "SET( HEADERS\nModule.h\n)\n";
	
	o << "ADD_LIBRARY( " << module_name	<< " MODULE ${SOURCES} ${HEADERS} )\n";
	
	o << "INSTALL(TARGETS "<<module_name<<" DESTINATION ~/.samson/modules )\n";
	o << "INSTALL(TARGETS "<<module_name<<" DESTINATION /var/samson/modules )\n";	
	o << "INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/"<< module_name << ")\n";
	
	o.close();

}

/**
 Create a new module: directory , makefile, module file, etc..
 */

void create_new_module( std::string name )
{
	std::cout << "Creating module " << name << std::endl;
	
	if( mkdir( name.c_str() ,S_IRWXU) )
	{
		std::cerr << "Error: not possible to access/create directory " << name << std::endl;
		return;
	}

	std::string sourceDir = name + "/source";
	if( mkdir( sourceDir.c_str() ,S_IRWXU) )
	{
		std::cerr << "Error: not possible to access/create directory " << sourceDir << std::endl;
		return;
	}
	
	// Create makefile
	create_makefile( name + "/makefile" );
	
	// Create CMakeFiles
	create_CMakeFiles( name , name + "/source/CMakeLists.txt" );

	
	// Module file
	create_module_file( name , name + "/source/Module" );
	
	// Module implementaton
	create_file( name + "/source/ModuleImpl.cpp" );
}



/** 
 Main function to parse everything 
 */

int main( int args , const char *argv[])
{

	fprintf(stderr,"\nMACRO Module tool  (v %s)\n", SAMSON_VERSION);
	fprintf(stderr, "========================================================\n");
		
	if( args < 2 )
	{
		fprintf(stderr, "Usage: samson_module module_file.\n");
		fprintf(stderr, "Usage: samson_modules -new module_name.\n");
		fprintf(stderr, "Type -help for more help\n\n");
		exit(0);
	}
	
	
	//Help parameter in the comman line
	AUCommandLine cmdLine;
	cmdLine.set_flag_boolean("help");		// Get this help
	cmdLine.set_flag_boolean("new");		// Flag to create a new module ( a driectory an so )
	cmdLine.parse(args , argv);
	
	if ( cmdLine.get_flag_bool("help") )
	{
		fprintf(stderr, "This tool is used to generate/update a .h & .cpp files with the definition of a new module for the samson platform.\n");
		fprintf(stderr, "A samson-module can contain new data types for your keys and values, maps, reducers and generators.");
		fprintf(stderr, "Sintax of this txt-file is really simple.\n");
		fprintf(stderr, "\n\nMore help comming soon (andreu@tid.es)\n");
		return 0;
	}

	if( cmdLine.get_flag_bool("new") )
	{
		// Create a directory with all the files inside
		std::string module_name = cmdLine.get_argument( 1 );
		create_new_module( module_name );
		return 0;
	}
	
	
	ss::DataCreator module_creator( argv[1] );
	module_creator.print();
		
	return 0;
}


