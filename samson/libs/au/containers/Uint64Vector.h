#ifndef _H_AU_STRING_UINT64_VECTOR
#define _H_AU_STRING_UINT64_VECTOR

#include "logMsg/logMsg.h"     // LM_W
#include <sstream>             // std::ostringstream
#include <stdio.h>             /* sprintf */
#include <string>              // std::string
#include <sys/time.h>          // struct timeval

#include "au/CommandLine.h"
#include "au/ErrorManager.h"   // au::ErrorManager
#include "au/console/ConsoleAutoComplete.h"
#include "au/containers/map.h"  // au::map
#include "au/containers/simple_map.h"
#include "au/string.h"         // au::str(...)




namespace au {
class Uint64Vector;
class Uint64Set;

class Uint64Vector : public std::vector<size_t>{
public:

  Uint64Vector();
  Uint64Vector(std::vector<size_t> v);

  size_t getNumberOfTimes(size_t t);
  std::set<size_t> getSet();
  void copyFrom(std::vector<size_t> &v);
  std::string str();
};

class Uint64Set : public std::set<size_t>{
  typedef std::set<size_t>::iterator   iterator;

public:

  bool contains(size_t num) const {
    return ( find(num) != end());
  }

  Uint64Set intersection(const Uint64Set& set) {
    Uint64Set intersection_set;

    for (iterator i = begin(); i != end(); i++) {
      if (set.contains(*i)) {
        intersection_set.insert(*i);
      }
    }
    return intersection_set;
  }

  Uint64Set non_intersection(const Uint64Set& set) {
    Uint64Set intersection_set;

    for (iterator i = begin(); i != end(); i++) {
      if (!set.contains(*i)) {
        intersection_set.insert(*i);
      }
    }
    return intersection_set;
  }

  Uint64Vector getVector() {
    Uint64Vector v;

    for (iterator i = begin(); i != end(); i++) {
      v.push_back(*i);
    }
    return v;
  }

  void remove_non_included_in(const Uint64Set& set) {
    for (iterator i = begin(); i != end(); i++) {
      if (!set.contains(*i)) {
        erase(i);
      }
    }
  }

  std::string str() {
    std::ostringstream output;

    for (iterator i = begin(); i != end(); i++) {
      output << *i << " ";
    }
    return output.str();
  }
};
}

#endif  // ifndef _H_AU_STRING_UINT64_VECTOR