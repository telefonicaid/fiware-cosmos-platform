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
* FILE            TokenTaker
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*      Automatic management of a mutex
*      Classical trick of retaining a mutex in the contructor and releasing in destructor
*      Used in collaboration with au::Token class
*
* ****************************************************************************/

#ifndef _H_AU_TOCKEN_TAKER
#define _H_AU_TOCKEN_TAKER

#include <pthread.h>             /* pthread_mutex_t                          */
#include <string>
#include <vector>

namespace au {
class Token;

class TokenTaker {
public:

  TokenTaker(Token *_token, const std::string& name = "unknown");
  ~TokenTaker();

  void Stop();
  void WakeUp();
  void WakeUpAll();

private:
  std::string name_;
  Token *token_;
};

class MultipleTokenTaker {
public:

  MultipleTokenTaker(const std::vector<Token *>& tokens);
  ~MultipleTokenTaker();

private:
  std::vector<Token *> tokens_;
};
}

#endif  // ifndef _H_AU_TOCKEN_TAKER
