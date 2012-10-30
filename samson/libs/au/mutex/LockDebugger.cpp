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

namespace au {
class LockDebugger;
LockDebugger *lockDebugger;


LockDebugger::LockDebugger() {
  pthread_mutex_init(&lock_, 0);
  pthread_key_create(&key_title_, NULL);
}

LockDebugger::~LockDebugger() {
  pthread_mutex_destroy(&lock_);
  pthread_key_delete(key_title_);
}

void LockDebugger::AddMutexLock(void *new_lock) {
  // Lock private data

  // Block until the mutex is free
  int ans = pthread_mutex_lock(&lock_);

  // Make sure there are no errors with lock
  if (ans != 0) {
    LM_X(1, ("pthread_mutex_lock error"));  // Recover list of mutexs...
  }
  std::set<void *> *locksVector = GetLocksVector();

  // We do not autoblock
  if (locksVector->find(new_lock) !=  locksVector->end()) {
    LM_X(1, ("Autolock detected"));  // We are not blocked
  }
  if (IsCrossBlocking(new_lock)) {
    assert(false);
    LM_X(1, ("Cross lock detected"));
  }

  // LM_M(("Adding lock %p to lockVector %p" , new_lock , locksVector ));


  // Add the new lock
  locksVector->insert(new_lock);

  // Unlock
  pthread_mutex_unlock(&lock_);
}

void LockDebugger::RemoveMutexLock(void *new_lock) {
  // Lock private data
  int ans = pthread_mutex_lock(&lock_);         // Block until the mutex is free

  if (ans != 0) {
    LM_X(1, ("pthread_mutex_lock error"));  // Recover list of mutexs...
  }
  std::set<void *> *locksVector = GetLocksVector();

  // Make sure it was there
  if (locksVector->find(new_lock) == locksVector->end()) {
    LM_X(1,
         ("Error debugging locks. Removing a lock that was not previously defined. List of %d locks", locksVector->size()));
  }
  locksVector->erase(new_lock);


  // Unlock
  pthread_mutex_unlock(&lock_);
}

std::set<void *> *LockDebugger::GetLocksVector() {
  pthread_t p  = pthread_self();

  std::set<void *> *locksVector;
  std::map< pthread_t, std::set<void *> * >::iterator i = locks_.find(p);
  if (i == locks_.end()) {
    locksVector =  new std::set<void *>();
    locks_.insert(std::pair< pthread_t, std::set<void *> * >(p, locksVector));
  } else {
    locksVector = i->second;
  }


  return locksVector;
}

bool LockDebugger::IsCrossBlocking(void *new_lock) {
  std::set<void *> *myLocks = GetLocksVector();

  std::map< pthread_t, std::set<void *> * >::iterator i;
  for (i = locks_.begin(); i != locks_.end(); i++) {
    if (i->first != pthread_self()) {
      // Check if it have my new lock
      if (i->second->find(new_lock) != i->second->end()) {
        // It contains the lock I am traying to get
        // Let see if they have any of my previous locks
        std::set<void *>::iterator j;
        for (j = myLocks->begin(); j != myLocks->end(); j++) {
          if (i->second->find(*j) != i->second->end()) {
            return true;
          }
        }
      }
    }
  }


  return false;
}

std::string LockDebugger::GetThreadTitle() {
  void *data = pthread_getspecific(lockDebugger->key_title_);

  if (data) {
    return *((std::string *)data);
  } else {
    return "Unknown";
  }
}

void LockDebugger::SetThreadTitle(const std::string& title) {
  void *data = pthread_getspecific(au::Singleton<LockDebugger>::shared()->key_title_);

  if (!data) {
    LM_X(1,
         ("pthread_getspecific returned NULL during lock debugging"));  // Set specific data
  }
  pthread_setspecific(lockDebugger->key_title_, new std::string(title));
}
}

