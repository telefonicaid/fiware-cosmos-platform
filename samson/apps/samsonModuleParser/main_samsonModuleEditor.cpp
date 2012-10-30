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

#include <dirent.h>
#include <sys/stat.h>
#include <sys/stat.h>            // stat()
#include <sys/types.h>

#include "logMsg/logMsg.h"       // LM_*
#include "logMsg/traceLevels.h"  // Trace Levels
#include "parseArgs/paUsage.h"   // paUsage
#include "parseArgs/parseArgs.h"  // parseArgs

#include "au/console/Console.h"
#include "au/console/ConsoleAutoComplete.h"

#include "au/containers/StringVector.h"
#include "au/tables/Select.h"
#include "au/tables/Table.h"

#include "DataContainer.h"
#include "DataCreator.h"
#include "au/CommandLine.h"                /* AUCommandLine                              */
#include <samson/module/Data.h>            /* DataInstance                               */
#include <samson/module/samsonVersion.h>   /* SAMSON_VERSION                             */

/* ****************************************************************************
 *
 * parse arguments
 */
char working_directory[1024];

// #define DEFAULT_WORKING_DIRECTORY "/Users/andreu/devel/samson/trunk/modules/"
#define DEFAULT_WORKING_DIRECTORY "./"

PaArgument paArgs[] =
{
  { "", working_directory, "", PaString, PaOpt, _i DEFAULT_WORKING_DIRECTORY, PaNL, PaNL, "Working directory"             },
  PA_END_OF_ARGS
};

/* ****************************************************************************
 *
 * logFd - file descriptor for log file used in all libraries
 */
int logFd = -1;


class SamsonModuleEditor : public au::Console {
  // List of modules defined in this directory...
  au::map<std::string, samson::ModuleInformation> modules;

public:


  SamsonModuleEditor() {
    // Parse current directory to get information....

    // Try to open directory
    DIR *dp;
    struct dirent *dirp;

    if ((dp  = opendir(working_directory)) == NULL) {
      return;       // Nothing else to do...
    }
    while ((dirp = readdir(dp)) != NULL) {
      std::string fileName = dirp->d_name;

      // Skip ".files"
      if (fileName.length() > 0) {
        if (fileName[0] == '.') {
          continue;
        }
      }
      std::string path = working_directory + fileName;

      struct ::stat info;
      stat(path.c_str(), &info);

      if (S_ISDIR(info.st_mode)) {
        writeOnConsole(au::str("Reading directory %s...\n", path.c_str()));
        std::string module_fileName = path + "/module";

        au::ErrorManager error;
        samson::ModuleInformation *module_information = samson::ModuleInformation::parse(module_fileName, &error);

        if (error.IsActivated()) {
          writeErrorOnConsole(error.GetMessage());
        } else {
          if (module_information->module.name != fileName) {
            writeErrorOnConsole(
              au::str("Module '%s' defined inside directory %s: Skipping...\n",
                      module_information->module.name.c_str(),
                      fileName.c_str()
                      ));
            delete module_information;
          } else {
            modules.insertInMap(fileName, module_information);
          }
        }
      }
    }

    closedir(dp);
  }

  std::string getPrompt() {
    return "ModuleEditor >>";
  }

  au::tables::Table *getTableOfModules() {
    au::StringVector fields("Name", "Title", "#Datas", "#Operations");
    au::tables::Table *table = new au::tables::Table(fields);

    int num_operations = 0;
    int num_datas = 0;

    au::map<std::string, samson::ModuleInformation>::iterator it_modules;
    for (it_modules = modules.begin(); it_modules != modules.end(); it_modules++) {
      samson::ModuleInformation *module_information = it_modules->second;

      au::StringVector fields_values;
      fields_values.push_back(module_information->module.name);
      fields_values.push_back(module_information->module.title);

      fields_values.push_back(au::str(module_information->datas.size()));
      fields_values.push_back(au::str(module_information->operations.size()));

      num_operations += module_information->operations.size();
      num_datas += module_information->datas.size();

      table->addRow(fields_values);
    }

    // Empty row...
    table->addRow(au::StringVector());

    // TOTAL row

    au::StringVector fields_values;
    fields_values.push_back("TOTAL");
    fields_values.push_back("");

    fields_values.push_back(au::str(num_datas));
    fields_values.push_back(au::str(num_operations));

    table->addRow(fields_values);

    return table;
  }

  void evalCommand(std::string command) {
    au::CommandLine cmdLine;

    cmdLine.Parse(command);

    if (cmdLine.get_num_arguments() == 0) {
      return;
    }

    std::string main_command = cmdLine.get_argument(0);

    if (main_command == "quit") {
      // TODO: Check everything is saved....

      // Quit console to finish
      quitConsole();
    }
    if (main_command == "ls_modules") {
      // Show information about modules
      au::tables::Table *table =  getTableOfModules();

      au::tables::SelectTableInformation select_table;
      select_table.title = "List of modules";
      select_table.addColumn("Name,l");
      select_table.addColumn("Title,l");
      select_table.addColumn("#Datas");
      select_table.addColumn("#Operations");

      writeOnConsole(table->str(&select_table));
      delete table;
    }

    if (main_command == "check") {
      au::map<std::string, samson::ModuleInformation>::iterator it_modules;
      for (it_modules = modules.begin(); it_modules != modules.end(); it_modules++) {
        samson::ModuleInformation *module_information = it_modules->second;

        for (size_t o = 0; o < module_information->operations.size(); o++) {
          // Name of the operation
          std::string operation = module_information->module.name + "." + module_information->operations[o].name;

          // Checking inputs
          for (size_t i = 0; i < module_information->operations[o].inputs.size(); i++) {
            samson::KVFormat key_values = module_information->operations[o].inputs[i].key_values;

            if (!check_data(key_values.keyFormat)) {
              writeWarningOnConsole(
                au::str("Operation %s: Unknown datatype %s at input %d"
                        , operation.c_str()
                        , key_values.keyFormat.c_str(),
                        i
                        )
                );
            }
            if (!check_data(key_values.valueFormat)) {
              writeWarningOnConsole(
                au::str("Operation %s: Unknown datatype %s at input %d"
                        , operation.c_str()
                        , key_values.valueFormat.c_str(),
                        i
                        )
                );
            }
          }

          // Checking outputs
          for (size_t i = 0; i < module_information->operations[o].outputs.size(); i++) {
            samson::KVFormat key_values = module_information->operations[o].outputs[i].key_values;

            if (!check_data(key_values.keyFormat)) {
              writeWarningOnConsole(
                au::str("Operation %s: Unknown datatype %s at output %d"
                        , operation.c_str()
                        , key_values.keyFormat.c_str(),
                        i
                        )
                );
            }
            if (!check_data(key_values.valueFormat)) {
              writeWarningOnConsole(
                au::str("Operation %s: Unknown datatype %s at output %d"
                        , operation.c_str()
                        , key_values.valueFormat.c_str(),
                        i
                        )
                );
            }
          }
        }
      }
    }
  }

  bool check_data(std::string data) {
    if (data == "txt") {
      return true;       // Only valid for some operations...
    }
    std::vector<std::string> components = au::split(data, '.');
    if (components.size() != 2) {
      return false;
    }

    samson::ModuleInformation *module_information = modules.findInMap(components[0]);

    if (!module_information) {
      return false;
    }

    for (size_t i = 0; i < module_information->datas.size(); i++) {
      if (module_information->datas[i].name == components[1]) {
        return true;
      }
    }

    return false;
  }

  void autoComplete(au::ConsoleAutoComplete *info) {
    if (info->completingFirstWord()) {
      info->add("ls_modules");
      info->add("quit");
      info->add("check");
    }
  }
};


int main(int argC, const char *argV[]) {
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen",                 (void *)false);
  paConfig("log file line format",          (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE] (p.PID) FUNC: TEXT");
  paConfig("screen line format",            (void *)"TYPE: TEXT");
  paConfig("log to file",                   (void *)true);

  paParse(paArgs, argC, (char **)argV, 1, false);      // No more pid in the log file name
  lmAux((char *)"father");
  logFd = lmFirstDiskFileDescriptor();

  SamsonModuleEditor samson_module_editor;
  samson_module_editor.runConsole();

  return 0;
}

