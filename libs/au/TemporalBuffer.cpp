#include <stdlib.h>
#include "TemporalBuffer.h" // Own interface

namespace au 
{
    TemporalBuffer::TemporalBuffer( size_t size )
    {
        data = (char*) malloc(size);
    }
    
    TemporalBuffer::~TemporalBuffer()
    {
        if(data)
            free (data);
    }
}
