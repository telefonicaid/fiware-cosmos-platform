
#include "LogFormatter.h"  // Own interface
#include "au/log/Log.h"

namespace au {
LogFormatter::LogFormatter(const std::string& definition) {
  au::token::Tokenizer tokenizer;

  // Keep a copy of the definition string
  definition_ = definition;

  // Att all reserved words
  size_t i = 0;
  while (log_reseved_words[i] != NULL) {
    tokenizer.addToken(log_reseved_words[i++]);
  }

  // Parse to get the tokens
  au::token::TokenVector token_vector = tokenizer.parse(definition);

  au::token::Token *token = token_vector.getNextToken();
  while (token) {
    fields_.push_back(token->content);
    token_vector.popToken();
    token = token_vector.getNextToken();
  }

  LM_VV(("LogFormatter %s %lu fields", definition.c_str(), fields_.size()));
}

std::string LogFormatter::get(au::SharedPointer<Log> log) const {
  if (definition_ == "all")
    return log->str();

  std::string output;
  for (size_t i = 0; i < fields_.size(); i++) {
    output.append(log->Get(fields_[i]));
  }
  return output;
}
}