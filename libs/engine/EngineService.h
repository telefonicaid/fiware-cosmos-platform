
#ifndef _H_SAMSON_ENGINE_SERVICE
#define _H_SAMSON_ENGINE_SERVICE

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                         // std::cout
#include <set>                              // std::set

#include "au/list.h"                        // au::list
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

/**
 Main engine platform
 */


class EngineService
{
    
    std::string name;
    
public:
    
    EngineService( std::string _name );
    
    virtual ~EngineService()
    {
        // Just make sure destructor is virtual
    }
    
    std::string getEngineServiceName()
    {
        return name;
    }
    
    virtual void quitEngineService()
    {
        // function to quit background threads safely
    }
    
};

// Function to destroy all engine services when exit ( See engin::Engine::init() )
void destroy_engine_services();

NAMESPACE_END

#endif
