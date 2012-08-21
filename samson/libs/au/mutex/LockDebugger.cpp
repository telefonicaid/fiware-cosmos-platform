

#include <assert.h>
#include <iostream>              /* std::cout                                */
#include <map>                   /* map                                      */
#include <pthread.h>             /* pthread_mutex_t                          */
#include <set>                   /* set                                      */
#include <sstream>               /* std::ostringstream                       */
#include <string>                /* std::string                              */

#include "logMsg/logMsg.h"       // LM_M()

#include "LockDebugger.h"        /* Own interface                            */

// #define DEBUG_AU_THREADS
// #define FULL_DEBUG_AU_THREADS

namespace au {
class LockDebugger;
LockDebugger *lockDebugger;

LockDebugger *LockDebugger::shared() {
  if (!lockDebugger)
    lockDebugger = new LockDebugger(); return lockDebugger;
}

LockDebugger::LockDebugger() {
  pthread_mutex_init(&_lock, 0);
  pthread_key_create(&key_title, NULL);
}

LockDebugger::~LockDebugger() {
  pthread_mutex_destroy(&_lock);
  pthread_key_delete(key_title);
}

void LockDebugger::destroy() {
  if (!lockDebugger)
    LM_RVE(("Attempt to destroy a non-initialized Lock Debugger")); LM_V(("Destroying ModulesManager"));
  delete lockDebugger;

  lockDebugger = NULL;
}

void LockDebugger::add_lock(void *new_lock) {
  // Lock private data

  // Block until the mutex is free
  int ans = pthread_mutex_lock(&_lock);

  // Make sure there are no errors with lock
  if (ans != 0)
    LM_X(1, ("pthread_mutex_lock error"));
  std::set<void *> *locksVector = _getLocksVector();


  // Make some checks here...

  // We do not autoblock
  if (locksVector->find(new_lock) !=  locksVector->end())
    LM_X(1, ("Autolock detected"));

  // We are not blocked
  if (_cross_blocking(new_lock)) {
    assert(false);
    LM_X(1, ("Cross lock detected"));
  }

  // LM_M(("Adding lock %p to lockVector %p" , new_lock , locksVector ));


  // Add the new lock
  locksVector->insert(new_lock);

  // Unlock
  pthread_mutex_unlock(&_lock);
}

void LockDebugger::remove_lock(void *new_lock) {
  // Lock private data
  int ans = pthread_mutex_lock(&_lock);         // Block until the mutex is free

  if (ans != 0)
    LM_X(1, ("pthread_mutex_lock error"));
  std::set<void *> *locksVector = _getLocksVector();

  // LM_M(("Removing lock %p from lockVector %p" , new_lock , locksVector ));

#ifdef FULL_DEBUG_AU_THREADS
  std::ostringstream o;
  o << "Removing thread \"" << getTitle() << "\" [LOCKS: " << locksVector->size() << "] to lock \"" << new_lock->description << "\"" <<
  std::endl;
  std::cout << o.str();
#endif
  // Make sure it was there
  if (locksVector->find(new_lock) == locksVector->end())
    LM_X(1, ("Error debugging locks. Removing a lock that was not previously defined. List of %d locks", locksVector->size()));
  locksVector->erase(new_lock);


  // Unlock
  pthread_mutex_unlock(&_lock);
}

std::set<void *> *LockDebugger::_getLocksVector() {
  pthread_t p  = pthread_self();

  std::set<void *> *locksVector;
  std::map< pthread_t, std::set<void *> * >::iterator i = locks.find(p);
  if (i == locks.end()) {
    locksVector =  new std::set<void *>();
    locks.insert(std::pair< pthread_t, std::set<void *> * >(p, locksVector));
  } else {
    locksVector = i->second;
  }


  return locksVector;
}

bool LockDebugger::_cross_blocking(void *new_lock) {
  std::set<void *> *myLocks = _getLocksVector();

  std::map< pthread_t, std::set<void *> * >::iterator i;
  for (i = locks.begin(); i != locks.end(); i++) {
    if (i->first != pthread_self())
      // Check if it have my new lock
      if (i->second->find(new_lock) != i->second->end()) {
        // It contains the lock I am traying to get
        // Let see if they have any of my previous locks
        std::set<void *>::iterator j;
        for (j = myLocks->begin(); j != myLocks->end(); j++) {
          if (i->second->find(*j) != i->second->end()) {
#ifdef FULL_DEBUG_AU_THREADS
            std::cout << "Cross lock detected. Me: " << myLocks->size() << " Other: " << i->second->size() << std::endl;
#endif
            return true;
          }
        }
      }
  }


  return false;
}

std::string LockDebugger::_getTitle() {
  void *data = pthread_getspecific(lockDebugger->key_title);

  if (data)
    return *((std::string *)data); else
    return "Unknown";
}

void LockDebugger::setThreadTitle(std::string title) {
  void *data = pthread_getspecific(LockDebugger::shared()->key_title);

  if (!data)
    LM_X(1, ("pthread_getspecific returned NULL during lock debugging"));
  pthread_setspecific(lockDebugger->key_title, new std::string(title));
}
}

