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

#include "Uint64Vector.h"
namespace au {
Uint64Vector::Uint64Vector() {
}

Uint64Vector::Uint64Vector(std::vector<size_t> v) {
  copyFrom(v);
}

size_t Uint64Vector::getNumberOfTimes(size_t t) {
  size_t num = 0;

  for (size_t i = 0; i < size(); i++) {
    if ((*this)[i] == t) {
      num++;
    }
  }
  return num;
}

std::set<size_t> Uint64Vector::getSet() {
  std::set<size_t> set;
  for (size_t i = 0; i < size(); i++) {
    set.insert((*this)[i]);
  }
  return set;
}

void Uint64Vector::copyFrom(std::vector<size_t> &v) {
  std::set<size_t> set;
  for (size_t i = 0; i < v.size(); i++) {
    push_back(v[i]);
  }
}

std::string Uint64Vector::str() {
  std::ostringstream output;

  for (size_t i = 0; i < size(); i++) {
    output << (*this)[i] << " ";
  }
  return output.str();
}

std::vector<int> GetVectorOfInts(const au::StringVector& vector) {
  std::vector<int> output_vector;
  for (size_t i = 0; i < vector.size(); i++) {
    output_vector.push_back(atoi(vector[i].c_str()));
  }
  return output_vector;
}

std::vector<int> GetVectorOfElementsLowerThan(const std::vector<int> & vector, int limit) {
  std::vector<int> output_vector;
  for (size_t i = 0; i < vector.size(); i++) {
    if (vector[i] < limit) {
      output_vector.push_back(vector[i]);
    }
  }
  return output_vector;
}
}