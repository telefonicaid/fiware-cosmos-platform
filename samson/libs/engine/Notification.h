/* ****************************************************************************
 *
 * FILE            Notification.h
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Notification message in the engine library
 * It is basically an "object-C" message kind of thing
 *
 *
 * --> A notification is a "message" sent in the main-thread runloop to a set of objects ( targets )
 * --> The notification is also delivered to all the objects that "listen"
 *     the notification chanel ( defined by notification's name)
 * --> Internall information is contained in environemnt variable and the optional object parameter
 *
 * ****************************************************************************/

#ifndef _H_ENGINE_NOTIFICATION
#define _H_ENGINE_NOTIFICATION

#include <string.h>

#include <set>                  // std::set
#include <string>

#include "au/containers/Dictionary.h"
#include "au/containers/GeneralDictionary.h"
#include "au/Environment.h"     // au::Enrivonment

namespace engine {

class Notification {
  public:
    // Constructor and destructor
    explicit Notification(const char *_name);
    ~Notification();

    // Add listeners
    void AddEngineListener(size_t listener_id);
    void AddEngineListeners(const std::set<size_t>& _listeners_id);

    // Accessorts
    const char *name();
    au::GeneralDictionary& dictionary();
    au::Environment& environment();
    bool isName(const char *name);
    const std::set<size_t>& targets();

    // Get a string for debug
    std::string GetDescription();
    std::string GetShortDescription();

  private:
    // Name of the notification
    const char *name_;

    // Generic dictionary ob objects ( shared pointers )
    au::GeneralDictionary dictionary_;

    // Identifiers that should receive this notification
    std::set<size_t> targets_;

    // Dictionary of properties for maximum flexibility
    au::Environment environment_;

    friend class ObjectsManager;
};
}

#endif  // ifndef _H_ENGINE_NOTIFICATION
