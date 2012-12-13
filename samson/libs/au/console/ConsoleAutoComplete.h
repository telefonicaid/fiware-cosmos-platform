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
#ifndef _AU_CONSOLE_AUTO_COMPLETE
#define _AU_CONSOLE_AUTO_COMPLETE

#include <string>
#include <vector>

#include "au/string/StringUtilities.h"

namespace au {
namespace console {
/**
 * \brief Alternative for auto completion
 */

class ConsoleAutoCompleteAlternative {
public:

  explicit ConsoleAutoCompleteAlternative(std::string txt) {
    label_ = txt;
    command_ = txt;
    add_space_if_unique_ = true;
  }

  ConsoleAutoCompleteAlternative(std::string label, std::string command, bool add_space_if_unique) {
    command_ = command;
    label_ = label;
    add_space_if_unique_ = add_space_if_unique;
  }

  std::string GetColoredLabel(std::string last_word) const {
    if (last_word.length() > label_.length()) {
      return label_;
    }

    if (label_.substr(0, last_word.length()) == last_word) {
      return au::str(au::BoldRed, "%s", last_word.c_str())
             + au::str(au::BoldMagenta, "%s", label_.substr(last_word.length()).c_str());
    } else {
      return label_;
    }
  }

  // Operation < to be used in std::sort
  friend bool operator<(const ConsoleAutoCompleteAlternative& a, const ConsoleAutoCompleteAlternative& b) {
    return a.label_ < b.label_;
  }

  std::string command() const {
    return command_;
  }

  std::string label() const {
    return label_;
  }

  bool add_space_if_unique() const {
    return add_space_if_unique_;
  }

private:

  friend class ConsoleAutoComplete;

  std::string label_;          // Label to show on screen
  std::string command_;        // Text to be compared with the current last word
  bool add_space_if_unique_;
};

/**
 * \brief Autocomplete information and container for all alternatives
 */

class ConsoleAutoComplete {
public:
  explicit ConsoleAutoComplete(std::string command);

  // Check what is the current situation in auto-completion
  bool completingFirstWord();
  bool completingSecondWord();
  bool completingThirdWord();
  bool completingSecondWord(std::string first_word);
  bool completingThirdWord(std::string first_word, std::string second_word);

  std::string getCurrentCommand();

  int completingWord();
  std::string firstWord();
  std::string secondWord();

  std::string lastCompletedWord();

  const std::vector<std::string>& get_previous_words() {
    return previous_words;
  }

  // Add an alternative for the last word
  void add(const std::string& command);
  void add(std::vector<std::string> commands);
  void add(const std::string& label, const std::string& command, bool add_space);
  void add(ConsoleAutoCompleteAlternative alternative);

  // Set some help message
  void setHelpMessage(std::string _help_message);

  // Add files in the current directory
  void auto_complete_files(std::string file_selector);

  // Get the string to add to the current comman
  std::string stringToAppend();
  std::string getHelpMessage();

  // Auxiliar functions
  int common_chars_in_last_word_alternative();
  bool necessary_print_last_words_alternatives();
  void print_last_words_alternatives();

  // Access options directly
  size_t getNumAlternatives() {
    return last_word_alternatives.size();
  }

  ConsoleAutoCompleteAlternative getAlternative(size_t i) {
    if (i >= last_word_alternatives.size()) {
      LM_X(1, ("Major error"));
    }
    return last_word_alternatives[i];
  }

protected:
  // Current message introduced so far
  std::string current_command;

  // Preivous words ( before the last one )
  std::vector<std::string> previous_words;
  std::string last_word;

  bool add_space_if_unique_alternative;

private:
  // Alternatives for the last word
  std::vector<ConsoleAutoCompleteAlternative> last_word_alternatives;

  std::string help_message;
};
}
}

#endif  // ifndef _AU_CONSOLE_AUTO_COMPLETE
