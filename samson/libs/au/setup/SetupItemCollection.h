


#ifndef _H_AU_SETUP_ITEM_COLLECTION
#define _H_AU_SETUP_ITEM_COLLECTION
  
#include <iostream>            // std::cout
#include <map>                 // std::map
#include <stdlib.h>            // atoll
#include <string>              // std::string
#include <sys/stat.h>          // mkdir
  
#include "logMsg/logMsg.h"
  
#include "au/Environment.h"  // au::Environment
#include "au/singleton/Singleton.h"
#include "au/file.h"
#include "au/containers/StringVector.h"
#include "au/containers/map.h"  // au::map
#include "au/setup/SetupItem.h"
  
namespace au {
  
    class SetupItemCollection {
      
    public:
      
      SetupItemCollection(){}
      ~SetupItemCollection();
      
      // Add a new setup parameter to consider
      void AddItem(const std::string& _name
                   , const std::string& _default_value
                   , const std::string& _description );

      void AddUInt64Item(const std::string& _name
                         , const std::string& _default_value
                         , const std::string& _description );
      
      // Load a particular file to include all setup parameters
      bool Load(const std::string& file);
      bool Save(const std::string& file);
      
      // Get access to parameters
      std::string Get(const std::string& name) const;
      bool Set(const std::string& name, const std::string& value);
      std::string GetDefault(const std::string& name) const;  // Access to default value
      size_t GetUInt64(const std::string& name) const;
      int GetInt(const std::string& name) const;
      
      // Check if a particular property if defined
      bool IsParameterDefined(const std::string& name) const;
      
      // Come back to default parameters
      void ResetToDefaultValues();
      
      //Debug string
      std::string str() const;
      
      // Get a vector with all parameter to setup
      std::vector<std::string> GetItemNames() const;
      
    private:
      
      // Collection of setup items
      au::map< std::string, SetupItem > items_;
      
    };

}

#endif