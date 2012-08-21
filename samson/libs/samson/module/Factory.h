#ifndef SAMSON_FACTORY_H
#define SAMSON_FACTORY_H

/**
 * Factory pattern implementation
 *
 * A factory is an element able to generate new opbjects in a dinamic way just indicating
 * the "name" of the element as an string.
 * Previously, all possible elements to be generated have to be added to the factory object.
 */

#pragma once


#include <cstdio>
#include <map>
#include <sstream>
#include <string>

namespace au {
/* Function to create new elements */
typedef void *(*factoryFunction)();

/* Map to accumulate pairs of "name" and "factory Fucntions" */
typedef std::map <std::string, factoryFunction>   mapFactory;

// Template of the factory function
template <class T>
void *factory() {
  return new T();
}

/**
 * Element that accumulate a set of factory
 * functions organized by a string
 */

class Factory {
  // Map of creation function

public:
  std::map <std::string, factoryFunction> creationFunctions;

  /*
   * Add elements to be able to create instances of them latter
   * The first parameter is the name of the element (it is not allowed to duplicate name)
   */

  void add(std::string name, factoryFunction function) {
    mapFactory::iterator iter =  creationFunctions.find(name);

    if (iter == creationFunctions.end())
      // There was no previous elements defined with name
      creationFunctions.insert(std::pair<std::string, factoryFunction>(name, function));
    else
      fprintf(stderr, "Warning: Previous definition of an element with the same name %s\n", name.c_str());

  }

  /* Create a new object of this type */
  void *create(std::string name) {
    mapFactory::iterator iter =  creationFunctions.find(name);

    if (iter != creationFunctions.end()) {
      factoryFunction function = iter->second;
      return function();
    } else {
      return NULL;
    }
  }

  std::string list(std::string name_of_elements) {
    std::ostringstream o;

    o << "List of " << name_of_elements << ": ";
    for (mapFactory::iterator iter = creationFunctions.begin(); iter != creationFunctions.end(); iter++) {
      o << iter->first << " ";
    }
    return o.str();
  }

  bool check(std::string name) {
    mapFactory::iterator iter =  creationFunctions.find(name);

    if (iter != creationFunctions.end()) {
      return true;
    } else {
      return false;
    }
  }
};


/**
 * A collection of factory elements
 * Useful to manage heterogenius types of elements in a uniform way
 * Example: MACRO platform: managin parsers, parsersOut, etc...
 */

class FactoryCollection {
  std::map< std::string, Factory > factories;

public:

  void add(std::string type, std::string name, factoryFunction function) {
    // Create the factory if necessary
    if (factories.find(type) == factories.end()) {
      // Create the factory
      Factory tmp;
      factories.insert(std::pair < std::string, Factory >(type, tmp));
    }

    // Get the factory
    std::map< std::string, Factory >::iterator iter = factories.find(type);
    if (iter == factories.end()) {
      printf("Error adding elements inside the factory (FactoryCollection)\n");
      exit(1);
    }

    // Add the element to be generated
    iter->second.add(name, function);
  }

  void *create(std::string type, std::string name) {
    // Get the factory
    std::map< std::string, Factory >::iterator iter = factories.find(type);

    if (iter == factories.end()) {
      return NULL;
    }

    return iter->second.create(name);
  }

  Factory *getFactory(std::string type) {
    std::map< std::string, Factory >::iterator iter = factories.find(type);

    if (iter == factories.end()) {
      return NULL;
    } else {
      return &iter->second;
    }
  }

  std::string listFactory(std::string type) {
    Factory *factory = getFactory(type);

    if (factory) {
      return factory->list("type");
    } else {
      return "No elements of type: " + type;
    }
  }

  bool check(std::string type, std::string name) {
    Factory *factory = getFactory(type);

    if (!factory) {
      return false;
    }

    return factory->check(name);
  }

  void copyElementsFrom(FactoryCollection *other) {
    // Copy all the elements form another factory collection
    std::map< std::string, Factory >::iterator iter_factories;

    for (iter_factories = other->factories.begin(); iter_factories != other->factories.end(); iter_factories++) {
      std::string factory_name = iter_factories->first;
      Factory f = iter_factories->second;

      for (mapFactory::iterator iter = f.creationFunctions.begin(); iter != f.creationFunctions.end(); iter++) {
        std::string name = iter->first;
        factoryFunction function  = iter->second;

        add(factory_name, name, function);

        // o << iter->first;
        // return o.str();
      }
    }
  }
};
}

#endif // ifndef SAMSON_FACTORY_H
