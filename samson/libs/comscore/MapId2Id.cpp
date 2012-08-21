

#include "MapId2Id.h"  // Own interface

namespace samson {
namespace comscore {
bool compareId2Id(const Id2Id&a, const Id2Id&b) {
  if (a.first != b.first)
    return ( a.first < b.first ); return ( a.second < b.second );
}
}
}
