
#include "Tokenizer.h"  // Own interface
#include <algorithm>

namespace au {
namespace token {
Token::Token(std::string _content, Type _type,  int _position) {
  content = _content;
  position = _position;
  type  = _type;
}

bool Token::is(std::string _content) {
  return content == _content;
}

bool Token::isLiteral() {
  return (type == literal);
}

bool Token::isSeparator() {
  return (type == separator);
}

bool Token::isNormal() {
  return (type == normal);
}

bool Token::isOperation() {
  if (content == "+")
    return true; if (content == "-")
    return true; if (content == "*")
    return true; if (content == "/")
    return true; return false;
}

bool Token::isComparator() {
  if (content == "==")
    return true; if (content == "!=")
    return true; if (content == "<=")
    return true; if (content == ">=")
    return true; if (content == "<")
    return true; if (content == ">")
    return true;

  return false;
}

bool Token::isNumber() {
  const char *data = content.c_str();

  bool found_sign  = false;
  bool found_dot   = false;
  bool found_digit = false;

  size_t pos = 0;

  while (true) {
    int c = data[pos];
    pos++;

    if (c == '\0')
      return true;

    // Dot
    if (c == '.') {
      found_dot = true;
      continue;
    }

    // Signs
    if (( c == '-' ) || ( c == '+' )) {
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

std::string Token::str() {
  switch (type) {
    case normal:
      return au::str("%s", content.c_str(), position);

    case literal:
      return au::str("\"%s\"", content.c_str(), position);

    case separator:
      return au::str("<%s>", content.c_str(), position);
  }

  LM_X(1, ("Internal error"));
  return "Unknown";
}

#pragma mark TokenVector

TokenVector::TokenVector() {
  // Init position
  position = 0;
}

bool compare_string_by_length(const std::string& s1, const std::string& s2) {
  return s1.length() > s2.length();
}

std::string TokenVector::getNextTokenContent() {
  Token *token = getNextToken();

  if (!token)
    return "<empty>"; else
    return token->content;
}

Token *TokenVector::getNextToken() {
  if (position >= size())
    return NULL; else
    return &(*this)[position];
}

Token *TokenVector::popToken() {
  if (position >= size())
    return NULL;

  Token *t = &(*this)[position];
  position++;
  return t;
}

bool TokenVector::popNextTokensIfTheyAre(std::string content, std::string content2) {
  if ((size() == 0) || (size() == 1))
    return false;

  if (position >= ( size() - 1 ))
    return false;

  if ((*this)[position].content != content)
    return false;

  if ((*this)[position + 1].content != content2)
    return false;

  // Extract both tokens
  getNextToken();
  getNextToken();

  return true;
}

bool TokenVector::popNextTokenIfItIs(std::string content) {
  Token *t = getNextToken();

  if (!t)
    return false;

  if (t->is(content)) {
    // pop component
    popToken();
    return true;
  }

  return false;
}

bool TokenVector::checkNextTokenIs(std::string content) {
  Token *t = getNextToken();

  if (!t)
    return false;

  if (t->is(content))
    return true; return false;
}

bool TokenVector::checkNextNextTokenIs(std::string content) {
  if ((size() == 0) || (size() == 1))
    return false;

  if (position >= ( size() - 1 ))
    return false;

  return ((*this)[position + 1].content == content );
}

std::string TokenVector::str() {
  std::ostringstream output;

  output << "( " << size() << " tokens ) ";
  for (size_t i = 0; i < size(); i++) {
    output << (*this)[i].str() << " ";
  }
  return output.str();
}

void TokenVector::set_error(au::ErrorManager *error, std::string error_message) {
  if (position < size())
    error->set(au::str("%s at postition %d", error_message.c_str(), (*this)[position].position)); else
    error->set(au::str("%s at the end of the command", error_message.c_str()));
}

bool TokenVector::eof() {
  return ( position >= size());
}

// Add spetial tokens
void Tokenizer::addSingleCharTokens(std::string char_tokens) {
  for (size_t i = 0; i < char_tokens.length(); i++) {
    std::string token = char_tokens.substr(i, 1);
    tokens.push_back(token);
  }
}

// General function to add spetial tokens
void Tokenizer::addToken(std::string token) {
  tokens.push_back(token);
}

// ------------------------------------------------
// Tokenizer
// ------------------------------------------------

TokenVector Tokenizer::parse(std::string txt) {
  TokenVector token_vector;

  // Sort tokens so longer tokens come first
  std::sort(tokens.begin(), tokens.end(), compare_string_by_length);

  size_t pos = 0;           // Begin of the considered "token"
  size_t i = 0;

  size_t len = txt.length();

  while (i < len) {
    // ------------------------------------------------------
    // Spetial literals "XXX"
    // ------------------------------------------------------
    if (txt[i] == '"') {
      // Previous token finish ( if any )
      if (pos < i)
        token_vector.push_back(Token(txt.substr(pos, i - pos), Token::normal,  pos)); size_t pos_literal_begin = i + 1;
      size_t pos_literal_end = i + 1;
      while ((pos_literal_end < txt.length()) && (txt[pos_literal_end] != '"' )) {
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
      if (pos < i)
        token_vector.push_back(Token(txt.substr(pos, i - pos), Token::normal, pos)); size_t pos_literal_begin = i + 1;
      size_t pos_literal_end = i + 1;
      while ((pos_literal_end < txt.length()) && (txt[pos_literal_end] != '\'' )) {
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
    for (size_t t = 0; t < tokens.size(); t++) {
      std::string token = tokens[t];
      if (i + token.length() <= txt.length())
        if (txt.substr(i, token.length()) == token) {
          separator_found = true;
          separator = token;
          break;
        }
    }
    // ------------------------------------------------------


    if (separator_found) {
      // Push previous token if any
      if (pos < i) {
        token_vector.push_back(Token(txt.substr(pos, i - pos), Token::normal, pos));  // Push separator
      }
      if (separator != " ")
        token_vector.push_back(Token(separator, Token::separator, i)); i += separator.length();
      pos = i;
    } else {
      i++;
    }
  }

  // Last element
  if (len > pos)
    token_vector.push_back(Token(txt.substr(pos), Token::normal, pos)); return token_vector;
}
}
}
