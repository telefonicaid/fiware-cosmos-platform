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
* FILE            ProcessStats.h
*
* AUTHOR          Javier Lois
*
* DATE            January 2012
*
* DESCRIPTION
*
* Provides process statistics and info
* Gets the information from the pseudo-file /proc/<pid>/stat
*/

#include <string>

class ProcessStats
{

    //possible fields in /proc/<pid>/stat
    int pid;
    std::string comm, state, ppid, pgrp, session, tty_nr;
    std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    std::string utime, stime, cutime, cstime, priority, nice;
    std::string itrealvalue, starttime;
    unsigned long nthreads, vsize, rss;

public:
    ProcessStats();
    ~ProcessStats();
    
    //identifier of the process
    int get_pid();
    
    //Number of threads running
    unsigned long get_nthreads();
    
    //Size of the virtual memory in bytes
    unsigned long get_vmsize();
    
    //Re-read the data
    void refresh();
    
};
