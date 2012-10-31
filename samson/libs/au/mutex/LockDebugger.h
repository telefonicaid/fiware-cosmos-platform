
/* ****************************************************************************
*
* FILE            LockDebugger.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*  Class used to debug the Locks and Tokens in au library.
*  It can detecs auto-locks and cross-lock conditions
*
* ****************************************************************************/

#ifndef SAMSON_LOCK_DEBUGGER_H
#define SAMSON_LOCK_DEBUGGER_H

#include <map>                   /* map                                      */
#include <pthread.h>             /* pthread_mutex_t                          */
#include <set>                   /* set                                      */
#include <string>                /* std::string                              */

#include "au/singleton/Singleton.h"

namespace au {
class Lock;

class LockDebugger {
public:

  // Methods to assign name to threads
  static void SetThreadTitle(const std::string&);
  static std::string GetThreadTitle();

  // Methods to notify when retain and release a mutex
  void AddMutexLock(void *new_lock);
  void RemoveMutexLock(void *new_lock);

private:

  LockDebugger();
  ~LockDebugger();

  // We use au::Singleton to implement a singleton over this class
  friend class au::Singleton<LockDebugger>;

  std::set<void *> *GetLocksVector();
  bool IsCrossBlocking(void *new_lock);

  pthread_mutex_t lock_;
  std::map< pthread_t, std::set< void * > * > locks_;
  pthread_key_t key_title_;      // Key to set or recover name of the thread
};
}

#endif  // ifndef SAMSON_LOCK_DEBUGGER_H
