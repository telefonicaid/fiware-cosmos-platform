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

#include "xml.h"        // Own interface

namespace au {
void xml_open(std::ostringstream& output, std::string name) {
  output << "<" << name << ">";
}

void xml_close(std::ostringstream& output, std::string name) {
  output << "</" << name << ">";
}

void xml_content(std::ostringstream& output, std::string _content) {
  output << "<![CDATA[" << _content << "]]>";
}
}