/* ****************************************************************************
 *
 * FILE                     samson_module.h
 *
 * DESCRIPTION				Parser of module file to create .h .cpp files
 *
 * ***************************************************************************/


#include <samson/samsonVersion.h>   /* SAMSON_VERSION                             */
#include "samson_module.h"			/* Own interface                              */
#include <samson/Data.h>            /* DataInstance                               */
#include "CommandLine.h"			/* AUCommandLine                              */
#include "DataContainer.h"
#include "DataCreator.h"


/** 
 Main function to parse everything 
 */

int main( int args , const char *argv[])
{
	
	// debug
	//ss::DataCreator _module_creator( "/Users/andreu/devel/samson_modules/cdr/module"  );		// A data creator object to generate the code
	//_module_creator.print();
	//return 0;
	
	

	fprintf(stderr,"SAMSON Module tool  (v %s)\n", SAMSON_MODULE_PARSER_VERSION);
	fprintf(stderr, "========================================================\n");

	//Help parameter in the comman line
	au::CommandLine cmdLine;
	cmdLine.set_flag_boolean("help");		// Get this help
	cmdLine.parse(args , argv);
	
	if ( cmdLine.get_flag_bool("help") )
	{
		fprintf(stderr, "This tool is used to generate/update a .h & .cpp files with the definition of a new module for the samson platform.\n");
		fprintf(stderr, "A samson-module can contain new data types for your keys and values, maps, reducers and generators.");
		fprintf(stderr, "Sintax of this txt-file is really simple.\n");
		fprintf(stderr, "\n\nMore help coming soon (andreu@tid.es)\n");
		return 0;
	}
	
	
	if( cmdLine.get_num_arguments() < 2 )
	{
		fprintf(stderr, "Usage: %s module_file \n" , argv[0]);
		fprintf(stderr, "Type -help for more help\n\n");
		exit(0);
	}
	
	std::string moduleFileName = cmdLine.get_argument(1);

	 
	ss::DataCreator module_creator( moduleFileName  );		// A data creator object to generate the code
	module_creator.print();
	
	return 0;
}


