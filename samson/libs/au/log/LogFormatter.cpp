
#include "au/log/LogFormatter.h"  // Own interface

#include "au/log/Log.h"

namespace au {
LogFormatter::LogFormatter(const std::string& definition , bool color ) {
  SetFormat(definition,color);
}

void LogFormatter::SetFormat(const std::string& definition , bool color ) {

  definition_ = definition;  // Keep a copy of the definition string
  color_ = color;

  fields_.clear();   // Remove previous definition ( if any )

  if( definition == "none" )
    return; // no format
  
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
    if( color_ )
      return au::str( log->GetColor() ,  "%s" , GetIntern(log).c_str() );
    else
      return GetIntern(log);
  }
  
std::string LogFormatter::GetIntern(au::SharedPointer<Log> log) const {

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