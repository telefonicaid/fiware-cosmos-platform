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

#include "au/containers/Queue.h"
#include "au/containers/map.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "engine/Buffer.h"

#ifndef _H_Buffercontainer_SamsonClient
#define _H_Buffercontainer_SamsonClient


namespace  samson {
class BufferContainer {
public:

  BufferContainer() : token_("BufferContainer") {
  }

  ~BufferContainer() {
  }

  void Push(const std::string& queue_name, engine::BufferPointer buffer);
  engine::BufferPointer Pop(const std::string& queue_name);

private:

  au::Token token_;   // Mutex protection
  au::map< std::string, au::Queue<engine::Buffer> > queues_;
};
}

#endif  // ifndef _H_Buffercontainer_SamsonClient