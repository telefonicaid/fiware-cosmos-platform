
/* ****************************************************************************
 *
 * FILE            Token
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *  Wrapper of the mutex included in pthread library.
 *  Used to simplify development of multi-thread apps
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_TOCKEN
#define _H_AU_TOCKEN

#include <pthread.h>             /* pthread_mutex_t                          */

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

class Token
{
    const char * name;              // Name of the token
    
    pthread_mutex_t _lock;			// Mutex to protect this tocken
    pthread_cond_t _block;          // Condition to block threads that call stop
    
    // Mechanism to discover if you have locked this mutex
    pthread_t t;
    bool locked;
    int counter; // Number of times this token is taken
    
public:
    
    Token( const char * name );
    ~Token();
    
private:
    
    // It is only possible to retain teh token with the class TokenTaker
    friend class TokenTaker;
    
    void retain();
    void release();
    
};

NAMESPACE_END

#endif
