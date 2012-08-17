



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
 */

#include <string>
#include <set>
#include "logMsg/logMsg.h"
#include "au/Environment.h"

namespace samson
{
  
  // All informaiton required for visualitzation
  
  class Visualization
  {
    
    std::set<std::string> flags_activated_;
    std::set<std::string> flags_not_activated_;
    
    std::string pattern_;

    // Environemnt properties
    au::Environment environment_;
    
  public:
    
    Visualization()
    {
      pattern_ = "*"; // Default pattern
    }
    
    void set_flag( const std::string& name , bool value );
    bool get_flag( const std::string& name ) const;
    
    void set_pattern( const std::string& pattern );
    const std::string& pattern() const;
    
    const au::Environment& environment();
    
  };
  
  // Simplified match 
  bool match( std::string pattern , std::string name );
  
}


#endif