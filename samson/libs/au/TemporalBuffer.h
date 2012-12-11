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
* Simple but useful class to work with a temporary allocated buffer
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

  // Reset buffer to another size
  void Reset(size_t size);

  // Accessors
  char *data();
  size_t size();

private:

  char *data_;
  size_t size_;
};
}

#endif  // ifndef AU_TEMPORAL_BUFFER_H_
