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

#ifndef _H_SAMSON_ENGINE_ELEMENT_COLLECTION
#define _H_SAMSON_ENGINE_ELEMENT_COLLECTION

#include <iostream>                              // std::cout
#include <list>
#include <pthread.h>
#include <set>                                   // std::set
#include <string>

#include "au/statistics/Cronometer.h"                       // au::Cronometer
#include "au/containers/list.h"                  // au::list
#include "au/mutex/Token.h"                      // au::Token
// NAMESPACE_BEGIN & }
#include "au/containers/vector.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "engine/NotificationListener.h"         // engine::EngineNotification
#include "engine/NotificationListenersManager.h"  // engine::NotificationListenersManager


namespace au {
class Error;
class Token;
}

namespace engine {
class EngineElement;
class ProcessItem;
class DiskOperation;
class Notification;

// ---------------------------------------------------
//
// EngineElementCollection
//
// ---------------------------------------------------

class EngineElementCollection {
public:

  EngineElementCollection();
  ~EngineElementCollection();

  // Unique method to add elements
  void Add(EngineElement *element);

  // Get next elements to be processed
  EngineElement *NextRepeatedEngineElement();
  EngineElement *NextNormalEngineElement();
  std::vector<EngineElement *> ExtraElements();

  double TimeForNextRepeatedEngineElement();

  bool IsEmpty();

  size_t GetNumEngineElements();
  size_t GetNumNormalEngineElements();
  size_t GetMaxWaitingTimeInEngineStack();

  std::string GetTableOfEngineElements();

  // Debug method to log all elements
  void PrintElements();

private:

  // Find the position in the list to inser a new element
  std::list<EngineElement *>::iterator FindPositionForRepeatedEngineElement(EngineElement *e);

  // Elements of the samson engine to be repeated periodically
  au::list<EngineElement> repeated_elements_;

  // General fifo elements of the SAMSON Engine
  au::list<EngineElement> normal_elements_;

  // Elements to be executed when nothing else has to be executed
  au::vector<EngineElement> extra_elements_;

  // Token to protect this list of elements
  au::Token token_;
};
}

#endif  // ifndef _H_SAMSON_ENGINE_ELEMENT_COLLECTION

