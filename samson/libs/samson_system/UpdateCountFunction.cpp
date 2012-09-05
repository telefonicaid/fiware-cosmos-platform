#include "UpdateCountFunction.h" // Own interface

#include <math.h>

namespace samson {
namespace system {
double UpdateCountFunction::UpdateCount(double old_count, size_t old_time, size_t new_time) {
  int diff = static_cast <int>(new_time - old_time);

  return (old_count * pow(forgetting_factor_, diff));
}
}
} // End of namespaces
