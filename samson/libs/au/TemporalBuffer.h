


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
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
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