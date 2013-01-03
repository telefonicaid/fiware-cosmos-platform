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
* FILE            set
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            September 2011
*
* DESCRIPTION
*
*      Specialized version of the std::set when included objects are pointers.
*      In this case, we can return NULL if object is not found in the set
*      It greatly simplifies development of objects managers in memory
*
* ****************************************************************************/

#ifndef _H_AU_SET
#define _H_AU_SET

#include "logMsg/logMsg.h"  // LOG_SM()
#include <list>            // std::list
#include <map>             // std::map
#include <set>             // std::set
#include <vector>       // std::vector




namespace au {
/**
 * CLass to use list structures of <class*> with additional function for easy manipulation
 */

template <class V>
class set : public std::set<V *>{
public:

  /*
   * Remove all the elements in the set, deleting each one of them
   * Please, use clear if you just want to clear the set ( without deleting objects )
   */

  void clearSet() {
    typename std::set<V * >::iterator iter;

    for (iter =  std::set<V *>::begin(); iter != std::set<V *>::end(); ++iter) {
      delete *iter;
    }
    std::set<V *>::clear();
  }

  /*
   * Extract a partircular element from the set.
   * Return value: The inptu value, if it was in the set, NULL otherwise
   */

  V *extractFromSet(V *v) {
    typename std::set<V *>::iterator iter = std::set<V *>::find(v);

    if (iter == std::set<V *>::end()) {
      return NULL;
    } else {
      // Remove from the set
      std::set<V *>::erase(v);
      return v;
    }
  }
};
}

#endif  // ifndef _H_AU_SET
