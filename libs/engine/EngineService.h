
#ifndef _H_SAMSON_ENGINE_SERVICE
#define _H_SAMSON_ENGINE_SERVICE

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                         // std::cout
#include <set>                              // std::set

#include "au/containers/list.h"                        // au::list
#include "au/Token.h"                       // au::Token
#include "au/Cronometer.h"                  // au::Cronometer
#include "au/namespace.h"                // NAMESPACE_BEGIN & NAMESPACE_END

#include "engine/Object.h"      // engine::EngineNotification
#include "engine/ObjectsManager.h"          // engine::ObjectsManager

NAMESPACE_BEGIN(engine)
class Error;
class Token;
NAMESPACE_END

NAMESPACE_BEGIN(engine)

class EngineElement;
class ProcessItem;
class DiskOperation;
class Notification;

NAMESPACE_END

#endif
