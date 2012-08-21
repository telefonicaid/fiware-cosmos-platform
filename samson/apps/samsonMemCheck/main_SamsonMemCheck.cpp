#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/user.h>                           // PAGE_SIZE
#include <unistd.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/parseArgs.h"

#include "au/CommandLine.h"                     // au::CommandLine
#include "au/console/Console.h"                 // au::Console

#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "samson/common/coding.h"               // samson::FormatHeader
#include "samson/common/ports.h"
#include "samson/common/samsonVars.h"           // SAMSON_ARG_VARS SAMSON_ARGS
#include "samson/common/samsonVersion.h"
#include "samson/module/KVFormat.h"             // samson::KVFormat
#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#ifdef LINUX
#define KERNEL_SHMMAX "/proc/sys/kernel/shmmax"
#define KERNEL_SHMALL "/proc/sys/kernel/shmall"
#endif

#ifdef OSX
#define KERNEL_SHMMAX "kern.sysv.shmmax"
#define KERNEL_SHMALL "kern.sysv.shmall"
#endif



/* ****************************************************************************
 *
 * Option variables
 */

SAMSON_ARG_VARS;
bool check;


/* ****************************************************************************
 *
 * parse arguments
 */

PaArgument paArgs[] =
{
  SAMSON_ARGS,
  PA_END_OF_ARGS
};


#ifdef LINUX
void sysctl_value(char *param_name, long int *param_value) {
  FILE *fd_param;     /* file handle for paramname*/
  char *param_value_str = NULL;

  fd_param = fopen(param_name, "r");
  param_value_str = (char *)malloc(21);
  while (fgets(param_value_str, 20, fd_param) != NULL) {
    ;                                                   /* We assume that there are no parameters whose string length > 20 chars */
  }
  *param_value = strtol(param_value_str, NULL, 10);
  free(param_value_str);
  param_value_str = NULL;
  fclose(fd_param);
}

#endif

#ifdef OSX
void sysctl_value(char *param_name, long int *param_value) {
  size_t len = sizeof(&(param_value));

  if (sysctlbyname(param_name, param_value, &len, 0, 0))
    perror("sysctlbyname");
}

#endif


int main(int argC, const char *argV[]) {
  long int kernel_shmmax = 0;
  long int kernel_shmall = 0;
  long int max_memory_size = 0;
  long int needed_shmall = 0;

  long int num_processes = 0;
  long int shared_memory_size_per_buffer = 0;
  long int samson_required_mem = 0;

  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen",                 (void *)"only errors");
  paConfig("log file line format",          (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format",            (void *)"TYPE@TIME  EXEC: TEXT");
  paConfig("log to file",                   (void *)true);

  paParse(paArgs, argC, (char **)argV, 1, false);

  // SamsonSetup init
  samson::SamsonSetup::init(samsonHome, samsonWorking);
  samson::SamsonSetup::shared()->createWorkingDirectories();      // Create working directories

  // Fetch the current SAMSON configuration
  std::string num_processes_str = samson::SamsonSetup::shared()->getValueForParameter("general.num_processess");
  num_processes = strtol(num_processes_str.c_str(), NULL, 10);
  std::string shared_memory_size_per_buffer_str = samson::SamsonSetup::shared()->getValueForParameter(
    "general.shared_memory_size_per_buffer");
  shared_memory_size_per_buffer = strtol(shared_memory_size_per_buffer_str.c_str(), NULL, 10);
  samson_required_mem = num_processes * shared_memory_size_per_buffer;

#ifdef LINUX
  // Fetch the system config
  sysctl_value((char *)KERNEL_SHMMAX, &kernel_shmmax);
  sysctl_value((char *)KERNEL_SHMALL, &kernel_shmall);
#endif

  // max memory allowed  shmall * page_size
  max_memory_size = kernel_shmall * PAGE_SIZE;

  // Check to see if we can allocate all the memory needed
  if (samson_required_mem > max_memory_size) {
    needed_shmall = samson_required_mem / PAGE_SIZE;
    printf("Unable to allocate the needed memory for SAMSON. The system has %ld allocated and we need %ld.\n",
           max_memory_size,
           samson_required_mem);
    printf("Set kernel.shmall to %ld using the command 'sysctl -w kernel.shmall=%ld'.\n", needed_shmall, needed_shmall);
  } else {
    printf("Found enough shared memory for SAMSON, samson_required_mem(%ld) <= max_memory_size(%ld)\n",
           samson_required_mem,
           max_memory_size);
  }

  // Check to see if the segment size (shmmax) is big enough for each SAMSON buffer
  if (shared_memory_size_per_buffer > kernel_shmmax) {
    printf(
      "The system shared memory segment size (kernel.shmmax) is too small for samson. The system allows for a maximum size of %ld and we need %ld\n",
      kernel_shmmax, shared_memory_size_per_buffer);
    printf("Set kernel.shmmax to %ld using the command 'sysctl -w kernel.shmmax=%ld'.\n", shared_memory_size_per_buffer,
           shared_memory_size_per_buffer);
  } else {
    printf(
      "The maximum shared memory segment size is sufficent for SAMSON. shared_memory_size_per_buffer(%ld) <=  kernel_shmmax(%ld)\n",
      shared_memory_size_per_buffer,
      kernel_shmmax);
  }
}

