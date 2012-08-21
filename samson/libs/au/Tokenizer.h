
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
* COPYRIGTH       Copyright 2012 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/


#ifndef _H_AU_TOKENIZER
#define _H_AU_TOKENIZER

#include <string>

#include "logMsg/logMsg.h"

#include "au/containers/vector.h"
#include "au/string.h"

namespace au {
namespace token {
class Token {
public:

  typedef enum {
    normal,
    separator,
    literal
  } Type;


  std::string content;                // Content of the token
  int position;                       // Position in the string
  Type type;

  // Constructor
  Token(std::string _content, Type _type, int _position);

  // Check if this component is a particular sequence
  bool is(std::string _content);

  // Check if this token is a numerical value
  bool isNumber();

  // Check type
  bool isNormal();
  bool isSeparator();
  bool isLiteral();

  // Debug str
  std::string str();

  // Other interesting informations
  bool isOperation();
  bool isComparator();
};


class TokenVector : public std::vector<Token>{
  size_t position;                            // Position inside the vector ( when retrieving for using... )


public:

  TokenVector();

  // Functions to deserialize the provided command
  Token *getNextToken();
  Token *popToken();

  // Simple function to recover text ( to show error )
  std::string getNextTokenContent();

  bool popNextTokenIfItIs(std::string content);
  bool popNextTokensIfTheyAre(std::string content, std::string content2);
  bool checkNextTokenIs(std::string content);
  bool checkNextNextTokenIs(std::string content);

  // Check end of the provided command
  bool eof();

  // Debug function
  std::string str();

  // Auxiliary function to set the error.
  void set_error(au::ErrorManager *error, std::string error_message);


  // Extract a TokenVector until a particular token ( removing this )
  TokenVector getTokensUntil(std::string limiter) {
    TokenVector result_tokens;

    while (true) {
      Token *token = popToken();

      if (!token)
        return result_tokens;




      if (token->content == limiter)
        return result_tokens;




      result_tokens.push_back(Token(token->content, token->type, token->position));
    }
  }
};


class Tokenizer {
  std::vector<std::string> tokens;            // Vector with the considered tokens

public:

  // Add special tokens
  void addSingleCharTokens(std::string tokens);

  // General function to add special tokens
  void addToken(std::string token);

  // Main function to parse the provided command
  TokenVector parse(std::string command);
};
}
}

#endif  // ifndef _H_AU_TOKENIZER
