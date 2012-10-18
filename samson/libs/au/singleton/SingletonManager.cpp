
#include "SingletonManager.h" // Own interface

#include "Singleton.h"

namespace au {

  // Unique instance of the singleton_manager
  SingletonManager singleton_manager;
  
  void SingletonManager::DestroySingletons() {
    for (size_t i = 0; i < singletons_.size(); i++) {
      singletons_[i]->Destroy();
    }
    singletons_.clear();
  }


}