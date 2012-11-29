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

#include "au/log/LogFormatter.h"  // Own interface

#include "au/log/Log.h"

namespace au {
LogFormatter::LogFormatter(const std::string& definition, bool color) {
  SetFormat(definition, color);
}

void LogFormatter::SetFormat(const std::string& definition, bool color) {
  definition_ = definition;  // Keep a copy of the definition string
  color_ = color;

  fields_.clear();   // Remove previous definition ( if any )

  if (definition == "none") {
    return;  // no format
  }
  // Tokenize with reserved words
  au::token::Tokenizer tokenizer;
  size_t i = 0;
  while (log_reseved_words[i] != NULL) {
    tokenizer.AddToken(log_reseved_words[i++]);
  }

  // Parse to get the tokens
  au::token::TokenVector token_vector = tokenizer.Parse(definition);

  au::token::Token *token = token_vector.GetNextToken();
  while (token) {
    fields_.push_back(token->content());
    token_vector.PopToken();
    token = token_vector.GetNextToken();
  }
}

std::string LogFormatter::get(au::SharedPointer<Log> log) const {
  if (color_) {
    return au::str(log->GetColor(), "%s", GetIntern(log).c_str());
  } else {
    return GetIntern(log);
  }
}

std::string LogFormatter::GetIntern(au::SharedPointer<Log> log) const {
  if (definition_ == "all") {
    return log->str();
  }
  std::ostringstream output;
  for (size_t i = 0; i < fields_.size(); i++) {
    output << log->Get(fields_[i]);
  }
  return output.str();
}
}