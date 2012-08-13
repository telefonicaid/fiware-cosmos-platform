#include "samson/common/MemoryCheck.h"
#include <sys/types.h>

#if defined(__sun__)
#include <rctl.h>
#else
#include <sys/sysctl.h>
#endif  /* __sun__ */

namespace samson
{
    SAMSON_EXTERNAL_VARS;
    SAMSON_ARG_VARS;

#if defined(LINUX)
    void sysctl_value(char *param_name, samson_sysctl_t *param_value)
    {
        FILE         *fd_param;     /* file handle for paramname*/
        char         *param_value_str = NULL;

        fd_param = fopen (param_name, "r");
        param_value_str = (char *) malloc(21);
        while (fgets(param_value_str, 20, fd_param) != NULL); /* We assume that there are no parameters whose string length > 20 chars */
        *param_value=strtol(param_value_str, NULL, 10);
        free(param_value_str);
        param_value_str = NULL;
        fclose(fd_param);
    }
#elif defined(OSX)
    void sysctl_value(char *param_name, samson_sysctl_t *param_value)
    {
        size_t len = sizeof(&(param_value));

        if (sysctlbyname(param_name, param_value, &len, 0, 0) )
        {  
            perror( "sysctlbyname" );
        }
    }
#elif defined(__sun__)
    void sysctl_value(char *param_name, samson_sysctl_t *param_value)
    {
        rctlblk_t *rblk = NULL;

        if ((rblk = (rctlblk_t *)malloc(rctlblk_size())) == NULL) 
        {
            (void) fprintf(stderr, "malloc failed: %s\n", strerror(errno));
        }
 
        if (getrctl(param_name, NULL, rblk, RCTL_FIRST) == -1)
        {
            (void) fprintf(stderr, "failed to get %s: %s\n", param_name, strerror(errno));
        }
        else
        {
            *param_value=rctlblk_get_value(rblk);
        }
        if (rblk)
        {
            free(rblk);
        }
    }
#endif /* LINUX */

    bool MemoryCheck()
    {
        samson_sysctl_t kernel_shmmax = 0;

#if !defined(__sun__)
        samson_sysctl_t kernel_shmall = 0;
        int page_size = 0;
        long int max_memory_size = 0;
        long int needed_shmall = 0;
        long int num_processes = 0;
        long int samson_required_mem = 0;
#endif  /* !__sun__ */ 

        unsigned long int shared_memory_size_per_buffer = 0;

#if defined(LINUX)
        const char *KERNEL_SHMMAX = "/proc/sys/kernel/shmmax";
        const char *KERNEL_SHMALL = "/proc/sys/kernel/shmall";
        const char *SYSCTL_SHMMAX = "kernel.shmmax";
        const char *SYSCTL_SHMALL = "kernel.shmall";
#elif defined(OSX)
        const char *KERNEL_SHMMAX = "kern.sysv.shmmax";
        const char *KERNEL_SHMALL = "kern.sysv.shmall";
        const char *SYSCTL_SHMMAX = "kern.sysv.shmmax";
        const char *SYSCTL_SHMALL = "kern.sysv.shmall";
#elif defined(__sun__)
        const char *KERNEL_SHMMAX = "project.max-shm-memory";
        const char *SYSCTL_SHMMAX = "project.max-shm-memory";
#endif /* LINUX */


        // Fetch the current SAMSON configuration
        std::string shared_memory_size_per_buffer_str = samson::SamsonSetup::shared()->getValueForParameter("general.shared_memory_size_per_buffer");
        shared_memory_size_per_buffer = strtoul(shared_memory_size_per_buffer_str.c_str(), NULL, 10);

        // Fetch the system config
        sysctl_value((char *)KERNEL_SHMMAX, &kernel_shmmax);
#if !defined(__sun__)
        sysctl_value((char *)KERNEL_SHMALL, &kernel_shmall);

        // Fetch the current page size
        page_size = getpagesize();

        // max memory allowed  shmall * page_size
        max_memory_size = kernel_shmall * page_size;

        std::string num_processes_str = samson::SamsonSetup::shared()->getValueForParameter("general.num_processess");
        num_processes = strtol(num_processes_str.c_str(), NULL, 10);
        samson_required_mem = num_processes * shared_memory_size_per_buffer;
        // Check to see if we can allocate all the memory needed
        if ( samson_required_mem > max_memory_size )
        {
            needed_shmall = samson_required_mem / page_size;
            LM_E(("Unable to allocate the needed memory for SAMSON. The system has %ld allocated and we need %ld.", max_memory_size, samson_required_mem));
            LM_E(("Set kernel.shmall to %ld using the command 'sudo sysctl -w %s=%ld'.", needed_shmall, SYSCTL_SHMALL, needed_shmall));
            return false;
        }
#endif  /* !__sun__ */

        // Check to see if the segment size (shmmax) is big enough for each SAMSON buffer
        if ( shared_memory_size_per_buffer > kernel_shmmax )
        {
            LM_E(("The system shared memory segment size (kernel.shmmax) is too small for SAMSON. The system allows for a maximum size of %ld and we need %ld", kernel_shmmax, shared_memory_size_per_buffer ));
            /* TODO - Determine the correct command for Solaris */
            LM_E(("Set kernel.shmmax to %ld using the command 'sudo sysctl -w %s=%ld'.", shared_memory_size_per_buffer, SYSCTL_SHMMAX, shared_memory_size_per_buffer ));
            return false;
        }
        return true;
    }
}
