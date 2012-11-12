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

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#ifndef _H_AU_SINGLETON
#define _H_AU_SINGLETON


namespace au {
    
    template< class C >
    class Singleton
    {
        static au::Token token;
        static C* instance;
        
    public:
        
        static C* shared()
        {
            au::TokenTaker tt(&token);
            if( !instance )
                instance = new C();
            return instance;
        }
        
    };
    
    // Static members
    template <class C> C* Singleton<C>::instance;
    template <class C> au::Token Singleton<C>::token("singleton");
    
} // end of au namesapce

#endif
