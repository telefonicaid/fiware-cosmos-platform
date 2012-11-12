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
