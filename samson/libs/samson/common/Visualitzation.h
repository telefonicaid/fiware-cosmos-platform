
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
#include "logMsg/logMsg.h"

namespace samson {

class Visualization {

  public:
  
    Visualization() {
      pattern_ = "*";   // Default pattern
    }

  // Check a string match provided pattern
    bool match(const std::string& value) const;

  // Set and Get flags
    void set_flag(const std::string& name, bool value);
    bool get_flag(const std::string& name) const;

  // Pattern operations
    void set_pattern(const std::string& pattern);
    const std::string& pattern() const;

  // Environment properties
    const au::Environment& environment();
  
private:
  
  std::set<std::string> flags_activated_;
  std::set<std::string> flags_not_activated_;
  std::string pattern_;
  au::Environment environment_;
  
};

}

#endif  // ifndef SAMSON_VISUALITZATION_H
