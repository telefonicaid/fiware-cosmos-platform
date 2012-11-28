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

#include "Tokenizer.h"  // Own interface
#include <algorithm>

namespace au {
namespace token {
Token::Token(const std::string& content, Type type,  int position)
  : content_(content)
    , position_(position)
    , type_(type) {
}

bool Token::IsContent(const std::string& content) const {
  return content_ == content;
}

bool Token::IsLiteral() const {
  return (type_ == literal);
}

bool Token::IsSeparator() const {
  return (type_ == separator);
}

bool Token::IsNormal() const {
  return (type_ == normal);
}

bool Token::IsOperation() const {
  if (content_ == "+") {
    return true;
  }
  if (content_ == "-") {
    return true;
  }
  if (content_ == "*") {
    return true;
  }
  if (content_ == "/") {
    return true;
  }
  return false;
}

bool Token::IsComparator() const {
  if (content_ == "==") {
    return true;
  }
  if (content_ == "!=") {
    return true;
  }
  if (content_ == "<=") {
    return true;
  }
  if (content_ == ">=") {
    return true;
  }
  if (content_ == "<") {
    return true;
  }
  if (content_ == ">") {
    return true;
  }

  return false;
}

bool Token::IsNumber() const {
  const char *data = content_.c_str();

  bool found_sign  = false;
  bool found_dot   = false;
  bool found_digit = false;

  size_t pos = 0;

  while (true) {
    int c = data[pos];
    pos++;

    if (c == '\0') {
      return true;
    }

    // Dot
    if (c == '.') {
      found_dot = true;
      continue;
    }

    // Signs
    if ((c == '-') || (c == '+')) {
      if (found_dot) {
        return false;                 // sign after dot
      }
      if (found_sign) {
        return false;                 // Double sign
      }
      if (found_digit) {
        return false;                 // Sign after digits
      }
      found_sign = true;
      continue;
    }

    if (isdigit(c)) {
      found_digit = true;
      continue;
    }

    return false;
  }

  LM_X(1, ("Internal error"));
  return true;
}

std::string Token::str() const {
  switch (type_) {
    case normal:
      return au::str("%s", content_.c_str(), position_);

    case literal:
      return au::str("\"%s\"", content_.c_str(), position_);

    case separator:
      return au::str("<%s>", content_.c_str(), position_);
  }

  LM_X(1, ("Internal error"));
  return "Unknown";
}

#pragma mark TokenVector

TokenVector::TokenVector() : position_(0) {
}

bool compare_string_by_length(const std::string& s1, const std::string& s2) {
  return s1.length() > s2.length();
}

std::string TokenVector::GetNextTokenContent() {
  Token *token = GetNextToken();

  if (!token) {
    return "<empty>";
  } else {
    return token->content();
  }
}

Token *TokenVector::GetNextToken() {
  if (position_ >= size()) {
    return NULL;
  } else {
    return &(*this)[position_];
  }
}

Token *TokenVector::PopToken() {
  if (position_ >= size()) {
    return NULL;
  }

  Token *t = &(*this)[position_];
  position_++;
  return t;
}

bool TokenVector::PopNextTokenIfContentIs(const std::string& content) {
  Token *t = GetNextToken();

  if (!t) {
    return false;
  }
  if (t->IsContent(content)) {
    PopToken();
    return true;
  }
  return false;
}

bool TokenVector::PopNextTwoTokensContentsAre(const std::string& content, const std::string&content2) {
  if (!CheckNextTokenContentIs(content)) {
    return false;
  }
  if (!CheckNextNextTokenContentIs(content2)) {
    return false;
  }

  PopToken();
  PopToken();
  return true;
}

bool TokenVector::CheckNextTokenContentIs(const std::string& content) {
  Token *t = GetNextToken();

  if (!t) {
    return false;
  }

  if (t->IsContent(content)) {
    return true;
  }
  return false;
}

bool TokenVector::CheckNextNextTokenContentIs(const std::string& content) {
  if ((size() == 0) || (size() == 1)) {
    return false;
  }

  if (position_ >= (size() - 1)) {
    return false;
  }

  return ((*this)[position_ + 1].content() == content);
}

std::string TokenVector::str() const {
  std::ostringstream output;

  output << "( " << size() << " tokens ) ";
  for (size_t i = 0; i < size(); ++i) {
    output << (*this)[i].str() << " ";
  }
  return output.str();
}

void TokenVector::set_error(au::ErrorManager& error, const std::string& error_message) {
  if (position_ < size()) {
    error.set(au::str("%s at postition %d", error_message.c_str(), (*this)[position_].position()));
  } else {
    error.set(au::str("%s at the end of the command", error_message.c_str()));
  }
}

void TokenVector::set_error(au::ErrorManager *error, const std::string& error_message) {
  if (position_ < size()) {
    error->set(au::str("%s at postition %d", error_message.c_str(), (*this)[position_].position()));
  } else {
    error->set(au::str("%s at the end of the command", error_message.c_str()));
  }
}

bool TokenVector::eof() const {
  return (position_ >= size());
}

// Add spetial tokens
void Tokenizer::AddSingleCharTokens(const std::string& char_tokens) {
  for (size_t i = 0; i < char_tokens.length(); ++i) {
    std::string token = char_tokens.substr(i, 1);
    tokens_.push_back(token);
  }

  // Sort tokens so longer tokens come first
  std::sort(tokens_.begin(), tokens_.end(), compare_string_by_length);
}

// General function to add spetial tokens
void Tokenizer::AddToken(const std::string& token) {
  tokens_.push_back(token);

  // Sort tokens so longer tokens come first
  std::sort(tokens_.begin(), tokens_.end(), compare_string_by_length);
}

TokenVector TokenVector::GetTokensUntil(std::string limiter) {
  TokenVector result_tokens;

  while (true) {
    Token *token = PopToken();

    if (!token) {
      return result_tokens;
    }

    if (token->content() == limiter) {
      return result_tokens;
    }

    result_tokens.push_back(Token(token->content(), token->type(), token->position()));
  }
}

// ------------------------------------------------
// Tokenizer
// ------------------------------------------------

TokenVector Tokenizer::Parse(const std::string& txt) const {
  TokenVector token_vector;

  size_t pos = 0;
  size_t i = 0;

  size_t len = txt.length();

  while (i < len) {
    // ------------------------------------------------------
    // Spetial literals "XXX"
    // ------------------------------------------------------
    if (txt[i] == '"') {
      // Previous token finish ( if any )
      if (pos < i) {
        token_vector.push_back(Token(txt.substr(pos, i - pos), Token::normal,  pos));
      }
      size_t pos_literal_begin = i + 1;
      size_t pos_literal_end = i + 1;
      while ((pos_literal_end < txt.length()) && (txt[pos_literal_end] != '"')) {
        pos_literal_end++;
      }

      // Add the literal
      token_vector.push_back(Token(
                               txt.substr(pos_literal_begin, pos_literal_end - pos_literal_begin)
                               , Token::literal
                               , pos_literal_begin));

      i = pos_literal_end + 1;
      pos = pos_literal_end + 1;
      continue;
    }

    // ------------------------------------------------------
    // Spetial literals 'XXX'
    // ------------------------------------------------------
    if (txt[i] == '\'') {
      // Previous token finish ( if any )
      if (pos < i) {
        token_vector.push_back(Token(txt.substr(pos, i - pos), Token::normal, pos));
      }
      size_t pos_literal_begin = i + 1;
      size_t pos_literal_end = i + 1;
      while ((pos_literal_end < txt.length()) && (txt[pos_literal_end] != '\'')) {
        pos_literal_end++;
      }

      // Add the literal
      token_vector.push_back(Token(
                               txt.substr(pos_literal_begin, pos_literal_end - pos_literal_begin)
                               , Token::literal
                               , pos_literal_begin));

      i = pos_literal_end + 1;
      pos = pos_literal_end + 1;
      continue;
    }

    // ------------------------------------------------------
    // Find any of the provided "tokens"
    // ------------------------------------------------------

    std::string separator;
    bool separator_found = false;
    for (size_t t = 0; t < tokens_.size(); ++t) {
      std::string token = tokens_[t];
      if (i + token.length() <= txt.length()) {
        if (txt.substr(i, token.length()) == token) {
          separator_found = true;
          separator = token;
          break;
        }
      }
    }
    // ------------------------------------------------------


    if (separator_found) {
      // Push previous token if any
      if (pos < i) {
        token_vector.push_back(Token(txt.substr(pos, i - pos), Token::normal, pos));  // Push separator
      }
      if (separator != " ") {
        token_vector.push_back(Token(separator, Token::separator, i));
      }
      i += separator.length();
      pos = i;
    } else {
      i++;
    }
  }

  // Last element
  if (len > pos) {
    token_vector.push_back(Token(txt.substr(pos), Token::normal, pos));
  }
  return token_vector;
}
}
}
