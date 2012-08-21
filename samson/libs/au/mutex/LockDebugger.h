
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
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef SAMSON_LOCK_DEBUGGER_H
#define SAMSON_LOCK_DEBUGGER_H

#include <map>                   /* map                                      */
#include <pthread.h>             /* pthread_mutex_t                          */
#include <set>                   /* set                                      */
#include <string>                /* std::string                              */



namespace au {
class Lock;

class LockDebugger {
  LockDebugger();
  ~LockDebugger();

public:

  // Singleton implementation
  static LockDebugger *shared();
  static void destroy();

  // Methods to assign name to threads
  static void SetThreadTitle(const std::string&);
  static std::string GetThreadTitle();

  // Methods to notify when retain and release a mutex
  void AddMutexLock(void *new_lock);
  void RemoveMutexLock(void *new_lock);

private:

  std::set<void *> *GetLocksVector();
  bool IsCrossBlocking(void *new_lock);

  pthread_mutex_t lock_;
  std::map< pthread_t, std::set< void * > * > locks_;
  pthread_key_t key_title_;      // Key to set or recover name of the thread
};
}

#endif  // ifndef SAMSON_LOCK_DEBUGGER_H
