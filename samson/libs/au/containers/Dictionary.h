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
* FILE            Dictionary
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*      Specialized version of the std::map when "values" are shared pointers
*      In this case, we can return a shared pointer to NULL if object is
*      not found in the map.
*      It greatly simplifies development of objects managers in memory
*
* ****************************************************************************/

#ifndef AU_CONTAINERS_DICTIONARY_H_
#define AU_CONTAINERS_DICTIONARY_H_

#include <iostream>
#include <list>            // std::list
#include <map>             // std::map
#include <set>             // std::set
#include <string.h>
#include <vector>       // std::vector

#include "au/containers/SharedPointer.h"
#include "logMsg/logMsg.h"  // LOG_SM()

// useful define for full iteration over a map structure

namespace au {
/* ------------------------------------------------------------------------
 *
 *  class au::map
 *
 *  map of SharedPointer's
 *
 *  ------------------------------------------------------------------------ */

template <typename K, typename V, typename _Compare = std::less<K> >
class Dictionary : public std::map< K, SharedPointer<V>, _Compare >{
public:

  // Iterator definition
  typename std::map< K, SharedPointer<V>, _Compare > type_map;
  typename std::map<K, V * >::iterator type_iterator;

  Dictionary() {
  }

  ~Dictionary() {
    // Make sure all elements are removed
    // This is included in std::map destructor ( just in case ;) )
    std::map< K, SharedPointer<V>, _Compare >::clear();
  }

  // Insert a pair of elements ( easy method )
  // If a previous element was inserted with the same key, it is automatically deleted
  void Set(const K& key, const SharedPointer<V>& value) {
    Extract(key);          // Extract any previous reference ( if any )
    std::map< K, SharedPointer<V>, _Compare >::insert(std::pair<K, SharedPointer<V> >(key, value));
  }

  // Handy function to add new elements directly
  // It is expecetd to reeive new allocated objects
  void Set(const K& key, V *value) {
    SharedPointer<V> tmp_value(value);
    Set(key, tmp_value);
  }

  bool ContainsKey(const K& key) const {
    return (std::map<K, SharedPointer<V>, _Compare >::find(key) !=
            std::map<K, SharedPointer<V>, _Compare>::end());
  }

  SharedPointer<V> Get(const K& key) const {
    typename std::map<K, SharedPointer<V>,
                      _Compare >::const_iterator iter = std::map<K, SharedPointer<V>, _Compare >::find(key);

    if (iter == std::map<K, SharedPointer<V>, _Compare>::end()) {
      return SharedPointer<V>(NULL);
    }

    return iter->second;
  }

  SharedPointer<V> GetOrCreate(const K& key) {
    SharedPointer<V> tmp = Get(key);
    if (tmp != NULL) {
      return tmp;
    }

    // Create a new object
    tmp = new V();
    Set(key, tmp);
    return tmp;
  }

  template <typename T>
  SharedPointer<V> GetOrCreate(const K& key, T a) {
    SharedPointer<V> tmp = Get(key);
    if (tmp != NULL) {
      return tmp;
    }

    tmp = new V(a);
    Set(key, tmp);
    return tmp;
  }

  void Extract(const std::set<K>& keys) {
    typename std::set<K>::iterator iter;
    for (iter = keys.begin(); iter != keys.end(); ++iter) {
      extractFromMap(*iter);
    }
  }

  SharedPointer<V> Extract(const K& key) {
    typename std::map<K, SharedPointer<V>, _Compare >::iterator iter
      = std::map<K, SharedPointer<V>, _Compare>::find(key);

    if (iter == std::map<K, SharedPointer<V>, _Compare>::end()) {
      return SharedPointer<V>(NULL);
    }

    SharedPointer<V> tmp = iter->second;
    std::map<K, SharedPointer<V>, _Compare>::erase(iter);
    return tmp;
  }

  std::set<K> getKeys() {
    std::set<K> childrens;
    typename std::map<K, V *, _Compare >::iterator iter;
    for (iter = std::map<K, SharedPointer<V>, _Compare>::begin(); iter != std::map<K, SharedPointer<V>, _Compare>::end();
         ++iter)
    {
      childrens.insert(iter->first);
    }
    return childrens;
  }

  std::vector<K> getKeysVector() {
    std::vector<K> v;

    typename std::map<K, SharedPointer<V>, _Compare >::iterator iter;
    for (iter = std::map<K, SharedPointer<V>, _Compare>::begin(); iter != std::map<K, SharedPointer<V>, _Compare>::end();
         ++iter)
    {
      v.push_back(iter->first);
    }
    return v;
  }

  std::vector< SharedPointer<V> > items() const {
    std::vector< SharedPointer<V> > vector;
    typename std::map<K, SharedPointer<V>, _Compare >::const_iterator iter;
    for (iter = std::map<K, SharedPointer<V>, _Compare>::begin(); iter != std::map<K, SharedPointer<V>, _Compare>::end();
         ++iter)
    {
      vector.push_back(iter->second);
    }
    return vector;
  }
};

template <class V>
class Vector : public std::vector< SharedPointer<V> >{
};
}

#endif  // ifndef AU_CONTAINERS_DICTIONARY_H_
