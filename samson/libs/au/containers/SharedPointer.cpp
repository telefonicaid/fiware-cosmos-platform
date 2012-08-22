
#include "au/mutex/TokenTaker.h"

#include "Object.h"  // Own interface


namespace au {
void Object::Release() {
  bool run_self_destroy = false;

  {
    TokenTaker tt(&token_);
    retain_counter_--;
    if (retain_counter_ == 0) {
      run_self_destroy = true;
    }
  }

  if (run_self_destroy) {
    delete this;
  }
}

void Object::Retain() {
  TokenTaker tt(&token_);

  retain_counter_++;
}

int Object::retain_counter() {
  return retain_counter_;
}
}