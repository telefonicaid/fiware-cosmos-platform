#include <vector>

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#ifndef _H_AU_SINGLETON
#define _H_AU_SINGLETON


namespace au {
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
    au::TokenTaker tt(&token_);

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
    au::TokenTaker tt(&token_);

    if (!instance_) {
      delete instance_;
      instance_ = NULL;
    }
  }

private:

  static au::Token token_;
  static C *instance_;
};

// Static members
template <class C> C * Singleton<C>::instance_ = NULL;
template <class C> au::Token Singleton<C>::token_("singleton");
}  // end of au namesapce

#endif  // ifndef _H_AU_SINGLETON
