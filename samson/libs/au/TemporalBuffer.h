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
 * FILE            TemporalBuffer.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            April 2012
 *
 * DESCRIPTION
 *
 * Simple but usefull class to work with a temporary allocated buffer
 *
 * ****************************************************************************/

#ifndef _H_AU_TEMPORAL_BUFFER
#define _H_AU_TEMPORAL_BUFFER

#include <string>

namespace au {
    
    class TemporalBuffer
    {
        
    public:
        
        char *data;
        
        TemporalBuffer( size_t size );
        ~TemporalBuffer();
        
    };
    
}

#endif
