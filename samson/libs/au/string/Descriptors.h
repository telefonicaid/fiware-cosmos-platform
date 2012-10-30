

/* ****************************************************************************
*
* FILE            Descriptors.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*      Descriptors solves the situation where you have a list of strings where some
*      of them are repeated. This helps you to group equal strings to display a unified message
*
* ****************************************************************************/

#ifndef AU_DESCRIPTORS_H_
#define AU_DESCRIPTORS_H_

#include "au/containers/map.h"       // au::map
#include <sstream>
#include <string>

namespace au {
class DescriptorsCounter {
public:

  DescriptorsCounter(const std::string& description);
  void Increase();
  friend std::ostream& operator<<(std::ostream& o, const DescriptorsCounter& descriptors);

private:

  std::string description_;
  int counter_;
};

class Descriptors {
public:

  Descriptors();
  ~Descriptors();

  void Add(const std::string& txt);
  size_t size();
  std::string str();

private:

  friend std::ostream& operator<<(std::ostream& o, const Descriptors& descriptors);
  au::map< std::string, DescriptorsCounter> concepts_;
};
}

#endif  // ifndef AU_DESCRIPTORS_H_
