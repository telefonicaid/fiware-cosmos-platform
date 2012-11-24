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
* FILE            list
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*      Specialized version of the std::list when included objects are pointers.
*      In this case, we can return NULL if object is not found in the list
*      It greatly simplifies development of objects managers in memory
*
* ****************************************************************************/

#ifndef _H_AU_VECTOR
#define _H_AU_VECTOR

#include <list>            // std::list
#include <map>             // std::map
#include <set>             // std::set
#include <vector>       // std::vector

#include "logMsg/logMsg.h"  // LOG_SM()




namespace au {
/**
 * CLass to use list structures of <class*> with additional function for easy manipulation
 */

template <class V>
class vector : public std::vector<V *>{
public:

  void clearVector() {
    for (size_t i = 0; i < std::vector<V *>::size(); i++) {
      delete (*this)[i];
    }
    std::vector<V *>::clear();
  }

  bool removeFromVector(V *v) {
    bool result = false;

    typename std::vector<V *>::iterator it;

    for (it = std::vector<V *>::begin(); it != std::vector<V *>::end(); ) {
      if (*it == v) {
        it = std::vector<V *>::erase(it);
        result = true;
      } else {
        it++;
      }
    }

    return result;
  }

  V *extract_back() {
    size_t size = std::vector<V *>::size();

    if (size == 0) {
      return NULL;
    }

    V *v = (*this)[size - 1];
    std::vector<V *>::pop_back();

    return v;
  }

  void reverse() {
    std::vector<V *> tmp;
    size_t num = std::vector<V *>::size();
    for (size_t i = 0; i < num; i++) {
      tmp.push_back((*this)[num - i - 1]);
    }
    std::vector<V *>::clear();
    for (size_t i = 0; i < num; i++) {
      std::vector<V *>::push_back(tmp[i]);
    }
  }
};

template <class C>
std::vector<C> vector_from_set(const std::set<C> set) {
  std::vector<C> vector;
  typename std::set<C>::iterator it;
  for (it = set.begin(); it != set.end(); it++) {
    vector.push_back(*it);
  }
  return vector;
}

template <class C>
std::set<C> set_from_vector(const std::vector<C> set) {
  std::set<C> vector;
  for (size_t i = 0; i < vector.size(); ++i) {
    vector.insert(vector[i]);
  }
  return vector;
}
}

#endif  // ifndef _H_AU_VECTOR
