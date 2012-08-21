
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
  void clearEnvironment();

  // Get methods
  std::string get(const std::string& name, const std::string& default_value) const;
  int get(const std::string& name, int defaultValue) const;
  size_t get(const std::string& name, size_t defaultValue) const;
  double get(const std::string& name, double value) const;

  // Set methods
  void set(const std::string&, const std::string& value);
  void unset(const std::string& name);
  bool isSet(const std::string& name) const;

  // Templarized assignation
  template<typename T>
  void set(std::string name, T value) {
    std::ostringstream v;

    v << value;
    set(name, v.str());
  }

  // Asignation operator
  Environment& operator=(Environment& environment);

  // Save and restore from string
  std::string saveToString();
  void recoverFromString(const std::string& input);

private:

  friend std::ostream& operator<<(std::ostream& o, Environment& enviroment);
  std::map<std::string, std::string> items_;
};
}

#endif  // ifndef AU_ENVIRONMENT_H_
