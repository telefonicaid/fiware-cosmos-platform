/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef MODULE_CONTAINER_H
#define MODULE_CONTAINER_H

/* ****************************************************************************
 *
 * FILE                     ModuleContainer.h
 *
 * DESCRIPTION				Information about a module (module definition)
 *
 */

#include <cstdio>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>             /* strtok, strcpy, ...                     */
#include <string>
#include <string>
#include <unistd.h>             /* _exit, ...                              */
#include <vector>

#include "DataType.h"
#include <samson/module/KVFormat.h>

namespace samson {
class DataCreator;
class AUTockenizer;

class ModuleContainer {
public:

  std::string name;                  // Short name for this module
  std::string title;                 // Long title for better description

  std::string author;
  std::string version;

  std::vector <std::string> includes;
  std::vector <std::string> help;

  std::map<std::string, std::string> sets;

  // Constructor
  ModuleContainer();

  /** Get a unique name for this module for the _H_ define */

  std::string getDefineUniqueName();

  std::string getClassName();

  std::string getFullClassName();

  std::vector<std::string> tockenizeWithDots(std::string myString);

  void parse(AUTockenizer *module_creator, int begin, int end);
};
}

#endif  // ifndef MODULE_CONTAINER_H
