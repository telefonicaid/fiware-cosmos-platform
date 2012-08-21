
#include "samson/common/coding.h"
#include "samson/module/ModulesManager.h"

namespace samson {
// Operations to compare formats
bool operator==(const KVFormat & left, const KVFormat & right) {
  if (left.keyFormat != right.keyFormat)
    return false; if (left.valueFormat != right.valueFormat)
    return false; return true;
}

bool operator!=(const KVFormat & left, const KVFormat & right) {
  if (left.keyFormat != right.keyFormat)
    return true; if (left.valueFormat != right.valueFormat)
    return true; return false;
}
}

