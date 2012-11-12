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

    void stop();
    
};

NAMESPACE_END

#endif
