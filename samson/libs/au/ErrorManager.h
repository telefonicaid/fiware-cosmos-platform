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
* FILE            ErrorManager.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
* Simple but useful error mesasage information
* Whatever sistem that can report an error can activate this error providing a message
*
* ****************************************************************************/

#ifndef AU_ERROR_MANAGER_H_
#define AU_ERROR_MANAGER_H_

#include "au/containers/list.h"
#include "au/containers/vector.h"
#include <set>
#include <sstream>              // std::ostringstream
#include <vector>

namespace au {
enum ErrorManagerItemType {
  message,
  warning,
  error
};

class ErrorManagerItem {
public:

  ErrorManagerItem(ErrorManagerItemType type, const std::string& message) : type_(type), message_(message) {
  }

  ErrorManagerItemType type() const {
    return type_;
  }

  std::string message() const {
    return message_;
  }

private:

  ErrorManagerItemType type_;
  std::string message_;
};


class ErrorManager {
public:

  ErrorManager() {
  }

  ~ErrorManager() {
    items_.clearVector();
  };

  // Methods to add elements
  void AddError(std::string message);
  void AddWarning(std::string message);
  void AddMessage(std::string message);

  // Add all elements from another Error manager adding some string to the messages
  void Add(const ErrorManager& error, const std::string& prefix_message);

  // Check if there are elements of a particular type
  bool HasMessages() const;
  bool HasErrors() const;
  bool HasWarnings() const;

  // Check the number of a particular type
  size_t GetNumErrors() const;
  size_t GetNumWarnings() const;
  size_t GetNumMessages() const;

  // Get the last error message
  std::string GetLastError() const;

  // Get first warning
  std::string GetLastWarning() const;

  // Get the last message
  std::string GetLastMessage() const;

  // Get total number of items
  size_t GetNumItems() const;

  // Reset messages and errors.
  void Reset();

  // Concert into something printable at console
  std::string str();

  // Accessor methods
  const au::vector<ErrorManagerItem>& items();

  // Add error with <<
  void operator<<(const std::string& error_message);

private:

  // Generic methos for a type
  bool Has(ErrorManagerItemType type) const;
  std::string GetLast(ErrorManagerItemType type) const;

  // List of errors messages
  au::vector<ErrorManagerItem> items_;
};
}
#endif  // ifndef AU_ERROR_MANAGER_H_
