
/* ****************************************************************************
*
* FILE            ListMap
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*  Combination of a set::map and std::list when objects included are pointers.
*  It contains convinient methods to simplify development
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef _H_AU_LIST_MAP
#define _H_AU_LIST_MAP

#include <list>            // std::list
#include <map>             // std::map
#include <set>             // std::set
#include <vector>       // std::vector

#include "logMsg/logMsg.h"  // LM_M()

namespace au {
/**
 * ListMap is a container that can be used as map and as list.
 * It allows to create an associative container ( map ) but can be access to the front or back elements
 *
 * Now it is implemented with standart library elements but it should be implemented as a map to
 * an intermediate class that is itself a double linked list.
 */

template <typename K, class V>
class ListMap {
public:

  std::map<K, V *> map;  // Real associate map of elements
  std::list<K> keys;    // List of the keys to access front and back elements


  // Iterator definition
  typename std::map<K, V * >::iterator iter;


  // Insert elements

  V *insertAtFront(K& key, V *value) {
    V *tmp =  extractFromMap(key);

    map.insert(std::pair<K, V *>(key, value));

    // Insert the key in the list
    keys.push_front(key);

    return tmp;
  }

  V *insertAtBack(K& key, V *value) {
    V *tmp =  extractFromMap(key);

    map.insert(std::pair<K, V *>(key, value));

    // Insert the key in the list
    keys.push_back(key);

    return tmp;
  }

  // Get reference to the limit elements

  V *getFromBack() {
    if (keys.size() == 0)
      return NULL;




    // Get the key from the back position
    K key = keys.back();

    typename std::map<K, V * >::iterator iter = map.find(key);

    if (iter == map.end()) {
      return NULL;
    } else {
      V *v = iter->second;
      return v;
    }
  }

  V *getFromFront() {
    if (keys.size() == 0)
      return NULL;





    // Get the key from the back position
    K key = keys.front();

    typename std::map<K, V * >::iterator iter = map.find(key);

    if (iter == map.end()) {
      return NULL;
    } else {
      V *v = iter->second;
      return v;
    }
  }

  // Extract elements

  V *extractFromBack() {
    if (keys.size() == 0)
      return NULL;




    // Get the key from the back position
    K key = keys.back();
    keys.pop_back();

    typename std::map<K, V * >::iterator iter = map.find(key);

    if (iter == map.end()) {
      return NULL;
    } else {
      V *v = iter->second;
      map.erase(iter);
      keys.remove(key);
      return v;
    }
  }

  V *extractFromFront() {
    if (keys.size() == 0)
      return NULL;





    // Get the key from the back position
    K key = keys.front();
    keys.pop_front();

    typename std::map<K, V * >::iterator iter = map.find(key);

    if (iter == map.end()) {
      return NULL;
    } else {
      V *v = iter->second;
      map.erase(iter);
      keys.remove(key);
      return v;
    }
  }

  /*
   * Function to easyly get pointers in a std::map < value , Pointer* >
   * NULL if not found
   */

  V *findInMap(K& key) {
    typename std::map<K, V * >::iterator iter = map.find(key);

    if (iter == map.end())
      return NULL;



    return iter->second;
  }

  /**
   * Function to remove a particular entry if exist
   * Return if it really existed
   */

  V *extractFromMap(K& key) {
    typename std::map<K, V * >::iterator iter = map.find(key);

    if (iter == map.end()) {
      return NULL;
    } else {
      V *v = iter->second;
      map.erase(iter);
      keys.remove(key);
      return v;
    }
  }

  size_t size() {
    return map.size();
  }

  void clearListMap() {
    typename std::map<K, V * >::iterator iter;

    for (iter = map.begin(); iter != map.end(); iter++) {
      delete iter->second;
    }

    map.clear();
    keys.clear();
  }
};
}

#endif  // ifndef _H_AU_LIST_MAP
