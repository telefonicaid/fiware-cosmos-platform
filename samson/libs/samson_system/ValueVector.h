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
#ifndef _H_SAMSON_system_VALUE_VECTOR
#define _H_SAMSON_system_VALUE_VECTOR

#include <stdlib.h>    // malloc()

#include "samson_system/Value.h"

namespace samson {
namespace system {
/*
 *
 * ValueVector
 *
 * Vector of values. Specially useful when parsing in an operation
 *
 */

class ValueVector {
  public:
    Value **values_;
    size_t num_values_;
    size_t max_num_values_;

    ValueVector() : values_(NULL), num_values_(0), max_num_values_(0) {}

    ~ValueVector() {
      clear();   // Clear the vector reusing all component
      if (values_ != NULL) {
        free(values_);   // Free allocated memory for the vector
        values_ = NULL;
      }
    }

    void ReserveValues(size_t size) {
      if (max_num_values_ >= size) {
        return;   // Nothing to do
      }
      size_t previous_max_num_values = max_num_values_;
      while (max_num_values_ < size) {
        if (max_num_values_ == 0) {
          max_num_values_ = 1;
        } else {
          max_num_values_ *= 2;
        }
      }

      // Alloc new vector
      Value **new_values = static_cast<Value **>(malloc(sizeof(*new_values) * max_num_values_));
      if (new_values == NULL) {
        LM_E(("Error in malloc for new_values of %lu bytes", sizeof(*new_values) * max_num_values_));
        abort();
      }
      for (size_t i = 0; i < previous_max_num_values; ++i) {
        new_values[i] = values_[i];
      }
      for (size_t i = previous_max_num_values; i < max_num_values_; ++i) {
        new_values[i] = NULL;
      }

      free(values_);
      values_ = new_values;
    }

    void clear() {
      for (size_t i = 0; i < num_values_; ++i) {
        delete values_[i];
        values_[i] = NULL;
      }
      num_values_ = 0;
    }

    void add(char *data) {
      Value *value = new Value();

      value->parse(data);   // Parse data

      // Add in the vector
      ReserveValues(num_values_ + 1);   // Make sure we have space for this new element
      values_[num_values_] = value;   // Put in place
      num_values_++;   // Increase the real length of the vector
    }
};
}
}   // End of namespace

#endif  // ifndef _H_SAMSON_system_VALUE_VECTOR
