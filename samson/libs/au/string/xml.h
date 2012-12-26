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
/* ****************************************************************************
*
* FILE            xml.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            September 2011
*
* DESCRIPTION
*
*      useful functions to generate xml tags
*
* ****************************************************************************/

#ifndef _H_AU_XML
#define _H_AU_XML

#include <sstream>
#include <string>

#include "au/containers/map.h"

namespace au {
// Simple xml tags
void xml_open(std::ostringstream& output, std::string name);
void xml_close(std::ostringstream& output, std::string name);

// Concept xml-protected
void xml_content(std::ostringstream& output, std::string _content);

template<typename T>
void xml_simple(std::ostringstream& output, std::string name, T value) {
  output << "<" << name << ">";
  output << value;
  output << "</" << name << ">";
}

template<typename T>
void xml_simple_literal(std::ostringstream& output, std::string name, T value) {
  output << "<" << name << ">";
  output << "<![CDATA[" << value << "]]>";
  output << "</" << name << ">";
}

/**
 * \brief Method to generate json key-value pairs inside a json object element.
 */

template<typename T>
void json_simple(std::ostringstream& output, std::string name, T value) {
  output << "\"" << name << "\":" << value << ",";
}

/**
 * \brief Method to generate json key-value pairs inside a json object element.
 * The value is expected to be a literal so "'s will be added
 */

template<typename T>   // Expected to be inside a {} block
void json_simple_literal(std::ostringstream& output, std::string name, T value) {
  output << "\"" << name << "\":\"" << value << "\"" << ",";
}

template<typename T>
std::string xml_simple(std::string name, T value) {
  std::ostringstream output;

  xml_simple(output, name, value);
  return output.str();
}

template<typename T>
void xml_single_element(std::ostringstream& output, std::string name, T *obj) {
  xml_open(output, name);
  obj->getInfo(output);
  xml_close(output, name);
}

template<typename T>
void xml_iterate_list(std::ostringstream& output, std::string name, T& vector) {
  au::xml_open(output, name);

  // typename std::map<K, V* >::iterator iter;
  typename T::iterator iter;

  for (iter = vector.begin(); iter != vector.end(); ++iter) {
    (*iter)->getInfo(output);
  }

  au::xml_close(output, name);
}

template<typename T>
void xml_reverse_iterate_list(std::ostringstream& output, std::string name, T& vector) {
  au::xml_open(output, name);

  // typename std::map<K, V* >::iterator iter;
  typename T::r_iterator iter;

  for (iter = vector.r_begin(); iter != vector.r_end(); ++iter) {
    (*iter)->getInfo(output);
  }

  au::xml_close(output, name);
}

template<typename T>
void xml_iterate_list_object(std::ostringstream& output, std::string name, T& vector) {
  au::xml_open(output, name);

  // typename std::map<K, V* >::iterator iter;
  typename T::iterator iter;

  for (iter = vector.begin(); iter != vector.end(); ++iter) {
    (iter)->getInfo(output);
  }

  au::xml_close(output, name);
}

template<typename K, typename V>
void xml_iterate_map(std::ostringstream& output, std::string name, au::map<K, V>& vector) {
  au::xml_open(output, name);

  // typename std::map<K, V* >::iterator iter;
  typename au::map<K, V>::iterator iter;

  for (iter = vector.begin(); iter != vector.end(); ++iter) {
    iter->second->getInfo(output);
  }

  au::xml_close(output, name);
}
}

#endif  // ifndef _H_AU_XML
