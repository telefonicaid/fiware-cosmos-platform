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

#include "au/setup/SetupItem.h" // Own interface


#include "au/setup/SetupItemCollection.h" // Own interface

#include "au/CommandLine.h"
#include "au/tables/Table.h"

namespace au {
  
  SetupItem::SetupItem(const std::string& name, const std::string& default_value, const std::string& description, SamsonAdaptorType type) :
  name_(name)
  ,default_value_(default_value)
  ,value_(default_value)
  ,description_(description)
  ,type_(type)
  {
    
    if (!CheckValidValue(default_value)) {
      LM_W(("Default value %s not valid for setup item %s", default_value_.c_str(), name_.c_str()));
    }
  }

  
  bool SetupItem::set_value(const std::string& value) {
    if (!CheckValidValue(value)) {
      return false;
    }
    value_ = value;
    return true;
  }
  
  bool SetupItem::CheckValidValue(const std::string& value) const {
    
    if (type_ == SetupItem_string) {
      return true;    // Everything is valid
    }
    
    if (type_ == SetupItem_uint64) {
      size_t p  = value.find_first_not_of("0123456789");
      return (p == std::string::npos);
    }
    
    // Unknown type
    return false;
  }
  
  std::string SetupItem::value() const {
    return value_;
  }
  
  std::string SetupItem::name() const {
    return name_;
  }
  
  std::string SetupItem::default_value() const {
    return default_value_;
  }
  
  std::string SetupItem::description() const {
    return description_;
  }
  
  std::string SetupItem::category() const {
    return name_.substr(0, name_.find(".", 0));
  }
  
  void SetupItem::ResetToDefaultValue() {
    value_ = default_value_;
  }
  
}