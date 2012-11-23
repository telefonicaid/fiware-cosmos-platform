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
#ifndef _H_DELILAH_TRACE_COLLECTION
#define _H_DELILAH_TRACE_COLLECTION

#include <cstdlib>                  // atexit

#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <sstream>                  // std::ostringstream
#include <stdio.h>
#include <time.h>

#include "logMsg/logMsg.h"

#include "au/console/Console.h"     // au::console::Console
#include "au/console/ConsoleAutoComplete.h"
#include "au/mutex/TokenTaker.h"    // au::TokenTake

#include "au/tables/Select.h"
#include "au/tables/Table.h"


#include "samson/delilah/Delilah.h"  // samson::Delilah

namespace samson {
// Class used to store traces received from SAMSON

class Alert {
public:

  NodeIdentifier node;

  std::string type;
  std::string context;
  std::string text;

  Alert(NodeIdentifier _node, std::string _type, std::string _context, std::string _text) {
    node = _node;
    type = _type;
    context = _context;
    text = _text;
  }
};

// class used to store some traces received from SAMSON

class AlertCollection {
  au::list<Alert> traces;
  size_t max_num_elements;

public:

  // Constructor
  AlertCollection();

  // Add traces to this collection ( limited number will be stored in memory )
  void add(NodeIdentifier node, std::string type, std::string context, std::string text);

  // Table with traces to be displayed on screen with show_traces command
  std::string str();
};
}

#endif  // ifndef _H_DELILAH_TRACE_COLLECTION
