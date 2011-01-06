#ifndef SAMSON_LOCK_H
#define SAMSON_LOCK_H

/* ****************************************************************************
*
* FILE                 Lock.h - 
*
* AUTHOR               Ken Zangelin, Andreu Urruela
*
* CREATION DATE        Oct 5 2010
*
*/
#include <pthread.h>             /* pthread_mutex_t                          */
#include <assert.h>              /* assert                                   */
#include <string>                /* std::string                              */

#include "StopLock.h"            /* StopLock                                 */

#define DEBUG_SS_THREADS

namespace au {

class Lock 
{
	pthread_mutex_t _lock;
	
public:	
	std::string description;
	
	Lock();
	~Lock();
	
	void lock();
	void unlock();
	
	void unlock_waiting_in_stopLock(StopLock* stopLock);
	void unlock_waiting_in_stopLock(StopLock* stopLock, int max_seconds);
	void wakeUpStopLock(StopLock* stopLock);
	void wakeUpAllStopLock(StopLock* stopLock);
};
}

#endif
