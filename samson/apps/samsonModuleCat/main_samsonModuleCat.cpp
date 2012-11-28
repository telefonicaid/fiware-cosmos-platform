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

#include <dirent.h>
#include <dirent.h>                       // DIR directory header
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/stat.h>                     // stat(.)
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>

#include "au/CommandLine.h"               // au::CommandLine

#include "au/tables/Table.h"

#include "logMsg/logMsg.h"                // LM_*
#include "logMsg/traceLevels.h"           // Trace Levels

#include "parseArgs/parseArgs.h"

#include "au/mutex/LockDebugger.h"        // au::LockDebugger

#include "samson/common/SamsonSetup.h"    // samson::SamsonSetup
#include "samson/common/coding.h"         // samson::FormatHeader
#include "samson/common/samsonVars.h"     // SAMSON_ARG_VARS

#include "samson/module/KVFormat.h"       // samson::KVFormat
#include "samson/module/ModulesManager.h"  // samson::ModulesManager

#include "samson/common/SamsonDataSet.h"  // samson::SamsonDataSet
#include "samson/common/SamsonFile.h"     // samson::SamsonFile


/* ****************************************************************************
 *
 * Option variables
 */

char file_name[1024];

/* ****************************************************************************
 *
 * parse arguments
 */

PaArgument paArgs[] =
{
  { " ", file_name, "", PaString, PaReq, (long)"null", PaNL, PaNL,
    "name of the file or directory to scan"     },
  PA_END_OF_ARGS
};

/* ****************************************************************************
 *
 * logFd - file descriptor for log file used in all libraries
 */
int logFd = -1;


void consider_file(std::string _fileName, au::tables::Table *table) {
  au::ErrorManager error;
  samson::Module *module = samson::ModulesManager::LoadModule(_fileName, error);

  if (error.IsActivated()) {
    LOG_SW(("Not possible to load file %s (%s)", _fileName.c_str(), error.GetMessage().c_str()));
    return;
  }

  // Add row with information
  au::StringVector values;
  values.push_back(_fileName);
  values.push_back(module->name);
  values.push_back(module->version);
  values.push_back(au::str(module->operations.size()));
  values.push_back(au::str(module->datas.size()));
  table->addRow(values);

  delete module;
}

void consider_directory(std::string directory, au::tables::Table *table) {
  DIR *dp;
  struct dirent *dirp;

  if ((dp  = opendir(directory.c_str())) == NULL) {
    return;
  }

  while ((dirp = readdir(dp)) != NULL) {
    std::string fileName = dirp->d_name;

    // Skip ".files"
    if (fileName.length() > 0) {
      if (fileName[0] == '.') {
        continue;  // Full path of the file
      }
    }
    std::string path = au::path_from_directory(directory, dirp->d_name);

    consider_file(path, table);
  }

  closedir(dp);
}

int main(int argC, const char *argV[]) {
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen",                 (void *)"only errors");
  paConfig("log file line format",          (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE] (p.PID) FUNC: TEXT");
  paConfig("screen line format",            (void *)"TYPE: TEXT");
  paConfig("log to file",                   (void *)true);
  paConfig("log to stderr",         (void *)true);

  paParse(paArgs, argC, (char **)argV, 1, false);      // No more pid in the log file name
  lmAux((char *)"father");
  logFd = lmFirstDiskFileDescriptor();


  au::StringVector fields("File", "Module", "Version", "#Operations", "#Datas");
  au::tables::Table table(fields);

  struct ::stat info;
  int error_stat;
  if ((error_stat = stat(file_name, &info)) < 0) {
    LM_E(("Error:%d at stat for file:'%s'", error_stat, file_name));
    std::cerr << "Error:" << error_stat << " at stat for file:" << file_name << std::endl;
    perror(file_name);
    exit(-1);
  }

  if (S_ISREG(info.st_mode)) {
    consider_file(file_name, &table);
  } else if (S_ISDIR(info.st_mode)) {
    consider_directory(file_name, &table);
  }
  table.setTitle("Module files");
  std::cout << table.str();
  std::cout << "\n";
}

