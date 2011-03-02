#ifndef PLATFORM_PROCESSES_H
#define PLATFORM_PROCESSES_H

/* ****************************************************************************
*
* FILE                     platformProcesses.h - get/set platform process list
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Mar 02 2011
*
*/
#include "Process.h"            // Process


namespace ss
{



/* ****************************************************************************
*
* platformProcessesSave - 
*/
extern void platformProcessesSave(ProcessVector* wvP);



/* ****************************************************************************
*
* platformProcessesGet -
*/
extern ProcessVector* platformProcessesGet(int* sizeP);

}

#endif
