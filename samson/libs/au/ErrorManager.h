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
* Simple but usefull error mesasage information
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
class ErrorMessage {
public:

  enum ErrorType {
    item_message,
    item_warning,
    item_error
  };

  ErrorMessage(ErrorType type, std::list<std::string> &contexts, std::string message);
  ~ErrorMessage() {
    contexts_.clear();
  }

  std::string GetMessage() const;
  std::string GetMultiLineMessage() const;
  ErrorType type() const;

private:

  ErrorType type_;
  std::vector<std::string> contexts_;   // List of context where this error happened
  std::string message_;
};


class ErrorManager {
public:

  ErrorManager();
  ~ErrorManager() {
    errors_.clearVector();
    contexts_.clear();
  };

  // Old method to add an error
  void set(std::string _message);

  // Methods to add elements
  void AddError(std::string message);
  void AddWarning(std::string message);
  void AddMessage(std::string message);

  // Add all elements from another Error manager adding some string to the messages
  void Add(const ErrorManager& error, const std::string& prefix_message);

  // Check if there is at least one error
  bool IsActivated() const;

  // Check if there are warnings
  bool HasWarnings() const;

  // Get the last error message
  std::string GetMessage() const;

  // Complete message error
  std::string GetErrorMessage(const std::string& message) const;

  // Get the all errors message in multiple lines
  std::string GetCompleteMessage();

  // Get first warning
  std::string GetLastWarning() const;

  // Reset messages and errors.
  void Reset();

  // Concert into something printable at console
  std::string str();

  // Accessor methods
  const au::vector<ErrorMessage>& errors();

  // Add error with <<
  void operator<<(const std::string& error_message);

private:

  // Add a context to this error manager
  void PushContext(std::string context);
  void PopContext();

  // List of errors messages
  au::vector<ErrorMessage> errors_;

  // Current list of context to emit errors
  std::list<std::string> contexts_;

  // Friend class to access PushContext & PopContext
  friend class ErrorContext;
};

// Class to define context over error messages

class ErrorContext {
public:

  ErrorContext(ErrorManager& error, std::string content);
  ~ErrorContext();

private:

  ErrorManager& error_manager_;
};
}
#endif  // ifndef AU_ERROR_MANAGER_H_
