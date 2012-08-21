#ifndef DATATYPE_H
#define DATATYPE_H

/* ****************************************************************************
 *
 * FILE                     DataType.h
 *
 * DESCRIPTION				Definition of a basic data-type
 */
#include <string>


#include <iostream>
#include <sstream>

#include <stdlib.h>                   // exit
#include <unistd.h>
#include <vector>


// Defines for the optional stuff
#define NAME_FILLEDOPTIONALFIELDS "__filledOptFields__"
#define UINT8                     "system.UInt8"
#define UINT16                    "system.UInt16"
#define UINT32                    "system.UInt32"
#define UINT64                    "system.UInt64"

using namespace std;

namespace samson {
// Basic operations with "." separated names
std::vector<std::string> tockenizeWithDots(std::string myString);
std::string getModuleFromFullName(std::string fullName);
std::string getNameFromFullName(std::string fullName);

class DataType {
public:


  typedef enum {
    container_none,
    container_vector,
    container_list
  } DataTypeContainer;

  DataTypeContainer container;

  string fullType;
  string module;
  string type;
  string name;
  bool optional;
  size_t valMask;

  DataType(std::string _full_type, std::string _name, DataTypeContainer container, bool _optional, size_t _valMask, int nline);

  /* Get the include file that is necessary to use this data type */
  std::string getInclude();

  /* Function to give us the name of a particular class */
  string classNameForType();

  /* Function to show the declaration of the field */
  string getDeclaration(string pre_line);

  /* Set length function... only in vectors */
  string      getSetLengthFunction(string pre_line);

  /* Set assigned function... only in optional variables */
  string      getSetAssignedFunction(string pre_line);

  /* Get assigned function... only in optional variables */
  string      getGetAssignedFunction(string pre_line);

  /* Add and Erase element functions... only in vectors */
  string      getAddFunction(string pre_line);
  string  getEraseFunction(string pre_line);

  /* Initialization inside the constructor */
  string getInitialization(string pre_line);
  string getInitialization(string pre_line, string initialValue);

  string getDestruction(string pre_line);

  /* Clearing optional fileds */
  string getUnassignedOptionals(string pre_line);

  string getParseCommandIndividual(string pre_line, string _name);
  string getParseCommandForCompare(string _name, string indice);
  string getParseCommandVector(string pre_line, string _name);

  string getParseCommand(string pre_line);

  string getSerializationCommandIndividual(string pre_line, string _name);
  string getSerializationCommandVector(string pre_line, string _name);

  string getSerializeCommand(string pre_line);

  string getSizeCommandIndividual(string pre_line, string _name);
  string getSizeCommandVector(string pre_line, string _name);
  string getSizeCommandList(string pre_line, string _name);

  string getSizeCommand(string pre_line);

  string getPartitionCommandIndividual(string pre_line, string _name);
  string getPartitionCommandVector(string pre_line, string _name);

  string getPartitionCommand(string pre_line);

  string getCompareCommandIndividual(string pre_line,  string _name);
  string getCompareCommandVector(string pre_line,  string _name);

  string getCompareCommand(string pre_line);

  string getSetFromStringCommandIndividual(string pre_line, string _name);
  string getSetFromStringCommandVector(string pre_line, string _name);

  string getSetFromStringCommand(string pre_line);
  string checkSetFromStringNamed(string pre_line);

  string getGetDataPath(string pre_line, int index);
  string getGetType(string pre_line, int index);
  string getGetTypeFromStr(string pre_line, int index);
  string getGetInstance(string pre_line, int index);

  string getToStringCommandIndividual(string pre_line, string _name);
  string getToStringCommandVector(string pre_line, string _name);
  string getToStringCommand(string pre_line);

  string getToStringJSONCommand(string pre_line);

  string getToStringXMLCommand(string pre_line);

  string getToStringHTMLCommand(string pre_line);

  string getTo_paint_header_Command(string pre_line);

  string getTo_paint_header_basic_Command(string pre_line);

  string getTo_paint_value_Command(string pre_line);

  string getTo_num_basic_fields_Command(string pre_line);

  string getTo_max_depth_Command(string pre_line);

  string getTo_max_num_values_Command(string pre_line);

  string getCopyFromCommandIndividual(string pre_line, string _name);
  string getCopyFromCommandVector(string pre_line, string _name);
  string getCopyFromCommand(string pre_line);


  bool isVector() {
    return (container == container_vector);
  }

  bool isList() {
    return (container == container_list);
  }
};
}

#endif // ifndef DATATYPE_H
