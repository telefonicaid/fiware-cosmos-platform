
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#ifndef _H_AU_SINGLETON
#define _H_AU_SINGLETON


namespace au {
template< class C >
class Singleton {
public:

  static C *shared() {
    au::TokenTaker tt(&token_);

    if (!instance_) {
      instance_ = new C();
    }
    return instance_;
  }

private:

  static au::Token token_;
  static C *instance_;
};

// Static members
template <class C> C * Singleton<C>::instance_;
template <class C> au::Token Singleton<C>::token_("singleton");
}  // end of au namesapce

#endif // ifndef _H_AU_SINGLETON
