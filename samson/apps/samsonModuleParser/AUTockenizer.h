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
#ifndef AU_TOKENIZER_H
#define AU_TOKENIZER_H


/* ****************************************************************************
 *
 * FILE                     AUTockenizer.h
 *
 * DESCRIPTION			   Tockenizer to parse files describing modules
 *
 */

#include <string>               // std::string
#include <vector>               // std::vector

#include "au/string/StringUtilities.h"

namespace samson {
class AUToken {
public:

  std::string str;                    // Content of the file
  int line;                           // Line in the original file

  AUToken() {
  }

  AUToken(std::string txt, int nline) {
    str = txt; line = nline;
  }
};

/**
 * General tokenizer element
 */

class AUTockenizer {
public:

  std::vector<AUToken> items;               // Parsed elements from the text
  int reference_pos;

  std::string fileName;                     // Used in error messages

  AUTockenizer(std::string _fileName, std::string txt);

  static std::vector<AUToken> removeSpacesAndReturns(std::vector<AUToken> items);
  static std::vector<AUToken> removeSpacesAndCommentsAndReturns(std::vector<AUToken> items);

  /**
   * Function to remove comments from tockens
   * Bascially removes every tocken between # and end_of_line
   */

  static std::vector<AUToken> removeComments(std::vector<AUToken> items);

  static std::vector<AUToken> tockenize(std::string txt);

  static bool isOneOf(char c, std::string s);

  static bool isOneOf(std::string c_string, std::string s);

  AUToken itemAtPos(unsigned int pos);

  bool isSpecial(int pos);

  bool isOpenSet(int pos);

  bool isCloseSet(int pos);

  bool isSemiColom(int pos);

  bool isOpenCloseLiteral(int pos);

  int searchSetFinishStartingAt(int pos);

  int searchCloseLiteral(int pos);

  std::string getLiteralInternal(int pos, int pos2);

  std::string getLiteralInternalwithBlanks(int pos, int pos2);

  std::string getLiteral(int *pos);

  std::string getBlock(int *pos);

  std::string getBlockwithBlanks(int *pos);

  void getScopeLimits(int *pos, int *begin, int *end);


  // Error message formated correctly

  std::string error_message(std::string message, int pos) {
    if (( pos == -1 ) || ( pos < 0 ) || ( pos >= (int)items.size())) {
      return au::str("%s: %s", fileName.c_str(), message.c_str());
    } else {
      return au::str("%s[%d]: %s ( error at %s )"
                     , fileName.c_str()
                     , items[pos].line
                     , message.c_str()
                     , items[pos].str.c_str()
                     );
    }
  }
};
}

#endif  // ifndef AU_TOKENIZER_H
