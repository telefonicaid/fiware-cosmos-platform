
#include "au/setup/SetupItemCollection.h" // Own interface

#include "au/CommandLine.h"
#include "au/tables/Table.h"

namespace au {

  SetupItemCollection::~SetupItemCollection() {
    items_.clearMap();
  }
  
  void SetupItemCollection::AddItem(const std::string& name
                                    , const std::string& default_value
                                    , const std::string& description ){
    if (items_.findInMap(name) != NULL) {
      LM_W(("Item %s already added to setup... ignoring", name.c_str()));
      return;
    }
    
    items_.insertInMap(name, new SetupItem(name, default_value, description, SetupItem_string));
  }

  void SetupItemCollection::AddUInt64Item(const std::string& name
                                    , const std::string& default_value
                                    , const std::string& description ){
    if (items_.findInMap(name) != NULL) {
      LM_W(("Item %s already added to setup... ignoring", name.c_str()));
      return;
    }
    
    items_.insertInMap(name, new SetupItem(name, default_value, description, SetupItem_uint64));
  }
  
  
  bool SetupItemCollection::Save( const std::string& fileName )
  {
    
    FILE *file = fopen(fileName.c_str(), "w");
    
    if (!file) {
      LM_W(("Impossible to open setup file %s", fileName.c_str()));
      return false;
    }
    
    fprintf(file, "# ----------------------------------------------------------------------------------------\n");
    fprintf(file, "# SAMSON SETUP\n");
    fprintf(file, "# ----------------------------------------------------------------------------------------\n");
    fprintf(file, "# File auto-generated using the samsonConfig tool.\n");
    fprintf(file, "# Edit manually, or use samsonConfig to regenerate.\n\n\n");
    
    
    
    au::map< std::string, SetupItem >::iterator i;
    
    // First concept
    std::string concept = "Z";
    
    for (i = items_.begin(); i != items_.end(); i++) {
      std::string tmp_concept = i->second->category();
      
      if (tmp_concept != concept) {
        fprintf(file, "# ------------------------------------------------------------------------ \n");
        fprintf(file, "# SECTION: %s\n", tmp_concept.c_str());
        fprintf(file, "# ------------------------------------------------------------------------ \n\n");
        concept = tmp_concept;
      }
      
      fprintf(file, "%-40s\t%-20s # %s\n"
              , i->first.c_str()
              , i->second->value().c_str()
              , i->second->description().c_str());
    }
    
    fclose(file);
    
    return true;
  }
  
  bool SetupItemCollection::Load(const std::string& fileName) {
    
    FILE *file = fopen(fileName.c_str(), "r");
    
    if (!file) {
      LM_W(("Warning: Setup file %s not found\n", fileName.c_str()));
      return false;
    }
    
    char line[2000];
    while (fgets(line, sizeof(line), file)) {
      au::CommandLine c;
      c.Parse(line);
      
      if (c.get_num_arguments() == 0) {
        continue;    // Skip comments
      }
      std::string mainCommand = c.get_argument(0);
      if (mainCommand[0] == '#') {
        continue;
      }
      if (c.get_num_arguments() >= 2) {
        std::string name = c.get_argument(0);
        std::string value =  c.get_argument(1);
        
        SetupItem *item = items_.findInMap(name);
        
        if (item) {
          item->set_value(value);
        } else {
          LM_W(("Unknown parameter %s found in setup file %s", name.c_str(), fileName.c_str()));
        }
      }
    }
    
    fclose(file);
    return  true;
  }
  
  std::string SetupItemCollection::Get(const std::string& name) const {
    SetupItem *item = items_.findInMap(name);
    if (!item) {
      LM_E(("Parameter %s not defined in the setup. This is not acceptable", name.c_str()));
      return "";
    }
    return item->value();
  }
  
  std::string SetupItemCollection::GetDefault(const std::string& name) const {
    SetupItem *item = items_.findInMap(name);
    if (!item) {
      LM_E(("Parameter %s not defined in the setup. This is not acceptable", name.c_str()));
      return "Error";
    }
    return item->default_value();
  }
  
  size_t SetupItemCollection::GetUInt64(const std::string& name) const {
    std::string value = Get(name);
    return atoll(value.c_str());
  }
  
  int SetupItemCollection::GetInt(const std::string& name) const {
    std::string value = Get(name);
    return atoi(value.c_str());
  }
  
  bool SetupItemCollection::IsParameterDefined(const std::string& name) const {
    return ( items_.findInMap(name) != NULL);
  }
  
  bool SetupItemCollection::Set(const std::string& name, const std::string& value) {
    SetupItem *item = items_.findInMap(name);
    if (!item) {
      LM_W(("Parameter %s not defined in the setup. This is not acceptable", name.c_str()));
      return false;
    }
    return item->set_value(value);
  }
  
  void SetupItemCollection::ResetToDefaultValues() {
    au::map< std::string, SetupItem >::iterator it_items;
    for (it_items = items_.begin(); it_items != items_.end(); it_items++) {
      it_items->second->ResetToDefaultValue();
    }
  }
  
  std::string SetupItemCollection::str() const {
    au::tables::Table table("Parameter,left|Default value|Current value,left|Description,left");
    
    table.setTitle("Samson setup parameters");
    
    std::ostringstream output;
    au::map< std::string, SetupItem >::const_iterator i;
    for (i = items_.begin(); i != items_.end(); i++) {
      au::StringVector values;
      values.push_back(i->first);
      values.push_back(i->second->default_value());
      values.push_back(i->second->value());
      values.push_back(i->second->description());
      
      table.addRow(values);
    }
    return table.str();
  }
  
  std::vector<std::string> SetupItemCollection::GetItemNames() const {
    return items_.getKeysVector();
  }
  
  
#pragma mark SamsonSetup
}