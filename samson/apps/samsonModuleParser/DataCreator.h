/* ****************************************************************************
 *
 * FILE                     DataCreator.h
 *
 * DESCRIPTION				Creation of Data headeres
 *
 * ***************************************************************************/


#ifndef _H_DATACREATOR_H
#define _H_DATACREATOR_H

#include <cstdlib>
#include <fstream>
#include <set>
#include <string>
#include <sys/stat.h>
#include <vector>

#include "DataContainer.h"              // DataContainer
#include "ModuleContainer.h"    // ModuleContainer
#include "OperationContainer.h"  // OperationContainer

#include "au/ErrorManager.h"

namespace samson {
class ModuleContainer;
class OperationContainer;
class DataContainer;

class ModuleInformation {
public:

  // Information about the module
  ModuleContainer module;

  // List of Datas
  vector <DataContainer> datas;

  // List of Operations
  vector <OperationContainer> operations;



  // Parse content of a module file to get list of datas , operations ,  ...
  static ModuleInformation *parse(std::string module_file, au::ErrorManager *error);

  // Print two two output files *.h and *.cpp with all information
  void print(std::string outputDirectory, std::string output_file);


private:

  // Auxiliar functions
  void printMainHeaderFile(std::string outputFileName);
  void printMainFile(std::string outputFileName);
};
}

#endif // ifndef _H_DATACREATOR_H
