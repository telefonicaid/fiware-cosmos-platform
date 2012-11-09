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
#ifndef _H_AU_SINGLETON_MANAGER
#define _H_AU_SINGLETON_MANAGER

#include <cstring>
#include <set>

/*
 * Andreu:  This class is used in SAMSON project and cannot contain mutexs since it is used across a fork operation
 */

namespace au {
class SingletonBase {
public:
  // Destroy the shared object
  virtual void DestroySingletonInternal() = 0;
};


class SingletonManager {
public:
  SingletonManager() {
  }

  virtual ~SingletonManager() {
    DestroySingletons();        // Remove all singleton instances
  }

  void DestroySingletons() {
    std::set<SingletonBase *>::iterator iter;
    for (iter = singletons_.begin(); iter != singletons_.end(); iter++) {
      (*iter)->DestroySingletonInternal();
    }
    singletons_.clear();
  }

  size_t size() const {
    return singletons_.size();
  }

private:

  void Add(SingletonBase *singleton) {
    singletons_.insert(singleton);
  }

  void Remove(SingletonBase *singleton) {
    singletons_.erase(singleton);
  }

  // Vector of singletons
  std::set<SingletonBase *> singletons_;

  // Friend class Singleton to add and remove instances of singleton
  template<class C>
  friend class Singleton;
};

extern SingletonManager singleton_manager;
}

#endif // ifndef _H_AU_SINGLETON_MANAGER