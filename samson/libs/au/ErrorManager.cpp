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

#include "au/ErrorManager.h"      // Own interface

namespace au {
ErrorMessage::ErrorMessage(ErrorType type, std::list<std::string> &contexts, std::string message)
  : type_(type)
    , message_(message) {
  std::list<std::string>::iterator it_contexts;
  for (it_contexts = contexts.begin(); it_contexts != contexts.end(); it_contexts++) {
    contexts_.push_back(*it_contexts);
  }
}

ErrorMessage::~ErrorMessage() {
}

// Get message in one line
std::string ErrorMessage::GetMessage() const {
  std::ostringstream output;

  for (size_t i = 0; i < contexts_.size(); i++) {
    output << contexts_[i] << " >> ";
  }
  output << message_;
  return output.str();
}

std::string ErrorManager::GetErrorMessage( const std::string& message) const
{
  std::ostringstream output;
  output << message << " ( " << GetMessage() << " )";
  return output.str();
}
  
std::string ErrorMessage::GetMultiLineMessage() const {
  std::ostringstream output;

  if (item_message == item_error) {
    output << "ERROR: ";
  }
  for (size_t i = 0; i < contexts_.size(); i++) {
    output << ">> " << contexts_[i] << "\n";
  }
  output << message_ << "\n";
  return output.str();
}

ErrorMessage::ErrorType ErrorMessage::type() const {
  return type_;
}

// ErrorManager

ErrorManager::ErrorManager() {
}

void ErrorManager::set(std::string message) {
  errors_.push_back(new ErrorMessage(ErrorMessage::item_error, contexts_, message));
}

void ErrorManager::AddError(std::string message) {
  errors_.push_back(new ErrorMessage(ErrorMessage::item_error, contexts_, message));
}

void ErrorManager::AddWarning(std::string message) {
  errors_.push_back(new ErrorMessage(ErrorMessage::item_warning, contexts_, message));
}

void ErrorManager::AddMessage(std::string message) {
  errors_.push_back(new ErrorMessage(ErrorMessage::item_message, contexts_, message));
}

void ErrorManager::operator<<(const std::string& error_message) {
  AddError(error_message);
}

bool ErrorManager::IsActivated() const {
  for (size_t i = 0; i < errors_.size(); i++) {
    if (errors_[i]->type() == ErrorMessage::item_error) {
      return true;
    }
  }
  return false;
}

std::string ErrorManager::GetMessage() const {
  // Get one line of the last error
  if (errors_.size() == 0) {
    return "No errors";
  } else {
    return errors_.back()->GetMessage();
  }
}

std::string ErrorManager::GetCompleteMessage() {
  std::ostringstream output;

  for (size_t i = 0; i < errors_.size(); i++) {
    output << errors_[i]->GetMultiLineMessage();
  }
  return output.str();
}

void ErrorManager::PushContext(std::string context) {
  contexts_.push_back(context);
}

void ErrorManager::PopContext() {
  contexts_.pop_back();
}

void ErrorManager::Reset() {
  // Reset messages and errors.
  errors_.clearVector();
}

std::string ErrorManager::str() {
  std::ostringstream output;

  // Transform the error message in something to print on screen
  for (size_t i = 0; i < errors_.size(); i++) {
    ErrorMessage *item = errors_[i];

    switch (item->type()) {
      case ErrorMessage::item_message:
        output << item->GetMultiLineMessage();
        break;

      case ErrorMessage::item_warning:
        output << "\033[1;35m" << item->GetMultiLineMessage() << "\033[0m";
        break;
      case ErrorMessage::item_error:
        output << "\033[1;31m" << item->GetMultiLineMessage() << "\033[0m";
        break;
    }
  }

  return output.str();
}

// Accessor methods
const au::vector<ErrorMessage>& ErrorManager::errors() {
  return errors_;
}

ErrorContext::ErrorContext(ErrorManager& error_manager, std::string context)
  : error_manager_(error_manager) {
  error_manager_.PushContext(context);
}

ErrorContext::~ErrorContext() {
  error_manager_.PopContext();
}
}