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

/* ****************************************************************************
*
* FILE            Tokenizer.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            2012
*
* DESCRIPTION
*
* Classes used to tokenize a string with a particular command
*
* ****************************************************************************/


#ifndef _H_AU_TOKENIZER
#define _H_AU_TOKENIZER

#include <string>

#include "logMsg/logMsg.h"

#include "au/containers/vector.h"
#include "au/string/StringUtilities.h"

namespace au {
/**
 * \brief Name space for au::token library ( Generic tokenizer for simple language interpretation )
 */
namespace token {
class Token {
public:

  typedef enum {
    normal,
    separator,
    literal
  } Type;

  // Constructor
  Token(const std::string& content, Type type, int position);

  // Check if this component is a particular sequence
  bool ContentMatch(const std::string& _content) const;

  // Check if this token is a numerical value
  bool IsNumber() const;

  // Check type
  bool IsNormal() const;
  bool IsSeparator() const;
  bool IsLiteral() const;

  // Handy function to check token
  bool IsOperation() const;
  bool IsComparator() const;

  // Debug str
  std::string str() const;

  // Accessors
  std::string content() const {
    return content_;
  }

  int position() const {
    return position_;
  }

  Type type() const {
    return type_;
  }

private:

  std::string content_;  // Content of the token
  int position_;        // Position in the string
  Type type_;           // Type of token
};


class TokenVector {
public:

  TokenVector();

  /**
   * \brief Get next token ( read-position inside vector is not advanced )
   */
  Token *GetNextToken();

  /**
   * \brief Get the next token ( read-position inside vector is advanced )
   */

  Token *PopToken();

  /**
   * \brief ( read-position inside vector is not advanced )
   */

  std::string GetNextTokenContent();


  bool PopNextTokenIfContentIs(const std::string& content);
  bool PopNextTwoTokensIfContentsAre(const std::string& content, const std::string&content2);

  bool CheckNextTokenContentIs(const std::string& content);
  bool CheckNextNextTokenContentIs(const std::string& content);

  // Check end of the provided command
  bool eof() const;

  // Debug function
  std::string str() const;

  // Auxiliary function to set the error with additional information about the token position
  void set_error(au::ErrorManager *error, const std::string& error_message);
  void set_error(au::ErrorManager &error, const std::string& error_message);

  // Extract a TokenVector until a particular token ( removing this as well )
  TokenVector GetTokensUntil(std::string limiter);

private:

  friend class Tokenizer;

  size_t position_;              // Position inside the vector
  std::vector<Token> tokens_;    // Vector of tokens
};


class Tokenizer {
public:

  Tokenizer() {
  };
  explicit Tokenizer(const std::string& tokens) {
    AddSingleCharTokens(tokens);
  };

  // Add special tokens
  void AddSingleCharTokens(const std::string& tokens);

  // General function to add special tokens
  void AddToken(const std::string& token);

  // Main function to parse the provided command into a vector of tokens
  TokenVector Parse(const std::string& command) const;

private:

  // Vector of tokens to be used in calls to Parse()
  std::vector<std::string> tokens_;
};
}
}

#endif  // ifndef _H_AU_TOKENIZER
