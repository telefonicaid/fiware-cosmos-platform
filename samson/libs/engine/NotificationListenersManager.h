/* ****************************************************************************
*
* FILE            ObjectsManager
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Managert of all the objets in engine library
*   It keep tracks of all the objects with a map for fast notification delivery
*
* ****************************************************************************/

#ifndef _H_ENGINE_OBJECTS_MANAGER
#define _H_ENGINE_OBJECTS_MANAGER


#include <string>       // size_t

#include "au/containers/map.h"     // au::map
#include "au/containers/simple_map.h"     // au::simple_map
#include "au/mutex/Token.h"     // au::Token
#include "au/mutex/TokenTaker.h"


namespace engine {
class NotificationListener;
class Notification;

// IdsCollection: collection of ids to send notifications

class IdsCollection {
public:

  std::set<size_t> ids_;

  void add(size_t id) {
    ids_.insert(id);
  }

  void remove(size_t id) {
    ids_.erase(id);
  }

  void addTo(std::set<size_t> &ids) {
    std::set<size_t>::iterator it;
    for (it = ids_.begin(); it != ids_.end(); it++) {
      ids.insert(*it);
    }
  }
};

/**
 *
 * Global class to manage all notifications listeners
 *
 **/

class NotificationListenersManager {

public:

  NotificationListenersManager();
  ~NotificationListenersManager();

  // Add and remove Objects
  void Add(NotificationListener *o);
  void Remove(NotificationListener *o);

  // Add and remove objects to channels
  void AddToChannel(NotificationListener *o, const char *name);
  void RemoveFromChannel(NotificationListener *o, const char *name);

  // Send a notification
  void Send(Notification *notification);

private:

  // Get the collections of ids for a particular channel
  IdsCollection *GetListenersForChannel(const char *name);

  // Internal funciton to send a notification to a particular target-objetc ( if exist )
  void Send(Notification *notification, size_t target);

  // Recover ids for a particular channel
  std::set<size_t> GetEndgineIdsForChannel(const char *name);

  // Internal variable to create new objects
  size_t engine_id_;
  
  // Map of all the objects by id
  au::map < size_t, NotificationListener > objects_;
  
  // Map of ids object per notification channel
  au::map< const char *, IdsCollection, au::strCompare > channels_;
  
  // Mutex protection
  au::Token token_;

  
};
}


#endif  // ifndef _H_ENGINE_OBJECTS_MANAGER