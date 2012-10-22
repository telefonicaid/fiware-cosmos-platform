#ifndef _H_AU_SINGLETON
#define _H_AU_SINGLETON

#include "au/singleton/SingletonManager.h"
#include <cstring>
#include <vector>
/*
 * Andreu:  This class is used in SAMSON project and cannot contain mutexs since it is used across a fork operation
 */

namespace au {
template<class C>
class Singleton : public SingletonBase {
  Singleton() {
    instance_ = new C();
  };

public:

  virtual ~Singleton() {
    if (instance_) {
      delete instance_;
      instance_ = NULL;
    }
  };

  static C *shared() {
    if (!singleton_) {
      singleton_ = new Singleton<C>;
      singleton_manager.Add(singleton_);    // Add an instance of this Singleton to be able to remove it at the end
    }
    return singleton_->instance_;
  }

  static void DestroySingleton() {
    if (singleton_) {
      singleton_manager.Remove(singleton_);
      delete singleton_;
      singleton_ = NULL;
    }
  }

private:

  virtual void DestroySingletonInternal() {  // Method only called from SingletonManager
    delete singleton_;
    singleton_ = NULL;
  }

  friend class SingletonManager;  // Necessary to call Destroy

  static Singleton<C> *singleton_;
  C *instance_;
};

// Static members of the singleton to hold the instance
template<class C> Singleton<C> *Singleton<C>::singleton_ = NULL;
}  // end of au namesapce

#endif  // ifndef _H_AU_SINGLETON
