
#ifndef _H_SAMSON_system_VALUE_VECTOR
#define _H_SAMSON_system_VALUE_VECTOR


#include "samson_system/Value.h"

namespace samson { namespace system {
                   /*
                    *
                    * ValueVector
                    *
                    * Vector of values. Specially usefull when parsing in an operation
                    *
                    */

                   class ValueVector {
public:

                     Value **values_;
                     size_t num_values_;
                     size_t max_num_values_;

                     ValueVector() {
                       values_ = NULL;
                       num_values_ = 0;
                       max_num_values_ = 0;
                     }

                     ~ValueVector() {
                       clear();  // Clear the vector rehusing all component
                       if (values_) {
                         free(values_);  // Free allocated memory for the vector
                       }
                     }

                     void reserve(size_t size) {
                       if (max_num_values_ >= size) {
                         return;  // Nothing to do
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
                       Value **new_values = (Value **)malloc(sizeof(Value *) * max_num_values_);
                       for (size_t i = 0; i < previous_max_num_values; i++) {
                         new_values[i] = values_[i];
                       }
                       for (size_t i = previous_max_num_values; i < max_num_values_; i++) {
                         new_values[i] = NULL;
                       }

                       free(values_);
                       values_ = new_values;
                     }

                     void clear() {
                       for (size_t i = 0; i < num_values_; i++) {
                         Value::reuseInstance(values_[i]);  // Rehuse this element
                         values_[i] = NULL;
                       }
                       num_values_ = 0;
                     }

                     void add(char *data) {
                       Value *value = Value::getInstance();  // Get a new instance of value from the common pool

                       value->parse(data);  // Parse data

                       // Add in the vector
                       reserve(num_values_ + 1);  // Make sure we have space for this new element
                       values_[num_values_] = value;  // Put in place
                       num_values_++;      // Increase the real length of the vector
                     }
                   };
                   } }  // End of namespace

#endif  // ifndef _H_SAMSON_system_VALUE_VECTOR