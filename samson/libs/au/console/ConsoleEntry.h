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

#ifndef _AU_CONSOLE_ENTRY
#define _AU_CONSOLE_ENTRY

#include <list>
#include <string>
#include <termios.h>                // termios

#include "au/console/ConsoleEscapeSequence.h"
#include "au/mutex/Token.h"

namespace au {
namespace console {
/**
 * \brief Class used to get something from the entry from the user
 */

class ConsoleEntry {
  typedef enum {
    normal_char,
    escape_sequence,
    unknown_escape_sequence
  } Type;

  Type type;
  char c;                    // Entry character ( except if 0 )
  std::string seq;           // Escape sequence

public:


  void setChar(char _c) {
    type = normal_char;
    c = _c;
  }

  void setEscapeSequence(std::string _seq) {
    type = escape_sequence;
    seq = _seq;
  }

  void setUnknownEscapeSequence(std::string _seq) {
    type = unknown_escape_sequence;
    seq = _seq;
  }

  bool isChar() {
    return ( type == normal_char);
  }

  bool isChar(char _c) {
    if (type == normal_char) {
      return (c == _c );
    } else {
      return false;
    }
  }

  char getChar() {
    if (type != normal_char) {
      LM_X(1, ("Getting a char from a non char Console Entry"));
    }
    return c;
  }

  bool isEscapeSequence() {
    return ( type == escape_sequence);
  }

  bool isUnknownEscapeSequence() {
    return ( type == unknown_escape_sequence);
  }

  std::string getEscapeSequece() {
    if (type == normal_char) {
      LM_X(1, ("Getting a escape sequence from a wrong Console Entry"));
    }

    return seq;
  }

  bool isCursorUp() {
    if (type != escape_sequence) {
      return false;
    }




    return ( seq == "[A" );
  }

  bool isCursorDown() {
    if (type != escape_sequence) {
      return false;
    }




    return ( seq == "[B" );
  }

  bool isCursorLeft() {
    if (type != escape_sequence) {
      return false;
    }




    return ( seq == "[D" );
  }

  bool isCursorRigth() {
    if (type != escape_sequence) {
      return false;
    }




    return ( seq == "[C" );
  }
};
}
}

#endif  // ifndef _AU_CONSOLE_ENTRY
