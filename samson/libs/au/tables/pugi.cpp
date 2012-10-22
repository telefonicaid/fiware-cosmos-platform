


/* ****************************************************************************
*
* FILE            pugi.cpp
*
* AUTHOR          Andreu Urruela
*
* PROJECT         pugi
*
* DATE            7/15/11
*
* DESCRIPTION
*
*  Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#include "au/string/StringUtilities.h"      // au::tabs
#include "au/tables/Tree.h"    // au::tables::TreeItem
#include "pugi.h"                       // Own interface

namespace pugi {
au::tables::TreeItem *treeItemFromDocument(const xml_document& xml_doc) {
  return treeItemFromNode(xml_doc.document_element());
}

au::tables::TreeItem *treeItemFromNode(const xml_node& xml_node) {
  std::string name = xml_node.name();

  if (name.length() == 0) {
    name = xml_node.value();
  }
  au::tables::TreeItem *treeItem = new au::tables::TreeItem(name);

  for (pugi::xml_node_iterator n = xml_node.begin(); n != xml_node.end(); n++) {
    pugi::xml_node node = *n;
    treeItem->add(treeItemFromNode(node));
  }

  return treeItem;
}
}
