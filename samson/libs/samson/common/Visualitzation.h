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

#ifndef SAMSON_VISUALITZATION_H
#define SAMSON_VISUALITZATION_H

/* ****************************************************************************
 *
 * FILE                     Visualitzation.h
 *
 * DESCRIPTION              Information about what to visualize
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            02/2012
 *
 * Information required for visualitzation of tables
 */


#include <set>
#include <string>

#include "au/Environment.h"
#include "au/string/Pattern.h"
#include "logMsg/logMsg.h"

namespace samson {

class Visualization {

  public:
  
    Visualization() {
    }

  // Check a string match provided pattern
    bool match(const std::string& value) const;

  // Set and Get flags
    void set_flag(const std::string& name, bool value);
    bool get_flag(const std::string& name) const;

  // Pattern operations
    void set_pattern(const std::string& pattern);
    std::string pattern() const;

  // Environment properties
    const au::Environment& environment();
  
private:
  
  std::set<std::string> flags_activated_;
  std::set<std::string> flags_not_activated_;
  au::SimplePattern pattern_;
  au::Environment environment_;
  
};

}

#endif  // ifndef SAMSON_VISUALITZATION_H
