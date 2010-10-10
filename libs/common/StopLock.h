#ifndef SAMSON_STOP_LOCK_H
#define SAMSON_STOP_LOCK_H

/* ****************************************************************************
*
* FILE                 StopLock.h - 
*
* AUTHOR               Ken Zangelin, Andreu Urruela
*
* CREATION DATE        Oct 5 2010
*
*/
#include <pthread.h>             /* pthread_mutex_t */



namespace au
{
class Lock;

class StopLock
{
	friend class Lock;

	Lock*          lock;
	
public:	
	pthread_cond_t condition;

	StopLock(Lock* _lock)
	{
		lock = _lock;
		pthread_cond_init(&condition, NULL);
	}
};
}

#endif
