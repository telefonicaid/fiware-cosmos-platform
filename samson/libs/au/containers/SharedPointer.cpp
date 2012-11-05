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

#include "au/mutex/TokenTaker.h"

#include "logMsg/logMsg.h"                          // LM_T
#include "logMsg/traceLevels.h"

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
    LM_T(LmtCleanup2, ("Calling delete for ~SharedPointer this:%p", this));
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
