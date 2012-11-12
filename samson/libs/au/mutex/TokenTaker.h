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
 * FILE            TokenTaker
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Automatic management of a mutex
 *      Classical trick of retaining a mutex in the contructor and releasing in destructor
 *      Used in collaboration with au::Token class
 *
 * ****************************************************************************/

#ifndef _H_AU_TOCKEN_TAKER
#define _H_AU_TOCKEN_TAKER

#include <pthread.h>             /* pthread_mutex_t                          */

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

class Token;

class TokenTaker
{
    const char* name;
    Token* token;
    
public:
    
    TokenTaker( Token* _token );
    TokenTaker( Token* _token, const char* name );
    ~TokenTaker();
    
    void stop(  );
    //void stop( int time_out );
    
    void wakeUp();
    void wakeUpAll();
    
    
    
};

NAMESPACE_END

#endif
