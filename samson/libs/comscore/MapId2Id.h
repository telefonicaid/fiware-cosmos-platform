
#ifndef _H_SAMSON_COMSCORE_MapId2Id
#define _H_SAMSON_COMSCORE_MapId2Id

#include "logMsg/logMsg.h"

#include <fnmatch.h>
#include <string.h>

#include "au/StringCollection.h"
#include "au/StructCollection.h"
#include "au/string.h"


#include "comscore/common.h"

bool match(const char *s1, const char *s2);

namespace samson {
namespace comscore {
typedef struct {
  uint first;
  uint second;
} Id2Id;

bool compareId2Id(const Id2Id &a, const Id2Id &b);


// Spetial class to map uint to several uints...
class MapId2Id : public au::StructCollection<Id2Id>{
public:

  uint find_one(uint key, uint begin, uint end) {
    // Last interval
    if (end == begin + 1) {
      return (uint) - 1;
    }

    uint mid_point = (begin + end) / 2;

    int c = key - v[mid_point].first;

    if (c == 0) {
      return mid_point;
    }

    if (c < 0) {
      return find_one(key, begin, mid_point);
    } else {
      return find_one(key, mid_point, end);
    }
  }

  uint find_one(uint key) {
    uint begin = 0;
    uint end = size;

    if (v[begin].first == key) {
      return begin;
    }
    if (v[end].first == key) {
      return end;
    }

    return find_one(key, begin, end);
  }

  bool find_range(uint key, uint *begin, uint *end) {
    uint pos = find_one(key);

    if (pos == (uint) - 1) {
      return false;
    }

    *begin = pos;
    *end = pos;

    while ((*begin > 0) && ( v[*begin - 1].first == key )) {
      *begin = *begin - 1;
    }
    while ((*end < (size - 2)) && ( v[*end + 1].first == key )) {
      *end = *end + 1;
    }

    return true;
  }

  std::vector<uint> find(uint key) {
    uint begin;
    uint end;

    std::vector<uint> values;

    // Find pattern range to evaluate
    if (find_range(key, &begin, &end)) {
      for (uint i = begin; i <= end; i++) {
        values.push_back(v[i].second);
      }
    }

    return values;
  }
};
}
}

#endif // ifndef _H_SAMSON_COMSCORE_MapId2Id