/* ****************************************************************************
*
* FILE            Object.h
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Object is the top class element in the engine libreary
*      It can receive notifications
*      It can be used as argument in a notification
*
* ****************************************************************************/

#ifndef _H_ENGINE_OBJECT
#define _H_ENGINE_OBJECT

#include <string>   // size_t

namespace engine {
class Notification;

/**
 * Base class for any element in the engine library to receive notifications
 */

class Object {
  // Class the managers this objects
  friend class ObjectsManager;

  // Unique identifier of this listener
  size_t engine_id;

  // Unique name
  const char *engine_name;

public:

  Object();                                  // Simple constructor ( object has an id )
  Object(const char *engine_name);           // Object can be accessed by this name anywhere
  virtual ~Object();                         // Destructor. ( virtual destructor is mandatory for generic remov

public:

  // Get my id as listener
  size_t getEngineId();

protected:

  // Start listening a particular notification
  void listen(const char *notification_name);

  // Method to receive a particular notification
  virtual void notify(Notification *notification);
};
}

#endif // ifndef _H_ENGINE_OBJECT
