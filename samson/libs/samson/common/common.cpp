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


#include "common.h"  // Own interface

#include "samson/module/samson.h"

namespace samson {
std::string str_block_id(size_t block_id) {
  BlockId id;

  id.uint64 = block_id;
  return au::str("B_%d_%d", id.uint32[0], id.uint32[1]);
}

au::SharedPointer<au::tables::Table> GetTableFromCollection(au::SharedPointer<gpb::Collection> collection) {
  std::string table_definition;


  if (collection->record_size() == 0) {
    return au::SharedPointer<au::tables::Table>(NULL);
  }

  for (int i = 0; i < collection->record(0).item_size(); ++i) {
    std::string name = collection->record(0).item(i).name();
    std::string format = collection->record(0).item(i).format();

    table_definition.append(name);
    if (format.length() != 0) {
      table_definition.append(",");
      table_definition.append(format);
    }

    if (i != (collection->record(0).item_size() - 1)) {
      table_definition.append("|");
    }
  }
  au::SharedPointer<au::tables::Table> table(new au::tables::Table(table_definition));

  std::string title = collection->title();
  table->setTitle(title);

  for (int r = 0; r < collection->record_size(); ++r) {
    au::StringVector values;
    for (int i = 0; i < collection->record(r).item_size(); i++) {
      values.push_back(collection->record(r).item(i).value());
    }
    table->addRow(values);
  }

  return table;
}
}