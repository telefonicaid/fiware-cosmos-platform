
#include <dirent.h>                       // DIR directory header
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <sys/stat.h>                     // stat(.)
#include <sys/types.h>
#include <unistd.h>

#include "au/CommandLine.h"               // au::CommandLine

#include "logMsg/logMsg.h"                // LM_*
#include "logMsg/traceLevels.h"           // Trace Levels

#include "parseArgs/parseArgs.h"

#include "au/mutex/LockDebugger.h"        // au::LockDebugger

#include "engine/MemoryManager.h"

#include "samson/common/SamsonSetup.h"    // samson::SamsonSetup
#include "samson/common/coding.h"         // samson::FormatHeader
#include "samson/common/samsonVars.h"     // SAMSON_ARG_VARS

#include "samson/module/KVFormat.h"       // samson::KVFormat
#include "samson/module/ModulesManager.h" // samson::ModulesManager

#include "samson/common/SamsonDataSet.h"  // samson::SamsonDataSet
#include "samson/common/SamsonFile.h"     // samson::SamsonFile


/* ****************************************************************************
 *
 * Option variables
 */

bool show_header;
bool show_hg;
bool show_hg_per_kvs;
size_t show_limit;
char file_name[1024];

SAMSON_ARG_VARS;

/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
  SAMSON_ARGS,
  { "-limit", &show_limit,     "",              PaInt,               PaOpt,        0,                0,               100000,
    "Limit in the number of records"           },
  { "-header",&show_header,          "",            PaBool,       PaOpt,     false,           false,  true,      "Show only header"                         },
  { "-hg",    &show_hg,              "",            PaBool,       PaOpt,     false,           false,  true,
    "Show only hash-group information"         },
  { "-show_hg",&show_hg_per_kvs,      "",            PaBool,       PaOpt,     false,           false,  true,
    "Show hash-group for each key-value"       },
  { " ",      file_name,             "",            PaString,     PaReq,     (long)"null",    PaNL,   PaNL,
    "name of the file or directory to scan"    },
  PA_END_OF_ARGS
};

/* ****************************************************************************
 *
 * logFd - file descriptor for log file used in all libraries
 */
int logFd = -1;




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

  engine::MemoryManager::init(1024 * 1024 * 1024);

  samson::SamsonSetup::init(samsonHome, samsonWorking);
  samson::ModulesManager::init("samsonCat");


  struct stat filestatus;
  int error_stat;
  if ((error_stat = stat(file_name, &filestatus)) < 0) {
    LM_E(("Error:%d at stat for file:'%s'", error_stat, file_name));
    std::cerr << "Error:" << error_stat << " at stat for file:" << file_name << std::endl;
    perror(file_name);
    exit(-1);
  }

  if (S_ISREG(filestatus.st_mode)) {
    // Open a single file
    au::ErrorManager error;
    au::SharedPointer<samson::SamsonFile> samson_file = samson::SamsonFile::create(file_name, error);

    if (error.IsActivated())
      LM_X(1, ("%s", error.GetMessage().c_str()));
    // Show header only
    if (show_header) {
      samson_file->printHeader(std::cout);
      exit(0);
    }

    // Show all hash-groups
    if (show_hg) {
      samson_file->printHashGroups(std::cout);
      exit(0);
    }

    // Print content
    samson_file->printContent(show_limit, show_hg_per_kvs, std::cout);
  } else if (S_ISDIR(filestatus.st_mode)) {
    // Open a single file
    au::ErrorManager error;
    au::SharedPointer<samson::SamsonDataSet> samson_data_set = samson::SamsonDataSet::create(file_name, error);

    if (error.IsActivated())
      LM_X(1, ( error.GetMessage().c_str()));
    if (show_header) {
      std::cout << "Total: " << samson_data_set->info().strDetailed() << "\n";
      samson_data_set->printHeaders(std::cout);
      exit(0);
    }

    if (show_hg) {
      samson_data_set->printHashGroups(std::cout);
      exit(0);
    }

    samson_data_set->printContent(show_limit, show_hg_per_kvs, std::cout);
  } else {
    LM_X(1, ("%s is not a file or a directory", file_name));
  }

  engine::MemoryManager::destroy();
}

