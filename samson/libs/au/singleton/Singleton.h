#ifndef _H_AU_SINGLETON
#define _H_AU_SINGLETON

#include <vector>
#include <cstring>
#include "au/singleton/SingletonManager.h"
/*
 Andreu:  This class is used in SAMSON project and cannot contain mutexs since it is used across a fork operation
 */

namespace au {

class SingletonBase {
  public:
    // Destroy the shared object
    virtual void Destroy() = 0;
};


template<class C>
class Singleton : public SingletonBase {
  public:
    static C *shared() {
      if (!instance_) {
        instance_ = new C();
        // Add an instance of this Singleton to be able to remove it at the end
        singleton_manager.Add(new Singleton<C> ());
      }
      return instance_;
    }

    virtual void Destroy() {
      delete instance_;
      instance_ = NULL;
    }

    static void DestroySingleton() {
      if (!instance_) {
        delete instance_;
        instance_ = NULL;
      }
    }

  private:
    static C *instance_;
};

// Static members of the singleton to hold the instance
template<class C> C * Singleton<C>::instance_ = NULL;
  
} // end of au namesapce

#endif  // ifndef _H_AU_SINGLETON
