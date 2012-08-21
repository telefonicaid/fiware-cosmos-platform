/* ****************************************************************************
*
* FILE            Buffer.h
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Buffer is a memory region allocated and controlled by MemoryManager
* It can be requested and used by any element in the application
* It can only be destroyed calling "destroyBuffer" at MemoryMamager
* It comes with some usufull function to interact with its contect
*
* ****************************************************************************/

#ifndef _ENGINE_BUFFER_H_
#define _ENGINE_BUFFER_H_

#include <cstdlib>                      // malloc, ...
#include <cstring>                      // size_t
#include <fstream>                      // std::ifstream

#include "au/containers/SharedPointer.h"
#include "au/mutex/Token.h"
#include "engine/Notification.h"
#include "engine/Object.h"              // engine::EngineNotificationObject
#include "engine/SimpleBuffer.h"        // engine::SimpleBuffer

namespace engine {
/**
 * Buffer class to hold data managed by MemoryManager
 */

class Buffer : public NotificationObject {
  Buffer(const std::string& name, const std::string& type, size_t max_size);

public:

  static au::SharedPointer<Buffer> create(const std::string& name, const std::string& type, size_t max_size);
  ~Buffer();

  // Get the maximum size of the buffer
  size_t getMaxSize();

  // Get used size of this buffer ( not necessary the maximum )
  size_t getSize();

  // Get internal name for debuggin
  std::string getName();

  // Get internal type for debuggin
  std::string getType();

  // Get a description of the buffer ( debugging )
  std::string str();

  // Set internal name and type for debuggin
  void setNameAndType(std::string name, std::string type);

  // Add string to the internal name
  void addToName(std::string description);

  // Write content to this buffer
  bool write(const char *input_buffer, size_t input_size);

  // Skip some space without writing anything ( usefull to fix-size headers )
  bool skipWrite(size_t size);

  // Write on the buffer the maximum possible ammount of data
  void write(std::ifstream &inputStream);

  // Get available space to write with "write call"
  size_t getAvailableWrite();

  // Reading content from the buffer
  // ------------------------------------------

  // Skip some space without reading
  size_t skipRead(size_t size);

  // Read command
  size_t read(char *output_buffer, size_t output_size);

  // Get pending bytes to be read
  size_t getSizePendingRead();

  // Manual manipulation of data
  // ------------------------------------------

  // Get a pointer to the data full space ( not related with write calls )
  char *getData();

  // Set used size manually
  void setSize(size_t size);

  // Interface with SimpleBuffer ( simplified view of the internal buffer )
  // ------------------------------------------

  SimpleBuffer getSimpleBuffer();
  SimpleBuffer getSimpleBufferAtOffset(size_t offset);

  // Spetial functions
  // ------------------------------------------

  // Remove the last characters of an unfinished line and put them in buffer.
  int removeLastUnfinishedLine(char *& buffer, size_t& buffer_size);


private:

  char *data_;                  // Buffer of data
  size_t max_size_;             // Maximum size of this buffer

  std::string name_;            // Internal name for debugging
  std::string type_;      // Identifier of groups of blocks

  /**
   * Current size used in this buffer
   * This is the one that should be used when transmitting the buffer across the network
   * This variable is updated with calls to "write" or set manually in setSize
   */

  size_t size_;

  /**
   * Internal variable used for the read process
   */

  size_t offset_;
};


// Buffer Shared Pointer
typedef au::SharedPointer<Buffer>   BufferPointer;
}

#endif // ifndef _ENGINE_BUFFER_H_
