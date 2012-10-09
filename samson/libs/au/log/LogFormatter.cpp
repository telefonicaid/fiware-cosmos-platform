#include "LogFormatter.h"  // Own interface
#include "au/log/Log.h"

namespace au {
LogFormatter::LogFormatter(const std::string& definition) {
  set_format(definition);
}

void LogFormatter::set_format(const std::string& definition) {

  // Keep a copy of the definition string
  definition_ = definition;

  fields_.clear();   // Remove previous definition ( if any )

  // Tokenize with reserved words
  au::token::Tokenizer tokenizer;
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

}

std::string LogFormatter::get(au::SharedPointer<Log> log) const {
  if (definition_ == "all") {
    return log->str();
  }

  std::string output;
  for (size_t i = 0; i < fields_.size(); i++) {
    output.append(log->Get(fields_[i]));
  }
  return output;
}
}
