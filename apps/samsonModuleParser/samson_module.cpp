/* ****************************************************************************
 *
 * FILE                     samson_module.h
 *
 * DESCRIPTION		    Parser of module file to create .h .cpp files
 *
 * ***************************************************************************/


#include <samson/samsonVersion.h>   /* SAMSON_VERSION                             */
#include "samson_module.h"			/* Own interface                              */
#include <samson/Data.h>            /* DataInstance                               */
#include "au/CommandLine.h"			/* AUCommandLine                              */
#include "DataContainer.h"
#include "DataCreator.h"
#include "parseArgs.h"          // parseArgs
#include "paUsage.h"            // paUsage
#include <sys/stat.h>           // stat()

int  tsCompare ( struct  timespec  time1, struct  timespec  time2)
{
    
    if (time1.tv_sec < time2.tv_sec)
        return (-1) ;				/* Less than. */
    else if (time1.tv_sec > time2.tv_sec)
        return (1) ;				/* Greater than. */
    else if (time1.tv_nsec < time2.tv_nsec)
        return (-1) ;				/* Less than. */
    else if (time1.tv_nsec > time2.tv_nsec)
        return (1) ;				/* Greater than. */
    else
        return (0) ;				/* Equal. */
    
}

/** 
 Main function to parse everything 
 */

int main( int argC , const char *argV[])
{
	
	


	//Help parameter in the comman line
	au::CommandLine cmdLine;
	cmdLine.set_flag_boolean("help");		// Get this help
	cmdLine.parse(argC , argV);
	
	if ( cmdLine.get_flag_bool("help") )
	{
		fprintf(stderr, "This tool is used to generate/update a .h & .cpp files with the definition of a new module for the samson platform.\n");
		fprintf(stderr, "A samson-module can contain new data types for your keys and values, maps, reducers and generators.");
		fprintf(stderr, "Sintax of this txt-file is really simple.\n");
		fprintf(stderr, "\n\nMore help coming soon (andreu@tid.es)\n");
		return 0;
	}
	
	
	if( cmdLine.get_num_arguments() < 4 )
	{
		fprintf(stderr, "Usage: %s module_file output_directory output_file_name \n" , argV[0]);
		fprintf(stderr, "Type -help for more help\n\n");
		exit(1);
	}
	
	std::string moduleFileName = cmdLine.get_argument(1);
    std::string outputDirectory = cmdLine.get_argument(2);
    std::string outputFilename = cmdLine.get_argument(3);

    
    // Check it time-stamp is greater to not do anything..
    struct stat stat_module , stat_output1, stat_output2 ;
    
    if ( stat( moduleFileName.c_str() , &stat_module ) != 0)
    {
        std::cerr << "Error opening module file";
        exit(0);
    }
    
    std::string output1_filename = outputFilename + ".cpp";
    std::string output2_filename = outputFilename + ".h";
    
    int res_stat1 = stat( output1_filename.c_str() , &stat_output1);
    int res_stat2 = stat( output2_filename.c_str() , &stat_output2);

    
    if( ( res_stat1 == 0) && ( res_stat2 == 0) )
    {
        
        if( tsCompare( stat_module.st_mtimespec , stat_output1.st_mtimespec ) < 0 )
            if( tsCompare( stat_module.st_mtimespec , stat_output2.st_mtimespec ) < 0 )
            {
                std::cerr << "Not creating Modules.cpp and Modules.h since module is input file is older than the new one\n";
                return 0;
            }
    }
    else
    {
        std::cerr << "Not possible to open " << output1_filename << " or " << output2_filename << " so, outputs will be generated again\n";
    }
    
	fprintf(stderr,"SAMSON Module tool  (v %s)\n", SAMSON_MODULE_PARSER_VERSION);
	fprintf(stderr, "========================================================\n");
    
	 
	ss::DataCreator module_creator( moduleFileName, outputDirectory , outputFilename  );		// A data creator object to generate the code
	module_creator.print();
	
	return 0;
}


