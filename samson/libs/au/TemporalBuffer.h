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

#ifndef AU_TEMPORAL_BUFFER_H_
#define AU_TEMPORAL_BUFFER_H_

#include <string>

namespace au {
class TemporalBuffer {
public:

  // Constructors & Destructors
  TemporalBuffer(size_t size);
  ~TemporalBuffer();

  // Accessors
  char *data();
  size_t size();

private:

  char *data_;
  size_t size_;
};
}

#endif  // ifndef AU_TEMPORAL_BUFFER_H_
