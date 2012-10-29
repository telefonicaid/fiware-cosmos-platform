
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

#include "au/string/StringUtilities.h"

#include "au/containers/StringVector.h"  // Own interface

namespace au {
StringVector::StringVector() {
}

StringVector::StringVector(const std::string& value) {
  push_back(value);
}

StringVector::StringVector(const std::string& value1,
                           const std::string& value2) {
  push_back(value1);
  push_back(value2);
}

StringVector::StringVector(const std::string& value1,
                           const std::string& value2,
                           const std::string& value3) {
  push_back(value1);
  push_back(value2);
  push_back(value3);
}

StringVector::StringVector(const std::string& value1,
                           const std::string& value2,
                           const std::string& value3,
                           const std::string& value4) {
  push_back(value1);
  push_back(value2);
  push_back(value3);
  push_back(value4);
}

StringVector::StringVector(const std::string& value1,
                           const std::string& value2,
                           const std::string& value3,
                           const std::string& value4,
                           const std::string& value5) {
  push_back(value1);
  push_back(value2);
  push_back(value3);
  push_back(value4);
  push_back(value5);
}

StringVector::StringVector(const std::string& value1,
                           const std::string& value2,
                           const std::string& value3,
                           const std::string& value4,
                           const std::string& value5,
                           const std::string& value6) {
  push_back(value1);
  push_back(value2);
  push_back(value3);
  push_back(value4);
  push_back(value5);
  push_back(value6);
}

StringVector::StringVector(const std::string& value1,
                           const std::string& value2,
                           const std::string& value3,
                           const std::string& value4,
                           const std::string& value5,
                           const std::string& value6,
                           const std::string& value7) {
  push_back(value1);
  push_back(value2);
  push_back(value3);
  push_back(value4);
  push_back(value5);
  push_back(value6);
  push_back(value7);
}

StringVector::StringVector(const std::set<std::string>& values) {
  std::set<std::string>::iterator it_values;
  for (it_values = values.begin(); it_values != values.end(); ++it_values) {
    push_back(*it_values);
  }
}

StringVector::StringVector(const std::vector<std::string>& values) {
  this->resize(values.size());
  std::copy(values.begin(), values.end(), this->begin());
}

StringVector::StringVector(const StringVector& string_vector) {
  this->resize(string_vector.size());
  std::copy(string_vector.begin(), string_vector.end(), this->begin());
}

StringVector StringVector::ParseFromString(const std::string& line,
                                           char separator) {
  StringVector string_vector = au::split(line, separator);

  return string_vector;
}

const StringVector& StringVector::operator=(const StringVector& values) {
  this->clear();        // Remove old values
  this->resize(values.size());
  std::copy(values.begin(), values.end(), this->begin());
  return *this;
}

void StringVector::Append(const StringVector& values) {
  size_t size = this->size();

  this->resize(size + values.size());
  std::copy(values.begin(), values.end(), this->begin() + size);
}

std::string StringVector::Get(size_t pos) const {
  if (pos >= size()) {
    return "";
  }
  return (*this)[pos];
}

void StringVector::RemoveDuplicated() {
  std::set<std::string> unique_values;
  for (size_t i = 0; i < size(); ++i) {
    unique_values.insert((*this)[i]);
  }

  clear();
  std::set<std::string>::iterator it_unique_values;
  for (it_unique_values = unique_values.begin();
       it_unique_values != unique_values.end(); ++it_unique_values)
  {
    push_back(*it_unique_values);
  }
}

std::string StringVector::str() {
  std::ostringstream output;

  for (size_t i = 0; i < size(); ++i) {
    output << (*this)[i];
    if (i != ( size() - 1 )) {
      output << " ";
    }
  }
  return output.str();
}
}

