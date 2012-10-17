/* ****************************************************************************
 *
 * FILE            map
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Specialized version of the std::map when "value" objects are pointers.
 *      In this case, we can return NULL if object is not found in the map
 *      It greatly simplifies development of objects managers in memory
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_MAP
#define _H_AU_MAP

#include <functional>      // std::less
#include <iostream>
#include <list>            // std::list
#include <map>             // std::map
#include <set>             // std::set
#include <string.h>
#include <utility>      // std::pair<>
#include <vector>       // std::vector

#include "logMsg/logMsg.h"  // LM_M()

// Usefull define for full iteration over a map structure

namespace au {
/**
 * Class to use map structures of <typedef,class*> with addittional function for easy manitpulation
 */

template<typename K, typename V, typename _Compare = std::less<K> >
class map : public std::map<K, V *, _Compare> {
  public:
    // Iterator definition
    typename std::map<K, V *>::iterator iter;

    // Insert a pair of elements ( easy method )
    // If a previous element was inserted with the same key, it is automatically deleted
    void insertInMap(const K& key, V *value) {
      V *tmp = extractFromMap(key);

      if (tmp) {
        delete tmp;
      }

      std::map<K, V *, _Compare>::insert(std::pair<K, V *>(key, value));
    }

    // Insert a pair of elements ( easy method )
    // No check over previous
    void insertNewInMap(const K& key, V *value) {
      insert(std::pair<K, V *>(key, value));
    }

    /*
     * Function to easyly get pointers in a std::map < value , Pointer* >
     * NULL if not found
     */

    V *findInMap(const K& key) const {
      typename std::map<K, V *, _Compare>::const_iterator iter = std::map<K, V *, _Compare>::find(key);

      if (iter == std::map<K, V *, _Compare>::end()) {
        return NULL;
      }

      return iter->second;
    }

    /**
     * Function to easily get value for a key creating if necessary ( only used with simple constructor )
     */

    V *findOrCreate(const K& key) {
      V *tmp = findInMap(key);

      if (!tmp) {
        tmp = new V();
        insertInMap(key, tmp);
      }
      return tmp;
    }

    template<typename T>
    V *findOrCreate(const K& key, T a) {
      V *tmp = findInMap(key);

      if (!tmp) {
        tmp = new V(a);
        insertInMap(key, tmp);
      }
      return tmp;
    }

    /**
     * Function to remove a particular entry if exist
     * Return if it really existed
     */

    bool removeInMap(const K& key) {
      typename std::map<K, V *, _Compare>::iterator iter = std::map<K, V *, _Compare>::find(key);

      if (iter == std::map<K, V *, _Compare>::end()) {
        return false;
      } else {
        delete iter->second;
        std::map<K, V *, _Compare>::erase(iter);
        return true;
      }
    }

    void removeInMap(const std::set<K>& keys) {
      typename std::set<K>::iterator iter;
      for (iter = keys.begin(); iter != keys.end(); iter++) {
        removeInMap(*iter);
      }
    }

    // Remove all elements that satify a bool function
    // Returns the number of removed elements

    int removeInMapIfFinished() {
      typename std::map<K, V *, _Compare>::iterator iter;

      std::set<K> keys_to_remove;

      for (iter = std::map<K, V *, _Compare>::begin(); iter != std::map<K, V *, _Compare>::end(); iter++) {
        if (iter->second->finished()) {
          keys_to_remove.insert(iter->first);
        }
      }

      typename std::set<K>::iterator k;

      for (k = keys_to_remove.begin(); k != keys_to_remove.end(); k++) {
        removeInMap(*k);
      }

      return keys_to_remove.size();
    }

    void RemoveKeysNotIncludedIn(const std::set<K>& keys) {
      typename std::map<K, V *, _Compare>::iterator iter;
      for (iter = std::map<K, V *, _Compare>::begin(); iter != std::map<K, V *, _Compare>::end();) {
        if (keys.find(iter->first) == keys.end()) {
          std::map<K, V *, _Compare>::erase(iter++);
        } else
          ++iter;
      }
    }

    V *extractFromMap(const K& key) {
      typename std::map<K, V *, _Compare>::iterator iter = std::map<K, V *, _Compare>::find(key);

      if (iter == std::map<K, V *, _Compare>::end()) {
        return NULL;
      } else {
        V *v = iter->second;
        std::map<K, V *, _Compare>::erase(iter);
        return v;
      }
    }

    void clearMap() {
      typename std::map<K, V *, _Compare>::iterator iter;

      for (iter = std::map<K, V *, _Compare>::begin(); iter != std::map<K, V *, _Compare>::end(); iter++) {
        delete iter->second;
      }
      std::map<K, V *, _Compare>::clear();
    }

    /* Funtion to get the keys */

    std::set<K> getKeys() {
      std::set<K> childrens;
      typename std::map<K, V *, _Compare>::iterator iter;
      for (iter = std::map<K, V *, _Compare>::begin(); iter != std::map<K, V *, _Compare>::end(); iter++) {
        childrens.insert(iter->first);
      }
      return childrens;
    }

    std::vector<K> getKeysVector() const {
      std::vector<K> v;

      typename std::map<K, V *, _Compare>::const_iterator iter;
      for (iter = std::map<K, V *, _Compare>::begin(); iter != std::map<K, V *, _Compare>::end(); iter++) {
        v.push_back(iter->first);
      }
      return v;
    }
};
}

#endif  // ifndef _H_AU_MAP
