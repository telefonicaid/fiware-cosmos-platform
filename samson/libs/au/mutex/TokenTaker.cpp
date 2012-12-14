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


#include <sys/time.h>                   // gettimeofday()

#include "logMsg/logMsg.h"              // LOG_SM()

#include "LockDebugger.h"               // LockDebugger
#include "TokenTaker.h"                 // Own interface
#include "au/ExecesiveTimeAlarm.h"
#include "au/mutex/Token.h"             // au::Token
#include "au/statistics/Cronometer.h"   // au::Cronometer

namespace au {
TokenTaker::TokenTaker(Token *token, const std::string& name) : name_(name), token_(token) {
  au::ExecesiveTimeAlarm alarm("TokenTaker::TokenTaker");

  token->Retain();
}

TokenTaker::~TokenTaker() {
  token_->Release();
}

void TokenTaker::Stop() {
  token_->Stop();
}

void TokenTaker::WakeUp() {
  token_->WakeUp();
}

void TokenTaker::WakeUpAll() {
  token_->WakeUpAll();
}

MultipleTokenTaker::MultipleTokenTaker(const std::vector<Token *>& tokens) : tokens_(tokens) {
  for (size_t i = 0; i < tokens_.size(); i++) {
    tokens_[i]->Retain();
  }
}

MultipleTokenTaker::~MultipleTokenTaker() {
  for (size_t i = 0; i < tokens_.size(); i++) {
    tokens_[i]->Release();
  }
}
}
