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
 * FILE            MemoryRequest
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Request to be delivered to MemoryManager.
 * It is used where we want to allocate a buffer of memory when available.
 *
 * ****************************************************************************/

#ifndef _H_MEMORY_REQUEST
#define _H_MEMORY_REQUEST


#include <cstring>

#include "au/namespace.h"
#include "au/Object.h"

NAMESPACE_BEGIN(engine)

/**
 Information about a particular request
 */

class Buffer;

class MemoryRequest : public au::Object
{
    
public:
    
    // Size of the memory requets ( extracted from environment )
    size_t size;
    
    // Percentadge of the memory to give this memory block
    double mem_percentadge;
    
    // Listner to notify
    size_t listner_id;
    
    // Buffer alocated for this
    Buffer *buffer;
    
    // Basic constructor
    MemoryRequest( size_t _size , double _mem_percentadge , size_t _listener_id );
    
    // Get information in xml mode
    void getInfo( std::ostringstream& output);
    
    
};    


NAMESPACE_END

#endif
