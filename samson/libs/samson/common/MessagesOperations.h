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
#ifndef _H_MESSAGES_OPERATIONS
#define _H_MESSAGES_OPERATIONS

#include <fnmatch.h>
#include <map>
#include <string>

#include "au/containers/map.h"
#include "au/containers/SharedPointer.h"

#include "samson/common/samson.pb.h"    // network::
#include "samson/common/Visualitzation.h"
#include "samson/module/KVFormat.h"     // KVFormat
namespace samson {

template<typename C>
void add(samson::gpb::CollectionRecord *record, std::string name, C _value, std::string format) {
  std::ostringstream value;

  value << _value;

  samson::gpb::CollectionItem *item = record->add_item();
  item->set_name(name);
  item->set_value(value.str());
  item->set_format(format);
}

template<typename C>
void add(samson::gpb::CollectionRecord *record, std::string name, C _value) {
  std::ostringstream value;

  value << _value;

  samson::gpb::CollectionItem *item = record->add_item();
  item->set_name(name);
  item->set_value(value.str());
}

template<typename C>
bool name_match(const char *pattern, C _value) {
  std::ostringstream value;

  value << _value;

  return (::fnmatch(pattern, value.str().c_str(), 0) == 0);
}

// Get a collection from a map
template<typename K, typename V>
au::SharedPointer<gpb::Collection> GetCollectionForMap(const std::string& title, std::map<K, V *>& m,
                                                       const Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name(title);

  typename std::map<K, V *>::iterator iter;

  for (iter = m.begin(); iter != m.end(); ++iter) {
    // Get pointer to the instance
    V *v = iter->second;

    // Create a new record for this instance
    gpb::CollectionRecord *record = collection->add_record();

    // Common type to joint queries ls_workers -group type
    v->fill(record, visualization);
  }

  return collection;
}
}

#endif  // ifndef _H_MESSAGES_OPERATIONS
