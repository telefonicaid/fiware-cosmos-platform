
#ifndef _H_AU_SHARED_POINTER
#define _H_AU_SHARED_POINTER

#include <memory>

#include "au/containers/list.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "logMsg/logMsg.h"                          // LM_T
#include "logMsg/traceLevels.h"

namespace au {
/* ------------------------------------------------------------------------
 *
 * class au::SharedPointer
 *
 * Shared pointer class
 *
 * ------------------------------------------------------------------------*/

class SharedReferenceCounter {
public:

  // Note: NULL is allowed

  SharedReferenceCounter() {
    reference_counter_ = 1;
  }

  ~SharedReferenceCounter() {
  }

  void Retain() {
    au::TokenTaker tt(&token_);

    reference_counter_++;
  }

  int Release() {
    au::TokenTaker tt(&token_);

    return --reference_counter_;
  }

  int count() {
    au::TokenTaker tt(&token_);

    return reference_counter_;
  }

private:

  int reference_counter_;
  au::Token token_;  // Mutex protection for multi-thread
};

template <class C>
class SharedPointer {
public:

  explicit SharedPointer(C *c = NULL) {
    // Pointing to a newly created element
    shared_reference_counter_ = new SharedReferenceCounter();
    c_ = c;
  }

  SharedPointer(const SharedPointer<C>& shared_pointer) {
    if (!shared_pointer.shared_reference_counter_) {
      LM_X(1, ("Internal error in au::SharedPoint"));
    }

    shared_reference_counter_ = shared_pointer.shared_reference_counter_;
    shared_reference_counter_->Retain();
    c_ = shared_pointer.c_;
  }

  ~SharedPointer() {
    LM_T(LmtCleanup2, ("Called ~SharedPointer this:%p with counter:%d", this, shared_reference_counter_->count()));
    // Release previous assignment
    Release();
  }

  C *operator->() const {
    return c_;
  }

  C& operator*() const {
    return *c_;
  }

  C *shared_object() const {
    return c_;
  }

  bool operator==(C *c) const {
    return ( c_ == c );
  }

  bool operator!=(C *c) const {
    return ( c_ != c );
  }

  template <class T>
  bool operator==(const SharedPointer<T>& shared_pointer) const {
    return (shared_reference_counter_ == shared_pointer.shared_reference_counter_ );
  }

  template <class T>
  bool operator!=(const SharedPointer<T>& shared_pointer) const {
    return (shared_reference_counter_ != shared_pointer.shared_reference_counter_ );
  }

  SharedPointer<C>& operator=(const SharedPointer<C>& shared_pointer) {
    Release();

    shared_reference_counter_ = shared_pointer.shared_reference_counter_;
    shared_reference_counter_->Retain();
    c_ = shared_pointer.c_;

    return *this;
  }

  SharedPointer<C>& operator=(C *c) {
    Reset(c);
    return *this;
  }

  void Reset(C *c) {
    Release();

    // Pointing to a newly created element
    shared_reference_counter_ = new SharedReferenceCounter();
    c_ = c;
  }

  int reference_count() {
    return shared_reference_counter_->count();
  }

  // Casting

  template<class T>
  SharedPointer<T> static_pointer_cast() const {
    return SharedPointer<T>(shared_reference_counter_, static_cast<T *>( shared_object()));
  }

  template<class T>
  SharedPointer<T> const_pointer_cast() const {
    return SharedPointer<T>(shared_reference_counter_, const_cast<T *>( shared_object()));
  }

  template<class T>
  SharedPointer<T> dynamic_pointer_cast() const {
    T *t = dynamic_cast<T *>( c_ );

    if (t == NULL) {
      return SharedPointer<T> (NULL);   // Different unrelated shared pointer to NULL
    }
    return SharedPointer<T>(shared_reference_counter_, t);
  }

private:

  // Release previous assignation
  void Release() {
    // Release in the reference counter & remove object if necessary
    if (shared_reference_counter_->Release()  == 0) {
      if (c_) {
        delete c_;
        c_ = NULL;
      }
      delete shared_reference_counter_;
    }
  }

  // Special constructor for castings
  SharedPointer(SharedReferenceCounter *shared_refence_counter, C *c) {
    // Pointing to a newly created element
    shared_reference_counter_ = shared_refence_counter;
    shared_reference_counter_->Retain();
    c_ = c;
  }

  template<class T>
  friend class SharedPointer;

  SharedReferenceCounter *shared_reference_counter_;
  C *c_;
};
}

#endif  // ifndef _H_AU_SHARED_POINTER
