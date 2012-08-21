
#ifndef _H_SAMSON_ENGINE_ELEMENT_COLLECTION
#define _H_SAMSON_ENGINE_ELEMENT_COLLECTION

#include <iostream>                         // std::cout
#include <list>
#include <pthread.h>
#include <set>                              // std::set
#include <string>

#include "au/Cronometer.h"                  // au::Cronometer
#include "au/containers/list.h"             // au::list
#include "au/mutex/Token.h"                 // au::Token
// NAMESPACE_BEGIN & }
#include "au/containers/vector.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "engine/Object.h"                  // engine::EngineNotification
#include "engine/ObjectsManager.h"          // engine::ObjectsManager


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
  // Elements of the samson engine to be repeated periodically
  au::list<EngineElement> repeated_elements;

  // General fifo elements of the SAMSON Engine
  au::list<EngineElement> normal_elements;

  // Elements to be executed when nothing else has to be executed
  au::vector<EngineElement> extra_elements;

  // Token to protect this list of elements
  au::Token token;


public:

  EngineElementCollection();
  ~EngineElementCollection();

  void add(EngineElement *element);

  EngineElement *getNextRepeatedEngineElement();
  EngineElement *getNextNormalEngineElement();
  std::vector<EngineElement *> getExtraElements();

  double getTimeForNextRepeatedEngineElement();

  bool isEmpty();

  size_t getNumEngineElements();
  size_t getNumNormalEngineElements();

  size_t getMaxWaitingTimeInEngineStack();

  void print_elements();

  std::string getTableOfEngineElements();

private:

  // Find the position in the list to inser a new element
  std::list<EngineElement *>::iterator _find_pos_in_repeated_elements(EngineElement *e);
};
}

#endif  // ifndef _H_SAMSON_ENGINE_ELEMENT_COLLECTION

