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
#ifndef _AU_CONSOLE_ESCAPE_SEQUENCE
#define _AU_CONSOLE_ESCAPE_SEQUENCE

#include "au/string/StringUtilities.h"
#include <string>
#include <vector>




namespace au {
typedef enum {
  sequence_non_compatible,     // This sequence is non compatible with introduced sequence
  sequence_unfinished,         // Compatible but not finished
  sequence_finished            // Compatible and finished
} SequenceDetectionCode;


class ConsoleEscapeSequence {
  std::set<std::string> sequences;   // Sequences supported
  std::string current_sequence;      // Current sequence

public:

  ConsoleEscapeSequence();

  // Add sequence to be detected
  void addSequence(std::string sequence) {
    sequences.insert(sequence);
  }

  void addSequence(char c) {
    addSequence(au::str("%c", c));
  }

  void addSequence(char c, char c2) {
    addSequence(au::str("%c%c", c, c2));
  }

  // Init the sequence detector
  void init() {
    current_sequence = "";
    current_sequence.clear();
  }

  // Add a character
  void add(char c) {
    current_sequence += c;
  }

  std::string getCurrentSequence() {
    return current_sequence;
  }

  SequenceDetectionCode checkSequence() {
    // By default is non compatible
    SequenceDetectionCode return_code = sequence_non_compatible;

    std::set<std::string>::iterator it_sequences;
    for (it_sequences = sequences.begin(); it_sequences != sequences.end(); it_sequences++) {
      SequenceDetectionCode code = checkSequence(*it_sequences);

      if (code == sequence_finished) {
        return sequence_finished;          // Return directly...
      }
      if (code == sequence_unfinished) {
        return_code = sequence_unfinished;  // Change the global return value
      }
    }

    return return_code;
  }

private:

  SequenceDetectionCode checkSequence(std::string sequence) {
    if (sequence.length() < current_sequence.length()) {
      return sequence_non_compatible;
    }





    for (size_t i = 0; i < current_sequence.length(); i++) {
      if (sequence[i] != current_sequence[i]) {
        return sequence_non_compatible;
      }
    }

    if (sequence.length() > current_sequence.length()) {
      return sequence_unfinished;
    }





    return sequence_finished;
  }
};
}

#endif  // ifndef _AU_CONSOLE_ESCAPE_SEQUENCE
