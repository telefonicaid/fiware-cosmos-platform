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

/*
 * FILE            Tree
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            August 2012
 *
 * DESCRIPTION
 *
 *  Tree manipulation methods
 */

#include "Tree.h"    // Own interface

#include <assert.h>
#include <string>       // std::string

#include "Collection.h"
#include "Select.h"
#include "Table.h"

#include "au/tables/pugi.h"
#include "au/string/xml.h"


namespace au {
namespace tables {
#pragma mark TreeCollection

TreeCollection::~TreeCollection() {
  for (size_t i = 0; i < trees.size(); i++) {
    delete trees[i];
  }
  trees.clear();
}

void TreeCollection::add(TreeItem *tree) {
  trees.push_back(tree);
}

void TreeCollection::add(TreeCollection *tree_collection) {
  for (size_t i = 0; i < tree_collection->trees.size(); i++) {
    add(tree_collection->trees[i]);
  }

  // Remove from the original list
  tree_collection->trees.clear();
}

size_t TreeCollection::getNumTrees() {
  return trees.size();
}

TreeItem *TreeCollection::getTree(size_t pos) {
  if (pos >= trees.size()) {
    return NULL;
  }

  return new TreeItem(trees[pos]);
}

void TreeCollection::replaceToAll(std::string name, std::string value) {
  for (size_t i = 0; i < trees.size(); i++) {
    trees[i]->replace(name, value);
  }
}

std::string TreeCollection::str(size_t limit) {
  if (trees.size() == 0) {
    return au::str("No trees in this collection");
  } else {
    std::ostringstream output;
    for (size_t i = 0; i < trees.size(); i++) {
      output << "-----------------------------------------------------\n";
      output << au::str(" Tree %lu / %lu \n", i + 1, trees.size());
      output << "-----------------------------------------------------\n";
      output << trees[i]->str(limit);
    }
    return output.str();
  }
}

void TreeCollection::addSpetialTags(TreeItem *tree_item) {
  std::string tag_name = tree_item->getFirstNodeValue("tag_name");
  std::string tag_value = tree_item->getFirstNodeValue("tag_value");

  if (tag_name != "") {
    replaceToAll(tag_name, tag_value);
  }
}

#pragma mark TreeItem

TreeItem::TreeItem(std::string _value) {
  value = _value;
}

TreeItem::TreeItem(TreeItem *tree) {
  value = tree->value;
  for (size_t i = 0; i < tree->items.size(); i++) {
    add(new TreeItem(tree->items[i]));
  }
}

TreeItem::~TreeItem() {
  for (size_t i = 0; i < items.size(); i++) {
    delete items[i];
  }
  items.clear();
}

std::string TreeItem::getValue() {
  return value;
}

std::string TreeItem::getFirstNodeValue() {
  if (items.size() == 0) {
    return "";
  }
  return items[0]->getValue();
}

std::string TreeItem::getFirstNodeValue(std::string node_name) {
  for (size_t i = 0; i < items.size(); i++) {
    if (items[i]->value == node_name) {
      return items[i]->getFirstNodeValue();
    }
  }

  return "";
}

TreeItem *TreeItem::add(TreeItem *item) {
  items.push_back(item);
  return item;
}

TreeItem *TreeItem::add(std::string value) {
  TreeItem *item = new TreeItem(value);

  add(item);
  return item;
}

TreeItem *TreeItem::add(std::string property, std::string value) {
  return add(property)->add(value);
}

bool TreeItem::isValue(std::string _value) {
  return ( value == _value );
}

void TreeItem::replace(std::string property, std::string value) {
  for (size_t i = 0; i < items.size(); i++) {
    if (items[i]->value == property) {
      if (items[i]->items.size() > 0) {
        items[i]->items[0]->value = value;
      } else {
        items[i]->add(value);
      } return;
    }
  }

  add(property, value);
}

size_t TreeItem::getNumItems() {
  return items.size();
}

size_t TreeItem::getNumItems(std::string name) {
  size_t total = 0;

  for (size_t i = 0; i < items.size(); i++) {
    if (items[i]->value == name) {
      total++;
    }
  }

  return total;
}

void TreeItem::str(std::ostringstream& output, int level, int max_level) {
  if (max_level > 0) {
    if (level >= max_level) {
      return;
    }
  }

  for (int i = 0; i < level; i++) {
    output << " ";
  }
  output << "* " << value << "\n";

  for (size_t i = 0; i < items.size(); i++) {
    items[i]->str(output, level + 1, max_level);
  }
}

std::string TreeItem::str(int max_level) {
  std::ostringstream output;

  str(output, 0, max_level);
  return output.str();
}

TreeCollection *TreeItem::selectTrees(SelectTreeInformation *select) {
  TreeCollection *tree_collection = new TreeCollection();

  for (size_t i = 0; i < items.size(); i++) {
    if (compare_strings(select->name, items[i]->value)) {
      if (select->check(items[i])) {
        tree_collection->add(new TreeItem(items[i]));
      }
    } else if (select->recursevely) {
      TreeCollection *_tree_collection = items[i]->selectTrees(select);
      tree_collection->add(_tree_collection);
      delete _tree_collection;
    }
  }

  tree_collection->addSpetialTags(this);
  return tree_collection;
}

TreeCollection *TreeItem::getTreesFromPath(std::string path) {
  // Create the result tree to add all nodes
  TreeCollection *tree_collection = new TreeCollection();
  // Expected format "//%s[%s]" or "/%s[%s]"

  // Find "/" in the provided path ( to see if we are final or not )
  size_t pos = path.find('/', 2);

  if (pos == std::string::npos) {
    // We are the last, so just look for the nodes
    SelectTreeInformation select(path);

    // Add this nodes directly
    TreeCollection *_tree_collection = selectTrees(&select);
    tree_collection->add(_tree_collection);
    delete _tree_collection;
  } else {
    // Next field to search...
    std::string node_description = path.substr(0, pos);
    std::string next_path = path.substr(pos, path.length() - pos);

    // Get the nodes that meet criteria
    SelectTreeInformation select(node_description);
    TreeCollection *_tree_collection = selectTrees(&select);

    for (size_t i = 0; i < _tree_collection->trees.size(); i++) {
      TreeCollection *__trees_collection = _tree_collection->trees[i]->getTreesFromPath(next_path);
      tree_collection->add(__trees_collection);
      delete __trees_collection;
    }
    delete _tree_collection;
  }

  tree_collection->addSpetialTags(this);
  return tree_collection;
}

TreeItem *TreeItem::getFirstTreeFromPath(std::string path) {
  TreeCollection *collection = getTreesFromPath(path);
  TreeItem *tree = collection->getTree(0);

  delete collection;
  return tree;
}

CollectionItem *TreeItem::getCollectionItem() {
  CollectionItem *collection_item = new CollectionItem();

  for (size_t i = 0; i < items.size(); i++) {
    if (getNumItems(items[i]->value) == 1) {
      if (items[i]->isFinalProperty()) {
        std::string property_key = items[i]->getValue();
        std::string property_value = items[i]->getFirstNodeValue();

        collection_item->add(property_key, property_value);
      } else {
        CollectionItem *_collection_item = items[i]->getCollectionItem();

        _collection_item->add_prefix(items[i]->getValue() + "/");
        collection_item->add(_collection_item);
        delete _collection_item;
      }
    }
  }

  return collection_item;
}

Table *TreeItem::getTable() {
  Table *table = new Table(StringVector("key", "value"));

  au::simple_map<std::string, std::string> properties;

  for (size_t i = 0; i < items.size(); i++) {
    if (items[i]->isFinalProperty()) {
      std::string property_key = items[i]->getValue();
      std::string property_value = items[i]->getFirstNodeValue();

      properties.removeInMap(property_key);
      properties.insertInMap(property_key, property_value);
    } else {
      Table *_table = items[i]->getTable();
      for (size_t r = 0; r < _table->getNumRows(); r++) {
        std::string property_key = items[i]->getValue() + "/" + _table->getValue(r, 0);
        std::string property_value = _table->getValue(r, 1);

        properties.removeInMap(property_key);
        properties.insertInMap(property_key, property_value);
      }
      delete _table;
    }
  }

  au::simple_map<std::string, std::string>::iterator it_properties;
  for (it_properties = properties.begin(); it_properties != properties.end(); it_properties++) {
    table->addRow(StringVector(it_properties->first, it_properties->second));
  }

  return table;
}

std::string getValueFromTable(Table *table, std::string concept) {
  for (size_t r = 0; r < table->getNumRows(); r++) {
    if (table->getValue(r, 0) == concept) {
      return table->getValue(r, 1);
    }
  }
  return "";
}

Collection *TreeItem::getCollectionFromPath(std::string path) {
  TreeCollection *tree_collection = getTreesFromPath(path);
  Collection *collection = new Collection();

  for (size_t i = 0; i < tree_collection->trees.size(); i++) {
    collection->add(tree_collection->trees[i]->getCollectionItem());
  }

  delete tree_collection;
  return collection;
}

Table *TreeItem::getTableFromPath(std::string path) {
  Collection *collection = getCollectionFromPath(path);
  Table *table = collection->getTable();

  delete collection;
  return table;
}

bool TreeItem::hasNode(std::string node_name) {
  for (size_t i = 0; i < items.size(); i++) {
    if (items[i]->value == node_name) {
      return true;
    }
  }
  return false;
}

bool TreeItem::isFinalProperty() {
  if (items.size() == 1) {
    if (items[0]->items.size() == 0) {
      return true;
    }
  }
  return false;
}

size_t TreeItem::getTotalNumNodes() {
  size_t total = 1;   // Myself

  for (size_t i = 0; i < items.size(); i++) {
    total += items[i]->getTotalNumNodes();
  }
  return total;
}

size_t TreeItem::getMaxDepth() {
  size_t max_depth = 0;   // Myself

  for (size_t i = 0; i < items.size(); i++) {
    size_t tmp_max_depth = items[i]->getMaxDepth();
    if (tmp_max_depth > max_depth) {
      max_depth = tmp_max_depth;
    }
  }
  return max_depth + 1;   // Myself
}

void TreeItem::getNodesFrom(TreeItem *tree) {
  for (size_t i = 0; i < tree->items.size(); i++) {
    items.push_back(tree->items[i]);
  }
  tree->items.clear();
}

TreeItem *TreeItem::getTreeFromXML(std::string &data) {
  std::ostringstream output;

  output << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
  au::xml_open(output, "root");
  output << data;
  au::xml_close(output, "root");

  // document to parse xml
  pugi::xml_document doc;

  // Parser the xml document into "doc"
  std::istringstream is_xml_document(output.str());

  // pugi::xml_parse_result result = doc.load(is_xml_document);
  doc.load(is_xml_document);

  // Create a tree based on the document we have just scanned
  return pugi::treeItemFromDocument(doc);
}

void TreeItem::replaceNode(TreeItem *item) {
  for (size_t i = 0; i < items.size(); i++) {
    if (items[i]->value == item->value) {
      delete items[i];
      items[i] = item;
      return;
    }
  }

  // if if was not present, just insert in the list of nodes
  items.push_back(item);
}
}
}
