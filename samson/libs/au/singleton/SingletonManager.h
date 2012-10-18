#ifndef _H_AU_SINGLETON_MANAGER
#define _H_AU_SINGLETON_MANAGER

#include <vector>
#include <cstring>

/*
 Andreu:  This class is used in SAMSON project and cannot contain mutexs since it is used across a fork operation
 */

namespace au {
  
  class SingletonBase;
  
  class SingletonManager {
  public:
    SingletonManager() {}
    
    virtual ~SingletonManager() {
      DestroySingletons();      // Remove all singleton instances
    }
    
    void DestroySingletons();
    
    size_t size() const {
      return singletons_.size();
    }
    
  private:
    
    void Add(SingletonBase *singleton) {
      singletons_.push_back(singleton);
    }
    
    // Vector of singletons
    std::vector<SingletonBase *> singletons_;
    
    // Friend class Singleton to add Singleton in this manager
    template<class C>
    friend class Singleton;
  };

  extern SingletonManager singleton_manager;
  
}

#endif