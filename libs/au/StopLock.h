
/* ****************************************************************************
 *
 * FILE            StopLock.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Wrapper of the condition type included in the pthreat library.
 *      It simplifies development of stop conditions
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef SAMSON_STOP_LOCK_H
#define SAMSON_STOP_LOCK_H

#include <pthread.h>             /* pthread_mutex_t */

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

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

NAMESPACE_END

#endif
