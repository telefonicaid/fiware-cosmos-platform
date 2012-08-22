

#include <sys/time.h>                   // gettimeofday()

#include "logMsg/logMsg.h"              // LM_M()

#include "LockDebugger.h"               // LockDebugger
#include "TokenTaker.h"                 // Own interface
#include "au/Cronometer.h"              // au::Cronometer
#include "au/ExecesiveTimeAlarm.h"
#include "au/mutex/Token.h"             // au::Token

namespace au {
TokenTaker::TokenTaker(Token *token, const std::string& name)
  : name_(name)
    , token_(token) {
  au::ExecesiveTimeAlarm alarm("TokenTaker::TokenTaker");

  // LM_M(("New TokenTaker %s for token %s", name ,  token->name_));
  token->Retain();
}

TokenTaker::~TokenTaker() {
  // LM_M(("Destroy TokenTaker %s for token %s", name ,  token->name_));
  token_->Release();
}

void TokenTaker::Stop() {
  token_->Stop();
}

void TokenTaker::WakeUp() {
  // Wake up a thread that has been "stop"
  // LM_M(("Wake up for token %s", token->name_ ));

  if (pthread_cond_signal(&token_->block_) != 0) {
    LM_X(1, ("Internal error at au::TokenTaker"));
  }
}

void TokenTaker::WakeUpAll() {
  // Wake up all stopped threads
  // LM_M(("Wake up all for token %s", token->name_ ));

  if (pthread_cond_broadcast(&token_->block_) != 0) {
    LM_X(1, ("Internal error at au::TokenTaker"));
  }
}
}
