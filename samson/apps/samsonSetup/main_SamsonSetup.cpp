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

/*
 * samsonSetup
 *
 * Console interface for configuring a SAMSON node
 *
 * AUTHOR: Andreu Urruela
 */
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/parseArgs.h"

#include "au/CommandLine.h"                     // au::CommandLine
#include "au/console/Console.h"                 // au::Console
#include "au/console/ConsoleAutoComplete.h"

#include "samson/common/MemoryCheck.h"          // samson::MemoryCheck
#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "samson/common/coding.h"               // samson::FormatHeader
#include "samson/common/ports.h"
#include "samson/common/samsonVars.h"           // SAMSON_ARG_VARS SAMSON_ARGS
#include "samson/common/samsonVersion.h"
#include "samson/module/KVFormat.h"             // samson::KVFormat
#include "samson/module/ModulesManager.h"       // samson::ModulesManager




/* ****************************************************************************
 *
 * Option variables
 */

SAMSON_ARG_VARS;
bool show;
int port;

/* ****************************************************************************
 *
 * parse arguments
 */

PaArgument paArgs[] = {
  SAMSON_ARGS,
  { "-show",  &show, "", PaBool, PaOpt, false, false, true, "Show current options"                    },
  PA_END_OF_ARGS
};

int logFd = -1;
bool modified = false;  // Global variable to know if we have to save something before quiting..

std::string getHelpMessage() {
  std::ostringstream output;

  output << "-------------------------------------\n";
  output << "Help for samsonSetup tool\n";
  output << "-------------------------------------\n";
  output << "\n";
  output << " show                     Show all setup outputs\n";
  output << " set property value       Set a particular value\n";
  output << " save                     Save modified values\n";
  output << " use_desktop_values       Set typical values for desktop (2GB RAM & 2 cores)\n";
  output << " use_default_values       Set default values for a server (10GB RAM & 16 cores)\n";
  output << " shared_memory_check      Check the current kernel shared memory configuration is OK\n";
#ifdef LINUX
  output << " auto_configure           Generate a working setup.txt configuration based on the current system setup\n";
#endif  // LINUX
  output << " quit                     Quit samsonSetup tool\n";

  return output.str();
}

class SamsonConfigConsole : public au::Console {
public:

  std::string getPrompt() {
    return "samsonSetup > ";
  }

  void autoComplete(au::ConsoleAutoComplete *info) {
    if (info->completingFirstWord()) {
      info->add("show");
      info->add("help");
      info->add("set");
      info->add("save");
      info->add("use_desktop_values");
      info->add("use_default_values");
      info->add("shared_memory_check");
#ifdef LINUX
      info->add("auto_configure");
#endif  // LINUX
    }

    if (info->completingSecondWord("set")) {
      std::vector<std::string> names = au::Singleton<samson::SamsonSetup>::shared()->getItemNames();
      info->add(names);
    }

    if (info->completingThirdWord("set", "*")) {
      std::string parameter = info->secondWord();
      std::string current_value = au::Singleton<samson::SamsonSetup>::shared()->get(parameter);
      std::string default_value = au::Singleton<samson::SamsonSetup>::shared()->get_default(parameter);

      info->setHelpMessage(au::str("Current value %s ( default %s )", current_value.c_str(), default_value.c_str()));
    }
  }

  // function to process a command instroduced by user
  void evalCommand(std::string command) {
    au::CommandLine cmd;

    cmd.Parse(command);

    if (cmd.get_num_arguments() == 0) {
      return;
    }

    std::string main_command = cmd.get_argument(0);

    if (main_command == "help") {
      writeOnConsole(getHelpMessage());
      return;
    }

    if (main_command == "set") {
      if (cmd.get_num_arguments() < 3) {
        writeOnConsole("Usage: set property value");
      }
      std::string property = cmd.get_argument(1);
      std::string value = cmd.get_argument(2);

      if (!au::Singleton<samson::SamsonSetup>::shared()->isParameterDefined(property)) {
        writeErrorOnConsole(au::str("Parameter '%s' not defined", property.c_str()));
        return;
      }

      if (au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter(property, value)) {
        modified = true;
        writeWarningOnConsole(au::str("Property '%s' set to '%s'", property.c_str(), value.c_str()));
      } else {
        writeWarningOnConsole(au::str("'%s' is not valid for property", value.c_str(), property.c_str()));
      }

      return;
    }

#ifdef LINUX
    if (main_command == "auto_configure") {
      samson_sysctl_t kernel_shmmax = 0;
      const char *KERNEL_SHMMAX = "/proc/sys/kernel/shmmax";

      // Determine how much memory we have access to
      int64_t physical_ram = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE);
      // Determine how many CPUs we have
      short int no_cpus = sysconf(_SC_NPROCESSORS_ONLN);
      // Fetch the current max shared memory segment size
      samson::sysctl_value(const_cast<char *>(KERNEL_SHMMAX), &kernel_shmmax);

      au::Singleton<samson::SamsonSetup>::shared()->resetToDefaultValues();
      au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("general.memory", au::str("%lld", physical_ram));
      au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("general.num_processess",
                                                                         au::str("%d", no_cpus));
      au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("general.shared_memory_size_per_buffer",
                                                                         au::str("%ld", kernel_shmmax));
      au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("stream.max_operation_input_size",
                                                                         au::str("%ld", kernel_shmmax));

      writeWarningOnConsole(au::str("Properties general.memory                        = %lld", physical_ram));
      writeWarningOnConsole(au::str("Properties general.num_processess                = %d", no_cpus));
      writeWarningOnConsole(au::str("Properties general.shared_memory_size_per_buffer = %ld", kernel_shmmax));
      writeWarningOnConsole(au::str("Properties stream.max_operation_input_size       = %ld", kernel_shmmax));

      modified = true;

      return;
    }
#endif  // LINUX

    if (main_command == "use_desktop_values") {
      au::Singleton<samson::SamsonSetup>::shared()->resetToDefaultValues();
      au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("general.memory", "2000000000");
      au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("general.num_processess", "2");
      au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("general.shared_memory_size_per_buffer",
                                                                         "64000000");
      au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("stream.max_operation_input_size", "64000000");

      writeWarningOnConsole("Properties general.memory                        = 2Gb");
      writeWarningOnConsole("Properties general.num_processess                = 2");
      writeWarningOnConsole("Properties general.shared_memory_size_per_buffer = 64Mb");
      writeWarningOnConsole("Properties stream.max_operation_input_size       = 64Mb");

      modified = true;

      return;
    }

    if (main_command == "use_default_values") {
      au::Singleton<samson::SamsonSetup>::shared()->clearCustumValues();
      writeOnConsole("OK");
      modified = true;
      return;
    }


    if (main_command == "save") {
      int res = au::Singleton<samson::SamsonSetup>::shared()->save();        // Save a new file with the current setup

      std::string fileName = au::Singleton<samson::SamsonSetup>::shared()->setupFileName();

      if (!res) {
        writeWarningOnConsole(au::str("Saved file at %s", fileName.c_str()));
        modified = false;
      } else {
        writeErrorOnConsole(au::str("Error saving file at %s", fileName.c_str()));
      }


      return;
    }


    if (main_command == "quit") {
      if (modified) {
        std::cout << "Setup file modified.... save (y/n)? ";
        char line[1024];
        if (fgets(line, 1024, stdin) == NULL) {
          writeWarningOnConsole("Read nothing");
        }
        if (strcmp(line, "y") || strcmp(line, "Y")) {
          au::Singleton<samson::SamsonSetup>::shared()->save();            // Save a new file with the current setup
          std::string fileName = au::Singleton<samson::SamsonSetup>::shared()->setupFileName();
          writeWarningOnConsole(au::str("Saved file at %s", fileName.c_str()));
        }
      }

      quitConsole();
      return;
    }

    if (main_command == "show") {
      std::cout << au::Singleton<samson::SamsonSetup>::shared()->str();
      return;
    }

    if (main_command == "shared_memory_check") {
      if (samson::MemoryCheck() == false) {
        std::cout << "Insufficient shared memory configured. Revise your kernel configuration.\n";
      } else {
        std::cout << "Kernel shared memory config OK.\n";
      } return;
    }


    writeErrorOnConsole(au::str("Unknown command '%s'", main_command.c_str()));
    writeWarningOnConsole("Type help to get a list of valid commands ");
  }
};





/* ****************************************************************************
 *
 * man texts -
 */
static const char *manSynopsis         = " [OPTION]";
static const char *manShortDescription = "samsonSetup is a tool to define setup parameters in a SAMSON system.\n\n";
static const char *manDescription      =
  "samsonSetup [-show] : Samson configuration tool.\n"
  "\n"
  "\t -show : Show all current options\n"
  "\n"
  "\t samsonSetup is an interactive tool to specify all the setup values in a Samson deployment\n"
  "\t Just type help inside samsonSetup to get a list of valid commands\n"
  "\n";

static const char *manExitStatus    = "0      if OK\n 1-255  error\n";
static const char *manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char *manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char *manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char *manVersion       = SAMSON_VERSION;



/* ****************************************************************************
 *
 * main -
 */
int main(int argC, const char *argV[]) {
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen",                 (void *)"only errors");
  paConfig("log file line format",          (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format",            (void *)"TYPE@TIME  EXEC: TEXT");
  paConfig("log to file",                   (void *)true);

  paConfig("man synopsis",                  (void *)manSynopsis);
  paConfig("man shortdescription",          (void *)manShortDescription);
  paConfig("man description",               (void *)manDescription);
  paConfig("man exitstatus",                (void *)manExitStatus);
  paConfig("man author",                    (void *)manAuthor);
  paConfig("man reportingbugs",             (void *)manReportingBugs);
  paConfig("man copyright",                 (void *)manCopyright);
  paConfig("man version",                   (void *)manVersion);

  paParse(paArgs, argC, (char **)argV, 1, false);

  // SamsonSetup init
  au::Singleton<samson::SamsonSetup>::shared()->SetWorkerDirectories(samsonHome, samsonWorking);

  if (show) {
    std::cout << au::Singleton<samson::SamsonSetup>::shared()->str();
    return 0;
  }

  std::cout << "\nType help to get a list of valid command\n\n";
  SamsonConfigConsole console;
  console.runConsole();

  // Show samson setup
  // std::cout << au::Singleton<samson::SamsonSetup>::shared()->str();
}

