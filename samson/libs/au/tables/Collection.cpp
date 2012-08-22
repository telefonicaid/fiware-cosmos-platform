
#include "Collection.h"  // Own interface

#include <set>

#include "Table.h"

namespace au {
namespace tables {
#pragma mark CollectionItem

CollectionItem::CollectionItem() {
}

CollectionItem::CollectionItem(CollectionItem *collection_item) {
  add(collection_item);
}

void CollectionItem::add(std::string name, std::string value) {
  values.insertInMap(name, value);
}

void CollectionItem::add(CollectionItem *collection_item) {
  au::simple_map<std::string, std::string>::iterator it_values;
  for (it_values = collection_item->values.begin(); it_values != collection_item->values.end(); it_values++) {
    add(it_values->first, it_values->second);
  }
}

std::string CollectionItem::get(std::string name) {
  if (values.isInMap(name)) {
    return values.findInMap(name);
  } else {
    return "";
  }
}

bool CollectionItem::match(std::string name, std::string value) {
  if (get(name) != value) {
    return false;
  }

  return true;
}

bool CollectionItem::match(CollectionItem *filter) {
  au::simple_map<std::string, std::string>::iterator it_values;
  for (it_values = filter->values.begin(); it_values != filter->values.end(); it_values++) {
    std::string name = it_values->first;
    std::string value = it_values->second;

    if (!match(name, value)) {
      return false;
    }
  }
  return true;
}

void CollectionItem::add_prefix(std::string prefix) {
  au::simple_map<std::string, std::string> _values;

  au::simple_map<std::string, std::string>::iterator it_values;
  for (it_values = values.begin(); it_values != values.end(); it_values++) {
    std::string key = prefix + it_values->first;
    _values.insertInMap(key, it_values->second);
  }

  // Insert elements back
  values.clear();
  values.insert(_values.begin(), _values.end());
}

#pragma mark Collection

Collection::Collection(Collection *collection) {
  add(collection);
}

Collection::~Collection() {
  for (size_t i = 0; i < items.size(); i++) {
    delete items[i];
  }
  items.clear();
}

void Collection::add(CollectionItem *item) {
  items.push_back(item);
}

void Collection::add(Collection *collection) {
  for (size_t i = 0; i < collection->items.size(); i++) {
    add(new CollectionItem(collection->items[i]));
  }
}

size_t Collection::getNumItems() {
  return items.size();
}

Table *Collection::getTable() {
  // Combine all fields for all records
  std::set<std::string> _fields;
  for (size_t i = 0; i < items.size(); i++) {
    CollectionItem *collection_item = items[i];
    au::simple_map<std::string, std::string>::iterator it_values;
    for (it_values = collection_item->values.begin(); it_values != collection_item->values.end(); it_values++) {
      _fields.insert(it_values->first);
    }
  }
  StringVector fields(_fields);

  // Create the table
  Table *table = new Table(fields);

  // Add all the rows
  for (size_t i = 0; i < items.size(); i++) {
    StringVector values;

    for (size_t f = 0; f < fields.size(); f++) {
      values.push_back(items[i]->get(fields[f]));
    }

    table->addRow(values);
  }

  return table;
}

Collection *Collection::getCollection(CollectionItem *filter) {
  Collection *collection = new Collection();

  for (size_t i = 0; i < items.size(); i++) {
    CollectionItem *item = items[i];

    if (item->match(filter)) {
      collection->add(new CollectionItem(item));
    }
  }

  return collection;
}

std::string Collection::str(std::string title) {
  Table *table = getTable();

  table->setTitle(title);
  std::string output = table->str();
  delete table;

  return output;
}

void Collection::remove(std::string name, std::string value) {
  std::vector<CollectionItem *> _items;

  for (size_t i = 0; i < items.size(); i++) {
    CollectionItem *item = items[i];

    if (item->match(name, value)) {
      delete item;
    } else {
      _items.push_back(item);
    }
  }

  items.clear();
  for (size_t i = 0; i < _items.size(); i++) {
    items.push_back(_items[i]);
  }
}
}
}
