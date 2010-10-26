#ifndef WORKER_STATUS_H
#define WORKER_STATUS_H

/* ****************************************************************************
*
* FILE                      workerStatus.h
*
* DESCRIPTION				extraction of machine information
*
*/
#include "samson.pb.h"          // WorkerStatus


namespace ss {


/* ****************************************************************************
*
* Definitions
*/
#define MAX_CORES        32
#define MAX_NETIFS       16



/* ****************************************************************************
*
* workerStatus
*/
extern void workerStatus(ss::network::WorkerStatus* wsP);

}

#endif
