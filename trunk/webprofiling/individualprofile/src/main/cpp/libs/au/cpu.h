#ifndef _H_AU_CPU
#define _H_AU_CPU

#include <iostream>
#include <string>

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

struct cpu_info
{
    int num_logical_cores; // Number of logical cores
    int cores;             // Number of cores in the system
    bool hyperThreads;     // Flag to indicate if hyper-threading is supported
};

// Function to get information about cores and logical cores
int get_cpu_info( cpu_info* _info );

NAMESPACE_END

#endif
