/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
/* ****************************************************************************
 *
 * FILE                     samson_module.h
 *
 * DESCRIPTION		    Parser of module file to create .h .cpp files
 *
 * ***************************************************************************/

#include "DataContainer.h"
#include "DataCreator.h"
#include "au/CommandLine.h"      /* AUCommandLine                              */
#include "parseArgs/paUsage.h"   // paUsage
#include "parseArgs/parseArgs.h"  // parseArgs
#include <samson/module/Data.h>  /* DataInstance                               */
#include <samson/module/samsonVersion.h>   /* SAMSON_VERSION                             */
#include <sys/stat.h>    // stat()



#ifdef __gnu_linux__
int tsCompare(time_t time1, time_t time2) {
  if (time1 < time2) {
    return -1;           /* Less than     */
  } else if (time1 > time2) {
    return 1;            /* Greater than  */
  } else { return 0;  /* Equal         */
  }
}

#else

int  tsCompare(struct  timespec time1, struct  timespec time2) {
  if (time1.tv_sec < time2.tv_sec) {
    return (-1);         /* Less than. */
  } else if (time1.tv_sec > time2.tv_sec) {
    return (1);          /* Greater than. */
  } else if (time1.tv_nsec < time2.tv_nsec) {
    return (-1);         /* Less than. */
  } else if (time1.tv_nsec > time2.tv_nsec) {
    return (1);          /* Greater than. */
  } else {
    return (0);          /* Equal. */
  }
}

#endif  // ifdef __gnu_linux__

bool verbose;

/**
 * Main function to parse everything
 */

int main(int argC, const char *argV[]) {
  // Help parameter in the command line
  au::CommandLine cmdLine;

  cmdLine.SetFlagBoolean("help");             // Get this help
  cmdLine.SetFlagBoolean("v");                // verbose mode
  cmdLine.Parse(argC, argV);

  if (cmdLine.GetFlagBool("help")) {
    fprintf(
      stderr,
      "samsonModuleParser: This tool is used to generate/update a .h & .cpp files with the definition of a new module for the samson platform.\n");
    fprintf(stderr,
            "A samson-module can contain new data types for your keys and values, maps, reducers and generators.");
    fprintf(stderr, "Syntax of this txt-file is really simple.\n");
    fprintf(stderr, "\n\nMore help coming soon (andreu@tid.es)\n");
    return 0;
  }


  if (cmdLine.get_num_arguments() < 4) {
    fprintf(stderr, "Usage: %s module_file output_directory output_file_name \n", argV[0]);
    fprintf(stderr, "Type -help for more help\n\n");
    exit(1);
  }

  std::string moduleFileName  = cmdLine.get_argument(1);
  std::string outputDirectory = cmdLine.get_argument(2);
  std::string outputFilename  = cmdLine.get_argument(3);

  // Check it time-stamp is greater to not do anything..
  struct stat stat_module, stat_output1, stat_output2;

  if (stat(moduleFileName.c_str(), &stat_module) != 0) {
    std::cerr << "samsonModuleParser: Error opening module file";
    exit(0);
  }

  std::string output1_filename = outputFilename + ".cpp";
  std::string output2_filename = outputFilename + ".h";

  int res_stat1 = stat(output1_filename.c_str(), &stat_output1);
  int res_stat2 = stat(output2_filename.c_str(), &stat_output2);

  verbose = cmdLine.GetFlagBool("v");

  if (( res_stat1 == 0) && ( res_stat2 == 0)) {
/* Disabling timestamp checking, since makefile is more clever and if it has called samsonModuleParser,
 * it has some work to do (for example, the samsonModuleParser itself may have changed).
 *
 * #ifdef __gnu_linux__
 *              if (tsCompare(stat_module.st_mtime, stat_output1.st_mtime) < 0)
 *                      if(tsCompare(stat_module.st_mtime, stat_output2.st_mtime) < 0)
 *          {
 *              std::cerr << "samsonModuleParser: Not creating " << output1_filename << " and " << output2_filename << " since the module input file is older than the new one\n";
 *              return 0;
 *          }
 * #else
 *      if( tsCompare( stat_module.st_mtimespec , stat_output1.st_mtimespec ) < 0 )
 *          if( tsCompare( stat_module.st_mtimespec , stat_output2.st_mtimespec ) < 0 )
 *          {
 *              std::cerr << "samsonModuleParser: Not creating " << output1_filename << " and " << output2_filename << " since the module input file is older than the new one\n";
 *              return 0;
 *          }
 * #endif
 */
  } else {
    if (verbose) {
      std::cerr << "samsonModuleParser: Not possible to open " << output1_filename << " or " << output2_filename <<
      " so, outputs will be generated again\n";
    }
  }

  if (verbose) {
    fprintf(stderr, "SAMSON Module Parser tool for samson %s \n", SAMSON_VERSION);
    fprintf(stderr, "=====================================================================\n");
  }

  // A data creator object to generate the code
  au::ErrorManager error;

  samson::ModuleInformation *module_information = samson::ModuleInformation::parse(moduleFileName, &error);

  if (error.IsActivated()) {
    std::cerr << "Error parsing file " << moduleFileName << "\n";
    std::cerr << error.GetMessage();
  }

  // Print all the files
  module_information->print(outputDirectory, outputFilename);
  delete module_information;

  return 0;
}

