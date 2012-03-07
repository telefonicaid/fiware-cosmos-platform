/* ****************************************************************************
*
* FILE            ProcessStats.cpp
*
* AUTHOR          Javier Lois
*
* DATE            January 2012
*
* DESCRIPTION
*
* Provides process statistics and info
*
*/

#include "ProcessStats.h"
//#include <iostream>
#include <sstream>
#include <fstream>


ProcessStats::ProcessStats()
{
    refresh();
}
ProcessStats::~ProcessStats()
{
}

//identifier of the process
int ProcessStats::get_pid()
{
    return pid;
}

//Number of threads running
unsigned long ProcessStats::get_nthreads()
{
    return nthreads;
}

//Size of the virtual memory in bytes
unsigned long ProcessStats::get_vmsize()
{
    return vsize;
}

//Re-read the data
void ProcessStats::refresh()
{
   std::ifstream stat_stream("/proc/self/stat",std::ios_base::in);
   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> nthreads >> itrealvalue >> starttime >> vsize >> rss; // There are more, should add them if they are ever needed
   stat_stream.close();
}
 
