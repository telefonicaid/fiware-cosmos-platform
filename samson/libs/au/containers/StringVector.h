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

#ifndef _H_AU_STRING_VECTOR
#define _H_AU_STRING_VECTOR

#include <set>
#include <sstream>             // std::ostringstream
#include <stdio.h>             /* sprintf */
#include <string>              // std::string
#include <sys/time.h>          // struct timeval
#include <vector>

#include "logMsg/logMsg.h"     // LOG_SW

/*
 *
 * StringVector is a handy class to work with vectors of string based on the standard std::vector<std::string>
 * - It allows to create vectors with multiple components in one line
 * - It allows to parse a string into components in one call
 * - It allows to transform into a string for visualization
 * - Other handy methods
 *
 */

namespace au {
class StringVector : public std::vector<std::string>{
public:

  // Constructors with different number of arguments
  StringVector();
  StringVector(const std::string& value);
  StringVector(const std::string& value, const std::string& value2);
  StringVector(const std::string& value, const std::string& value2,
               const std::string& value3);
  StringVector(const std::string& value, const std::string& value2,
               const std::string& value3
               ,
               const std::string& value4);
  StringVector(const std::string& value, const std::string& value2,
               const std::string& value3
               , const std::string& value4,
               const std::string& value5);
  StringVector(const std::string& value, const std::string& value2,
               const std::string& value3
               , const std::string& value4, const std::string& value5,
               const std::string& value6);
  StringVector(const std::string& value, const std::string& value2,
               const std::string& value3
               , const std::string& value4, const std::string& value5,
               const std::string& value6
               ,
               const std::string& value7);

  StringVector(const std::set<std::string>& values);
  StringVector(const std::vector<std::string>& values);
  StringVector(const StringVector& string_vector);

  const StringVector& operator=(const StringVector& values);

  // Parse a line in component using a parituclar separator
  static StringVector ParseFromString(const std::string& line,
                                      char separator = ' ');

  // Append elements contained in other vector
  void Append(const StringVector& values);

  // Get string at a particular position
  std::string Get(size_t pos) const;

  // Remove duplicated values in the vector
  void RemoveDuplicated();

  // Handy punction to add different types of values
  template <typename T>
  void Push(T v) {
    std::ostringstream output;

    output << v;
    push_back(output.str());
  }

  // Debug string
  std::string str();
};
}

#endif  // ifndef _H_AU_STRING_VECTOR
