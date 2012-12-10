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
 * FILE                     AUTockenizer.cpp
 *
 * DESCRIPTION				Tockenizer to parse files describing modules
 *
 * ***************************************************************************/


#include <sstream>                  // std::ostringstream
#include <stdio.h>
#include <stdlib.h>                 // exit()

#include "logMsg/logMsg.h"          // LM_X

#include "AUTockenizer.h"           // Own interface

namespace samson {
AUTockenizer::AUTockenizer(std::string _fileName, std::string txt) {
  fileName = _fileName;

  reference_pos = 0;
  // First tokenize everything
  std::vector<AUToken> previous_items = tockenize(txt);

  // Remove spaces and returns.
  // Spaces between " " are not removed
  // items = removeSpacesAndReturns( removeComments(previous_items) );
  items = removeSpacesAndCommentsAndReturns(previous_items);
}

std::vector<AUToken> AUTockenizer::removeSpacesAndReturns(std::vector<AUToken> items) {
  std::vector<AUToken> new_items;

  bool literal = false;

  std::vector<AUToken>::iterator iter;

  for (iter = items.begin(); iter < items.end(); ++iter) {
    if (literal) {
      if ((*iter).str == "\"") {
        literal = false;
      }
      new_items.push_back(*iter);
    } else {
      if ((*iter).str == "\"") {
        literal = true;
        new_items.push_back(*iter);
      } else {
        // Outside literals, we do not have "returns" "spaces" "tabs" "\r"
        if (!isOneOf((*iter).str, " \t\r\n")) {
          new_items.push_back(*iter);
        }
      }
    }
  }

  return new_items;
}

/**
 * Function to remove comments from tockens
 * Basically removes every tocken between # and end_of_line
 */


std::vector<AUToken> AUTockenizer::removeComments(std::vector<AUToken> items) {
  std::vector<AUToken> new_items;

  bool removing = false;

  std::vector<AUToken>::iterator iter;

  for (iter = items.begin(); iter < items.end(); ++iter) {
    if (removing) {
      if ((*iter).str == "\n") {
        removing = false;
        new_items.push_back(*iter);                                     // Keep the return
      }
    } else {
      if ((*iter).str == "#") {
        removing = true;
      } else {
        new_items.push_back(*iter);
      }
    }
  }

  return new_items;
}

std::vector<AUToken> AUTockenizer::removeSpacesAndCommentsAndReturns(std::vector<AUToken> items) {
  std::vector<AUToken> new_items;

  bool literal = false;
  bool removing = false;

  std::vector<AUToken>::iterator iter;

  for (iter = items.begin(); iter < items.end(); ++iter) {
    if (literal) {
      if ((*iter).str == "\"") {
        literal = false;
      }
      if ((*iter).str == "\n") {
        fprintf(stdout, "samsonModuleParser: Warning, line break inside literal at line:%d\n", (*iter).line);
      }
      new_items.push_back(*iter);
    } else if (removing) {
      if ((*iter).str == "\n") {
        removing = false;
      }
    } else {
      if ((*iter).str == "\"") {
        literal = true;
        new_items.push_back(*iter);
      } else if ((*iter).str == "#") {
        removing = true;
      } else {
        // Outside literals, we do not have "returns" "spaces" "tabs" "\r"
        if (!isOneOf((*iter).str, " \t\r\n")) {
          new_items.push_back(*iter);
        }
      }
    }
  }

  return new_items;
}

/**
 *
 * Functions to tockenize the input string
 *
 */

std::vector<AUToken> AUTockenizer::tockenize(std::string txt) {
  std::string tockens = " #\r\t\n{};\"";              // All possible delimiters
  int nline = 1;

  std::vector<AUToken> items;
  AUToken item;

  // Simple parser
  size_t pos = 0;
  for (size_t i = 0; i < txt.size(); i++) {
    if (isOneOf(txt[i], tockens)) {
      if (i > pos) {
        item.str = txt.substr(pos, i - pos);
        item.line = nline;
        items.push_back(item);
        // fprintf(stdout, "item:'%s', line:%d, pos:%lu\n", item.str.c_str(), item.line, i);
      }

      // Emit the literal with one letter if that is the case
      item.str = txt[i];
      item.line = nline;
      items.push_back(item);
      // fprintf(stdout, "item:'%s', line:%d, pos:%lu\n", item.str.c_str(), item.line, i);

      if (txt[i] == '\n') {
        nline++;
      }
      pos = i + 1;
    }
  }

  if (txt.size() > pos) {
    item.str = txt.substr(pos, txt.size() - pos);
    item.line = nline;
    items.push_back(item);
    // fprintf(stdout, "item:'%s', line:%d, pos:%lu\n", item.str.c_str(), item.line, i);
  }

  return items;
}

/**
 * Auxiliar functions
 */

bool AUTockenizer::isOneOf(char c, std::string s) {
  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == c) {
      return true;
    }
  }

  return false;
}

bool AUTockenizer::isOneOf(std::string c_string, std::string s) {
  if (c_string.size() > 1) {
    return false;
  }

  char c = c_string[0];


  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == c) {
      return true;
    }
  }

  return false;
}

/**
 * Auxiliar function to get the "i-th" input item
 */

AUToken AUTockenizer::itemAtPos(unsigned int pos) {
  if (pos >= items.size()) {
    fprintf(
      stderr,
      "samsonModuleParser: Error accessing a wrong position(%d) inside the AUTockenizer element (items.size():%d) (coming from reference:'%s', line:%d)\n",
      pos, int(items.size()), items[reference_pos].str.c_str(), items[reference_pos].line);

    fprintf(stderr, "Error at AUTockenizer\n");
    exit(-1);
  }

  // fprintf(stdout, "returning items[%d].str:'%s'\n", pos, items[pos].str.c_str());
  return items[pos];
}

bool AUTockenizer::isSpecial(int pos) {
  std::string tmp = itemAtPos(pos).str;

  return isOneOf(tmp, "{};");
}

bool AUTockenizer::isOpenSet(int pos) {
  return(itemAtPos(pos).str == "{");
}

bool AUTockenizer::isCloseSet(int pos) {
  return(itemAtPos(pos).str == "}");
}

bool AUTockenizer::isSemiColom(int pos) {
  return(itemAtPos(pos).str == ";");
}

bool AUTockenizer::isOpenCloseLiteral(int pos) {
  return(itemAtPos(pos).str == "\"");
}

int AUTockenizer::searchSetFinishStartingAt(int pos) {
  int number_intern_sets = 1;

  if (!isOpenSet(pos)) {
    fprintf(stderr, "samsonModuleParser: Error in format while parsing the document at line:%d\n", itemAtPos(pos).line);
    fprintf(stderr, "Error at AUTockenizer\n");
    exit(-1);
  }
  reference_pos = pos;
  pos++;
  while (number_intern_sets > 0) {
    if (isCloseSet(pos)) {
      number_intern_sets--;
    } else if (isOpenSet(pos)) {
      number_intern_sets++;
    }
    pos++;
  }

  return (pos - 1);
}

int AUTockenizer::searchCloseLiteral(int pos) {
  reference_pos = pos;

  pos++;
  while (!isOpenCloseLiteral(pos)) {
    pos++;
  }

  return pos;
}

std::string AUTockenizer::getLiteralInternal(int pos, int pos2) {
  std::ostringstream o;

  for (int i = (pos + 1); i < pos2; i++) {
    o << itemAtPos(i).str;
  }
  return o.str();
}

std::string AUTockenizer::getLiteralInternalwithBlanks(int pos, int pos2) {
  std::ostringstream o;
  bool sepFound = true;

  int prevLine = 0;

  for (int i = pos; i <= pos2; i++) {
    if ((i != pos) && (itemAtPos(i).line != prevLine)) {
      if (sepFound == false) {
        fprintf(
          stderr,
          "samsonModuleParser: Error, expected ';' in the lines of a block. Reference line:%d, between item:'%s', and item:'%s'\n",
          items[i - 1].line, itemAtPos(
            i - 1).str.c_str(), itemAtPos(i).str.c_str());
        fprintf(stderr, "Error at AUTockenizer\n");
        exit(-1);
      }
    }
    if (itemAtPos(i).str == ";") {
      o << "\n";
      sepFound = true;
      // fprintf(stdout, "Find sepFound with '%s'(pos:%d)\n", itemAtPos(i).str.c_str(), i);
    } else {
      o << itemAtPos(i).str << " ";
      if ((itemAtPos(i).str != " ") && (itemAtPos(i).str != "\t") && (itemAtPos(i).str != "\n")) {
        sepFound = false;  // fprintf(stdout, "Deleting sepFound with '%s'(pos:%d)\n", itemAtPos(i).str.c_str(), i);
      }
    }

    prevLine = itemAtPos(i).line;
  }

  return o.str();
}

std::string AUTockenizer::getLiteral(int *pos) {
  reference_pos = *pos;

  if (isOpenCloseLiteral(*pos)) {
    int pos_start = *pos;
    int pos_finish = searchCloseLiteral(*pos);
    *pos = pos_finish + 1;
    return getLiteralInternal(pos_start, pos_finish);
  } else {
    int pos_item = *pos;
    *pos = *pos + 1;
    return itemAtPos(pos_item).str;
  }
}

std::string AUTockenizer::getBlock(int *pos) {
  int begin;
  int end;

  reference_pos = *pos;

  getScopeLimits(pos, &begin, &end);

  return getLiteralInternal(begin, end);
}

std::string AUTockenizer::getBlockwithBlanks(int *pos) {
  int begin;
  int end;

  reference_pos = *pos;

  getScopeLimits(pos, &begin, &end);

  return getLiteralInternalwithBlanks(begin, end);
}

void AUTockenizer::getScopeLimits(int *pos, int *begin, int *end) {
  reference_pos = *pos;
  if (!isOpenSet(*pos)) {
    fprintf(
      stderr,
      "samsonModuleParser: Error getting the limits in scope of a {  } while parsing the document, from reference:'%s' line:%d\n",
      items[reference_pos].str.c_str(), items[reference_pos].line);
    fprintf(stderr, "\t\tgetScopeLimits called from non OpenSet position:'%s'\n", itemAtPos(*pos).str.c_str());
    fprintf(stderr, "Error at AUTockenizer\n");
    exit(-1);
  }

  *begin = *pos + 1;

  int tmp = searchSetFinishStartingAt(*pos);

  *end = tmp - 1;
  // *end = tmp;
  *pos = tmp + 1;
}
}
