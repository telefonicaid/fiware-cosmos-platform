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


#include "au/Object.h"

namespace engine {
/**
 * Information about a particular request
 */

class Buffer;

class MemoryRequest : public au::Object {
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
  MemoryRequest(size_t _size, double _mem_percentadge, size_t _listener_id);

  // Get information in xml mode
  void getInfo(std::ostringstream& output);
};
}

#endif // ifndef _H_MEMORY_REQUEST
