
/* ****************************************************************************
*
* FILE            Queue
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*      Specialized version of the std::queue when included objects are shared pointers.
*      In this case, we can return NULL if no object should be returned
*      It greatly simplifies development of objects managers in memory
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef AU_CONTAINERS_QUEUE_H_
#define AU_CONTAINERS_QUEUE_H_

#include "au/containers/SharedPointer.h"
#include <list>            // std::map

#include "logMsg/logMsg.h" // LM_M()

namespace au {
template <class V>
class Queue {
public:

  void Push(const SharedPointer<V>& v) {
    list_.push_back(v);
  }

  // Handy function to push newly allocated objects
  void Push(V *v) {
    SharedPointer<V> tmp_v(v);
    Push(tmp_v);
  }

  SharedPointer<V> Pop() {
    if (list_.size() == 0) {
      return SharedPointer<V>(NULL);
    } else {
      SharedPointer<V> r = list_.front();
      list_.pop_front();
      return r;
    }
  }

  SharedPointer<V> Front() {
    if (list_.size() == 0) {
      return SharedPointer<V>(NULL);
    } else {
      return list_.front();
    }
  }

  SharedPointer<V> Back() {
    if (list_.size() == 0) {
      return NULL;
    } else {
      return list_.back();
    }
  }

  // Extract an element from the queue
  bool Contains(const SharedPointer<V>& v) {
    typename std::list< SharedPointer<V> >::iterator iter;
    for (iter = list_.begin(); iter != list_.end(); iter++) {
      if (*iter == v) {
        return true;
      }
    }
    return false;
  }

  void ExtractAll(const SharedPointer<V>& v) {
    typename std::list< SharedPointer<V> >::iterator iter;
    for (iter = list_.begin(); iter != list_.end(); ) {
      if (*iter == v) {
        list_.erase(iter++);
      } else {
        ++iter;
      }
    }
  }

  size_t size() {
    return list_.size();
  }

  std::vector< SharedPointer<V> > items() {
    std::vector< SharedPointer<V> > vector;
    typename std::list< SharedPointer<V> >::iterator iter;
    for (iter = list_.begin(); iter != list_.end(); iter++) {
      vector.push_back(*iter);
    }
    return vector;
  }

private:

  std::list< SharedPointer<V> > list_;
};
}

#endif // ifndef AU_CONTAINERS_QUEUE_H_
