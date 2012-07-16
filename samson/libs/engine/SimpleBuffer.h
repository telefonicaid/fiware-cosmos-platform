
/* ****************************************************************************
 *
 * FILE            SimpleBuffer.h
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Simple struct containg information for a memory buffer
 *
 * ****************************************************************************/

#ifndef _H_SIMPLE_BUFFER
#define _H_SIMPLE_BUFFER

#include <cstring>			// size_t
#include <cstdlib>			// malloc, ...
#include <fstream>			// std::ifstream

#include "au/namespace.h"

NAMESPACE_BEGIN(engine)

/**
 Simple class to specify an allocated space in memory
 */

class SimpleBuffer
{
    
    char *_data;
    size_t _size;
    
public:
    
    SimpleBuffer()
    {
        _data = NULL;
        _size = 0;
    }
    
    SimpleBuffer( char *data , size_t size )
    {
        _data = data;
        _size = size;
    }
    
    bool checkSize( size_t size )
    {
        return ( _size >= size );
    }
    
    char* getData()
    {
        return _data;
    }
    
    size_t getSize()
    {
        return _size;
    }
    
};

NAMESPACE_END

#endif
