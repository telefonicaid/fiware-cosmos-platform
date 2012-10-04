#include <vector>

#ifndef _H_AU_SINGLETON
#define _H_AU_SINGLETON


namespace au {
  
  /*
   Andreu:  This class is used in SAMSON project and cannot contain mutexs since it is used across a fork operation
   */
  
class SingletonBase {
public:
  // Destroy the shared object
  virtual void Destroy() = 0;
};

class SingletonManager {
public:

  SingletonManager() {
  }

  ~SingletonManager() {
    // Remove all singleton instances
    DestroySingletons();
  }

  void DestroySingletons() {
    for (size_t i = 0; i < singletons_.size(); i++) {
      singletons_[i]->Destroy();
    }
    singletons_.clear();
  }

  size_t size() {
    return singletons_.size();
  }

private:

  void Add(SingletonBase *singleton) {
    singletons_.push_back(singleton);
  }

  // Vector of singletons
  std::vector<SingletonBase *> singletons_;

  template< class C >
  friend class Singleton;
};

// Unique instance of the manager
extern SingletonManager singleton_manager;

template< class C >
class Singleton : public SingletonBase {
public:

  static C *shared() {

    if (!instance_) {
      instance_ = new C();
      // Add an instance of this Singleton to be able to remove it at the end
      singleton_manager.Add(new Singleton<C>());
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

// Static members
template <class C> C * Singleton<C>::instance_ = NULL;
}  // end of au namesapce

#endif  // ifndef _H_AU_SINGLETON
