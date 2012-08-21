
#ifndef _H_AU_OBJECT
#define _H_AU_OBJECT

#include "au/containers/list.h"
#include "au/mutex/Token.h"

namespace au {
/* ------------------------------------------------------------------------
 *
 * class au::Object
 *
 * Simple retain-relese model for an object ( multi-thread protected )
 *
 * ------------------------------------------------------------------------*/

class Object {
public:

  Object() : token_("au::Object") {
    retain_counter_ = 1;
  }

  virtual ~Object() {
  };

  // Retain & Release
  void Release();
  void Retain();

  int retain_counter();


private:

  Token token_;           // mutex protection
  int retain_counter_;    // Owner counter
};

template <class C>
class ObjectContainer {
public:

  ObjectContainer() {
    object_ = NULL;
  }

  ~ObjectContainer() {
    if (object_) {
      object_->Release();
    }
  }

  void operator=(ObjectContainer<C>& other) {
    set_object(other.object_);
  }

  void operator=(C *c) {
    set_object(c);
  }

  C *object() {
    return object_;
  }

  void set_object(C *object) {
    // If we have internally the same buffer, do nothing
    if (object == object_) {
      return;
    }



    // If previous buffer, just release
    if (object_) {
      object_->Release();
    }

    // If no input buffer, do nothing
    if (!object) {
      return;
    }



    // Keep a retained copy of this
    object_ = object;
    object_->Retain();
  }

  void Release() {
    if (object_) {
      object_->Release();
      object_ = NULL;
    }
  }

private:

  C *object_;   // Retained object
};

template <class C>
class ObjectList {
public:

  au::list<C> list;   // List of elements

  void push_back(C *c) {
    c->Retain();
    list.push_back(c);
  }

  void push_front(C *c) {
    c->Retain();
    list.push_front(c);
  }

  C *front() {
    return list.front();
  }

  void clear() {
    typename au::list<C>::iterator it;

    for (it = list.begin(); it != list.end(); it++) {
      (*it)->Release();
    }

    list.clear();
  }

  void pop_front() {
    // Relese the front element and remove from list
    list.front()->Release();
    list.pop_front();
  }

  void extract_front(ObjectContainer<C>& container) {
    C *c = list.extractFront();

    if (!c) {
      return;
    }



    container.set_object(c);     // Now it is retained by this element
    c->Release();
  }

  void extract_back(ObjectContainer<C>& container) {
    C *c = list.extractBack();

    if (!c) {
      return;
    }



    container.set_object(c);     // Now it is retained by this element
    c->Release();
  }

  size_t size() {
    return list.size();
  }
};
}

#endif  // ifndef _H_AU_OBJECT