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

#include "samson/common/EnvironmentOperations.h"                        // Own definition of this methods

#include <iomanip>
#include <iostream>


namespace samson {
void copyEnviroment(Environment *from, gpb::Environment *to) {
  std::map<std::string, std::string>::iterator iter;
  for (iter = from->environment.begin(); iter != from->environment.end(); ++iter) {
    gpb::EnvironmentVariable *ev = to->add_variable();

    ev->set_name(iter->first);
    ev->set_value(iter->second);
  }
}

void copyEnviroment(const gpb::Environment & from, Environment *to) {
  for (int i = 0; i < from.variable_size(); i++) {
    std::string name = from.variable(i).name();
    std::string value = from.variable(i).value();
    to->set(name, value);
  }
}
}