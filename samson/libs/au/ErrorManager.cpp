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
void ErrorManager::AddError(std::string message) {
  items_.push_back(new ErrorManagerItem(au::error, message));
}

void ErrorManager::AddWarning(std::string message) {
  items_.push_back(new ErrorManagerItem(au::warning, message));
}

void ErrorManager::AddMessage(std::string message) {
  items_.push_back(new ErrorManagerItem(au::message, message));
}

void ErrorManager::Add(const ErrorManager& error, const std::string& prefix_message) {
  for (size_t i = 0; i < error.items_.size(); ++i) {
    std::string message = prefix_message + " " + error.items_[i]->message();
    items_.push_back(new ErrorManagerItem(error.items_[i]->type(), message));
  }
}

void ErrorManager::operator<<(const std::string& error_message) {
  AddError(error_message);
}

bool ErrorManager::Has(ErrorManagerItemType type) const {
  for (size_t i = 0; i < items_.size(); ++i) {
    if (items_[i]->type() == type) {
      return true;
    }
  }
  return false;
}

std::string ErrorManager::GetLast(ErrorManagerItemType type) const {
  size_t num_errors = items_.size();

  for (size_t i = 0; i < num_errors; ++i) {
    if (items_[num_errors - 1 - i]->type() == type) {
      return items_[num_errors - 1 - i]->message();
    }
  }
  return "";
}

bool ErrorManager::HasErrors() const {
  return Has(au::error);
}

bool ErrorManager::HasWarnings() const {
  return Has(au::warning);
}

bool ErrorManager::HasMessages() const {
  return Has(au::message);
}

std::string ErrorManager::GetLastError() const {
  return GetLast(au::error);
}

std::string ErrorManager::GetLastWarning() const {
  return GetLast(au::warning);
}

std::string ErrorManager::GetLastMessage() const {
  return GetLast(au::message);
}

size_t ErrorManager::GetNumItems() const {
  return items_.size();
}

void ErrorManager::Reset() {
  // Reset messages and errors.
  items_.clearVector();
}

std::string ErrorManager::str() {
  std::ostringstream output;

  // Transform the error message in something to print on screen
  for (size_t i = 0; i < items_.size(); ++i) {
    ErrorManagerItem *item = items_[i];

    switch (item->type()) {
      case au::message:
        output << item->message();
        break;

      case au::warning:
        output << "\033[1;35m" << item->message() << "\033[0m";
        break;
      case au::error:
        output << "\033[1;31m" << item->message() << "\033[0m";
        break;
    }
  }

  return output.str();
}

// Accessor methods
const au::vector<ErrorManagerItem>& ErrorManager::items() {
  return items_;
}
}
