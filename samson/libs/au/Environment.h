
/* ****************************************************************************
*
* FILE            Enviroment.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*  Collection of enviroment variables. It is a key-value colection usign strings
*  Convenient functions are provided to use values as integers, doubles, etc...
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef AU_ENVIRONMENT_H_
#define AU_ENVIRONMENT_H_

#include <map>             // std::map
#include <math.h>
#include <sstream>         // std::ostringstream
#include <stdlib.h>

#include "au/S.h"

namespace au {
class Environment {
public:

  Environment() {
  }

  ~Environment() {
  }

  // Add all elements from another Environment
  void Add(const Environment& environment);

  // Clear environment previously defined
  void ClearEnvironment();

  // Get methods
  std::string Get(const std::string& name, const std::string& default_value) const;
  int Get(const std::string& name, int defaultValue) const;
  size_t Get(const std::string& name, size_t defaultValue) const;
  double Get(const std::string& name, double value) const;

  // Set methods
  void Set(const std::string&, const std::string& value);
  void Unset(const std::string& name);
  bool IsSet(const std::string& name) const;

  // Templarized assignation
  template<typename T>
  void Set(std::string name, T value) {
    std::ostringstream v;

    v << value;
    Set(name, v.str());
  }

  // Asignation operator
  Environment& operator=(Environment& environment);

  // Save and restore from string
  std::string SaveToString() const;
  void RecoverFromString(const std::string& input);

  // Debug string
  std::string str() const;

private:

  friend std::ostream& operator<<(std::ostream& o, const Environment& enviroment);
  std::map<std::string, std::string> items_;
};
}

#endif  // ifndef AU_ENVIRONMENT_H_
