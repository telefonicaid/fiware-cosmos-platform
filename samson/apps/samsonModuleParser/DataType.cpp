/* ****************************************************************************
 *
 * FILE                     DataType.cpp
 *
 * DESCRIPTION				Definition of a basic data-type
 *
 */

#include "DataType.h"       // Own interface
#include <stdio.h>
#include <string.h>         // strcpy, strtok
#include <unistd.h>         // exit(.)

namespace samson {
DataType::DataType(std::string _full_type, std::string _name, DataTypeContainer _container, bool _optional,
                   size_t _valMask,
                   int nline) {
  container = _container;
  optional = _optional;
  valMask = _valMask;
  fullType = _full_type;
  module = getModuleFromFullName(_full_type);
  type = getNameFromFullName(_full_type);

  if (tockenizeWithDots(fullType).size() <= 1) {
    fprintf(stderr,
            "samsonModuleParser: Error: Please specify a full data-type name (ex sna.Link). Input:'%s' at line:%d\n",
            fullType.c_str(), nline);
    exit(1);
  }

  name = _name;
}

/**
 *       Get the inlude file that is necessary to use this data type
 */

std::string DataType::getInclude() {
  std::ostringstream output;

  output << "<samson/modules/" << module << "/" << type << ".h>";
  return output.str();
}

/**
 *       Function to give us the name of a particular class
 */

string DataType::classNameForType() {
  ostringstream o;

  std::vector<std::string> tockens = tockenizeWithDots(fullType);
  o << "::samson::";
  for (size_t i = 0; i <= tockens.size() - 2; i++) {
    o << tockens[i] << "::";
  }
  o << tockens[tockens.size() - 1];     // Last element

  return o.str();
}

/* Function to show the declaration of the field */

string DataType::getDeclaration(string pre_line) {
  ostringstream o;

  if (container == container_vector) {
    // Basically a pointer to hold the elements
    o << pre_line << classNameForType() << " *" << name << ";\n";

    o << pre_line << "int " << name << "_length;\n";                    // Current lenght of the element
    o << pre_line << "int " << name << "_max_length;\n";                // Max lengh of this vector

    return o.str();
  }

  if (container == container_list) {
    // Use template to generate list kind elements
    o << pre_line << "::samson::List< " << classNameForType() << " > " << name << ";\n";
    return o.str();
  }


  // Simple types;
  o << pre_line << classNameForType() << " " << name << ";\n";

  return o.str();
}

/* Set length function... only in vectors */

string DataType::getSetLengthFunction(string pre_line) {
  if (!isVector()) {
    fprintf(stderr, "samsonModuleParser: Error generating the get length function over a non-vector data type");
    exit(1);
  }

  ostringstream o;

  o << pre_line << "void " << name << "SetLength(int _length){\n";
  o << pre_line << "\tif( _length > " << name << "_max_length){ \n";

  {
    o << pre_line << "\t\t" << classNameForType() << " *_previous = " << name << ";\n";
    o << pre_line << "\t\tint previous_length = " << name << "_length;\n";

    o << pre_line << "\t\tif(" << name << "_max_length == 0) " << name << "_max_length = _length;\n";
    o << pre_line << "\t\twhile(" << name << "_max_length < _length) " << name << "_max_length *= 2;\n";
    o << pre_line << "\t\t" << name << " = new " << classNameForType() << "[" << name << "_max_length ];\n";


    o << pre_line << "\t\tif( _previous ){\n";

    o << pre_line << "\t\t\tfor (int i = 0 ; i < previous_length ; i++)\n";
    o << pre_line << "\t\t\t\t" << name << "[i].copyFrom( &_previous[i] );\n";
    o << pre_line << "\t\t\tdelete[] _previous;\n";
    o << pre_line << "\t\t}\n";
  }

  o << pre_line << "\t}\n";
  o << pre_line << "\t" << name << "_length=_length;\n";
  o << pre_line << "}\n\n";


  return o.str();
}

/* Set assigned function... only in optional variables */

string DataType::getSetAssignedFunction(string pre_line) {
  ostringstream o;

  if (optional) {
    o << pre_line << "void " << name << "_SetAssigned(bool _assign)\n";
    o << pre_line << "{\n";
    o << pre_line << "\tif (_assign)\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\t" << NAME_FILLEDOPTIONALFIELDS << ".value |= " << hex << showbase << valMask << ";\n";
    o << pre_line << "\t}\n";
    o << pre_line << "\telse\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\t" << NAME_FILLEDOPTIONALFIELDS << ".value &= ~" << hex << showbase << valMask << ";\n";
    o << pre_line << "\t}\n";
    o << pre_line << "}\n";
  }
  return o.str();
}

/* Get assigned function... only in optional variables */

string DataType::getGetAssignedFunction(string pre_line) {
  ostringstream o;

  if (optional) {
    o << pre_line << "bool " << name << "_GetAssigned(void)\n";
    o << pre_line << "{\n";
    o << pre_line << "\treturn (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ");\n";
    o << pre_line << "}\n";
  }
  return o.str();
}

/* Add element function... only in vectors */

string DataType::getAddFunction(string pre_line) {
  if (!isVector()) {
    fprintf(stderr, "samsonModuleParser: Error generating the add function over a non-vector data type");
    exit(1);
  }

  ostringstream o;

  o << pre_line <<  classNameForType() << "* " << name << "Add(){\n";
  o << pre_line << "\t" << name << "SetLength( " << name << "_length + 1 );\n";
  o << pre_line << "\t" << "return &" << name << "[" << name << "_length-1];\n";
  o << pre_line << "}\n\n";

  return o.str();
}

/* Erase element function... only in vectors */

string DataType::getEraseFunction(string pre_line) {
  if (!isVector()) {
    fprintf(stderr, "samsonModuleParser: Error generating the erase function over a non-vector data type");
    exit(1);
  }

  ostringstream o;

  o << pre_line <<  "bool "  << name << "Erase( int item_to_erase){\n";
  o << pre_line << "\t" << "if ((item_to_erase < 0) || (item_to_erase >= " << name << "_length)) return false;\n";
  o << pre_line << "\t" << "for (int i = item_to_erase; (i < " << name << "_length-1); i++)\n";
  o << pre_line << "\t\t" << name << "[i].copyFrom(&" << name << "[i+1]);\n";
  o << pre_line << "\t" << name << "_length -= 1;\n";
  o << pre_line << "\t" << "return true;\n";
  o << pre_line << "}\n\n";

  return o.str();
}

/* Initialization inside the constructor */

string DataType::getInitialization(string pre_line, string initial_value) {
  ostringstream o;

  o << pre_line << name << " = " << initial_value << ";\n";

  return o.str();
}

string DataType::getInitialization(string pre_line) {
  ostringstream o;

  if (valMask == 0) {  // artificially inserted NAME_FILLEDOPTIONALFIELDS field
    o << pre_line << name << " = 0;\n";
    return o.str();
  }

  // Only requited in vectors and lists
  if (isVector()) {
    o << pre_line << name << "_length=0;\n";                    // Current lenght of the element
    o << pre_line << name << "_max_length=0;\n";                // Max lengh of this vector
    o << pre_line << name << " = NULL;\n";                      // Point to null
    return o.str();
  }

  if (isList()) {
    o << pre_line << name << ".clear();\n";
    return o.str();
  }

  return o.str();
}

string DataType::getDestruction(string pre_line) {
  ostringstream o;

  // Only requited in vectors
  if (isVector()) {
    // The same with a vector of classes
    o << pre_line << "if( " << name << " )\n";
    o << pre_line << "\tdelete[] " << name << " ;\n";
  } else if (isList()) {
    // No destructor since everything is included in samson::list
  }


  return o.str();
}

string DataType::getUnassignedOptionals(string pre_line) {
  ostringstream o;

  if (valMask == 0) {  // artificially inserted NAME_FILLEDOPTIONALFIELDS field
    o << pre_line << name << " = 0;\n";
    return o.str();
  }

  return o.str();
}

string DataType::getParseCommandIndividual(string pre_line, string _name) {
  ostringstream o;

  o << pre_line << "{ //Parsing " << name << "\n";
  o << pre_line << "\t_offsetInternal += " << _name << ".parse(_dataInternal+_offsetInternal);\n";
  o << pre_line << "}\n";
  return o.str();
}

string DataType::getParseCommandForCompare(string _name, string indice) {
  ostringstream o;

  o << "(*_offsetInternal" << indice << ") += " << _name << ".parse(_dataInternal" << indice << "+" <<
  "*_offsetInternal" << indice << ");";
  return o.str();
}

string DataType::getParseCommandVector(string pre_line, string _name) {
  ostringstream o;
  string pre_line_local;

  _name  = "not used";
  o << pre_line << "{ //Parsing vector " << name << "\n";
  o << pre_line << "\tsize_t _length;\n";
  o << pre_line << "\t_offsetInternal += samson::staticVarIntParse( _dataInternal+_offsetInternal , &_length );\n";
  o << pre_line << " \t" << name << "SetLength( _length );\n";
  o << pre_line << "\tfor (int i = 0 ; i < (int)" << name << "_length ; i++)\n";
  pre_line_local = pre_line + "\t";
  o << getParseCommandIndividual(pre_line_local, name + "[i]") << "\n";
  o << pre_line << "}\n";
  return o.str();
}

string DataType::getParseCommand(string pre_line) {
  ostringstream o;

  if (valMask == 0) {  // artificially inserted NAME_FILLEDOPTIONALFIELDS field
    o << getParseCommandIndividual(pre_line, name) << "\n";
    return o.str();
  }

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
  }
  if (isVector()) {
    o << getParseCommandVector(pre_line, name) << "\n";
  } else {
    o << getParseCommandIndividual(pre_line, name) << "\n";
  } return o.str();
}

string DataType::getSerializationCommandIndividual(string pre_line, string _name) {
  ostringstream o;

  o << pre_line << "{ //Serializing " << name << "\n";
  o << pre_line << "\t_offsetInternal += " << _name << ".serialize(_dataInternal+_offsetInternal);\n";
  o << pre_line << "}\n";
  return o.str();
}

string DataType::getSerializationCommandVector(string pre_line, string _name) {
  ostringstream o;
  string pre_line_local;

  _name  = "not used";

  o << pre_line << "{ //Serialization vector " << name << "\n";
  o << pre_line << "\t_offsetInternal += samson::staticVarIntSerialize( _dataInternal+_offsetInternal , " << name <<
  "_length );\n";
  o << pre_line << "\tfor (int i = 0 ; i < (int)" << name << "_length ; i++)\n";
  pre_line_local = pre_line + "\t";
  o << getSerializationCommandIndividual(pre_line_local, name + "[i]") << "\n";
  o << pre_line << "}\n";
  return o.str();
}

string DataType::getSerializeCommand(string pre_line) {
  ostringstream o;


  if (valMask == 0) {  // artificially inserted NAME_FILLEDOPTIONALFIELDS field
    o << getSerializationCommandIndividual(pre_line, name) << "\n";
    return o.str();
  }

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
  }
  if (isVector()) {
    o << getSerializationCommandVector(pre_line, name) << "\n";
  } else {
    o << getSerializationCommandIndividual(pre_line, name) << "\n";
  } return o.str();
}

string DataType::getSizeCommandIndividual(string pre_line, string _name) {
  ostringstream o;

  _name  = "not used";

  o << pre_line << "{ //Sizing " << name << "\n";
  o << pre_line << "\t_offsetInternal += " << classNameForType() << "::size(_dataInternal+_offsetInternal);\n";
  o << pre_line << "}\n";
  return o.str();
}

string DataType::getSizeCommandList(string pre_line, string _name) {
  ostringstream o;

  _name  = "not used";

  o << pre_line << "{ //Sizing " << name << "\n";
  o << pre_line << "\t_offsetInternal += ::samson::List< " << classNameForType() <<
  " >::size(_dataInternal+_offsetInternal);\n";
  o << pre_line << "}\n";
  return o.str();
}

string DataType::getSizeCommandVector(string pre_line, string _name) {
  ostringstream o;
  string pre_line_local;

  _name  = "not used";

  o << pre_line << "{ //Getting size of vector " << name << "\n";
  o << pre_line << "\tsize_t _length;\n";
  o << pre_line << "\t_offsetInternal += samson::staticVarIntParse( _dataInternal+_offsetInternal , &_length );\n";

  o << pre_line << "\t" << classNameForType() << " _tmp;\n";
  o << pre_line << "\tfor (int i = 0 ; i < (int)_length ; i++)\n";
  pre_line_local = pre_line + "\t";
  o << getSizeCommandIndividual(pre_line_local, "_tmp") << "\n";
  o << pre_line << "}\n";
  return o.str();
}

string DataType::getSizeCommand(string pre_line) {
  ostringstream o;

  if (valMask == 0) {  // artificially inserted NAME_FILLEDOPTIONALFIELDS field
    o << getSizeCommandIndividual(pre_line, name) << "\n";
    return o.str();
  }

  if (optional) {
    o << pre_line << "if (local" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
  }
  if (isVector()) {
    o << getSizeCommandVector(pre_line, name) << "\n";
  } else if (isList()) {
    o << getSizeCommandList(pre_line, name) << "\n";
  } else {
    o << getSizeCommandIndividual(pre_line, name) << "\n";
  } return o.str();
}

string DataType::getPartitionCommandIndividual(string pre_line, string _name) {
  ostringstream o;

  o << pre_line << "{ //Partitioning " << name << "\n";
  o << pre_line << "\treturn " << _name << ".hash(max_num_partitions);\n";
  o << pre_line << "}\n";
  return o.str();
}

string DataType::getPartitionCommandVector(string pre_line, string _name) {
  ostringstream o;
  string pre_line_local;

  _name  = "not used";

  o << pre_line <<  "if( " << name << "_length > 0 )\n";
  pre_line_local = pre_line + "\t";
  o << getPartitionCommandIndividual(pre_line_local, name + "[0]") << "\n";
  o << pre_line << "else return 0;\n";
  return o.str();
}

string DataType::getPartitionCommand(string pre_line) {
  ostringstream o;

  if (optional) {
    fprintf(stderr, "samsonModuleParser: Error, calling Partition on an optional field:'%s'\n", name.c_str());
    return o.str();
  }

  if (isVector()) {
    o << getPartitionCommandVector(pre_line, name) << "\n";
  } else {
    o << getPartitionCommandIndividual(pre_line, name) << "\n";
  } return o.str();
}

string DataType::getCompareCommandIndividual(string pre_line,  string _name) {
  ostringstream o;

  // Simple types;
  o << pre_line << "{ // comparing " << _name << "\n";

  if (optional) {
    o << pre_line << "\tif (((local" << NAME_FILLEDOPTIONALFIELDS << "1.value & " << hex << showbase << valMask <<
    ") != 0) && ((local" <<
    NAME_FILLEDOPTIONALFIELDS << "2.value & " << hex << showbase << valMask << ") == 0))\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\treturn 1;\n";
    o << pre_line << "\t}\n";
    o << pre_line << "\telse if (((local" << NAME_FILLEDOPTIONALFIELDS << "1.value & " << hex << showbase << valMask <<
    ") == 0) && ((local" << NAME_FILLEDOPTIONALFIELDS << "2.value & " << hex << showbase << valMask << ") != 0))\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\treturn -1;\n";
    o << pre_line << "\t}\n";
    o << pre_line << "\telse if ((local" << NAME_FILLEDOPTIONALFIELDS << "1.value & " << hex << showbase << valMask <<
    ") && (local" <<
    NAME_FILLEDOPTIONALFIELDS << "2.value & " << hex << showbase << valMask << "))\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\tint tmp = " << classNameForType() <<
    "::compare(_dataInternal1,_dataInternal2,_offsetInternal1 , _offsetInternal2);\n";
    o << pre_line << "\t\tif( tmp != 0) return tmp;\n";
    o << pre_line << "\t}\n";
  } else {
    o << pre_line << "\tint tmp = " << classNameForType() <<
    "::compare(_dataInternal1,_dataInternal2,_offsetInternal1 , _offsetInternal2);\n";
    o << pre_line << "\tif( tmp != 0) return tmp;\n";
  }
  o << pre_line << "}   //  " << _name << " compared \n";


  return o.str();
}

string DataType::getCompareCommandVector(string pre_line,  string _name) {
  ostringstream o;

  _name  = "not used";

  o << pre_line << "{ // Comparing vector " << name << "\n";

  if (optional) {
    o << pre_line << "\tif (((local" << NAME_FILLEDOPTIONALFIELDS << "1.value & " << hex << showbase << valMask <<
    ") != 0) && ((local" <<
    NAME_FILLEDOPTIONALFIELDS << "2.value & " << hex << showbase << valMask << ") == 0))\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\treturn 1;\n";
    o << pre_line << "\t}\n";
    o << pre_line << "\telse if (((local" << NAME_FILLEDOPTIONALFIELDS << "1.value & " << hex << showbase << valMask <<
    ") == 0) && ((local" << NAME_FILLEDOPTIONALFIELDS << "2.value & " << hex << showbase << valMask << ") != 0))\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\treturn -1;\n";
    o << pre_line << "\t}\n";
    o << pre_line << "\telse if ((local" << NAME_FILLEDOPTIONALFIELDS << "1.value & " << hex << showbase << valMask <<
    ") && (local" <<
    NAME_FILLEDOPTIONALFIELDS << "2.value & " << hex << showbase << valMask << "))\n";
    o << pre_line << "\t{\n";


    o << pre_line << "\t\tsize_t _length1,_length2;\n";
    o << pre_line <<
    "\t\t*_offsetInternal1 += samson::staticVarIntParse( _dataInternal1+(*_offsetInternal1) , &_length1 );\n";
    o << pre_line <<
    "\t\t*_offsetInternal2 += samson::staticVarIntParse( _dataInternal2+(*_offsetInternal2) , &_length2 );\n";

    o << pre_line << "\t\tif( _length1 < _length2 ) return -1;\n";
    o << pre_line << "\t\tif( _length1 > _length2 ) return 1;\n";

    o << pre_line << "\t\tfor (int i = 0 ; i < (int)_length1 ; i++)\n";

    o << getCompareCommandIndividual(pre_line + "\t\t", name + "[i]");


    o << pre_line << "\t}\n";
  } else {
    o << pre_line << "\tsize_t _length1,_length2;\n";
    o << pre_line <<
    "\t*_offsetInternal1 += samson::staticVarIntParse( _dataInternal1+(*_offsetInternal1) , &_length1 );\n";
    o << pre_line <<
    "\t*_offsetInternal2 += samson::staticVarIntParse( _dataInternal2+(*_offsetInternal2) , &_length2 );\n";

    o << pre_line << "\tif( _length1 < _length2 ) return -1;\n";
    o << pre_line << "\tif( _length1 > _length2 ) return 1;\n";

    o << pre_line << "\tfor (int i = 0 ; i < (int)_length1 ; i++)\n";

    o << getCompareCommandIndividual(pre_line + "\t", name + "[i]");
  }

  o << pre_line << "}   // vector " << name << " compared \n";

  return o.str();
}

string DataType::getCompareCommand(string pre_line) {
  ostringstream o;


  if (isVector()) {
    o << getCompareCommandVector(pre_line, name);
  } else {
    o << getCompareCommandIndividual(pre_line, name);
  } return o.str();
}

string DataType::getToStringCommandIndividual(string pre_line, string _name) {
  ostringstream o;

  o << pre_line << "{ //Texting " << name << "\n";
  o << pre_line << "\to << " << _name << ".str();\n";
  o << pre_line << "}\n";
  return o.str();
}

string DataType::getToStringCommandVector(string pre_line, string _name) {
  ostringstream o;
  string pre_line_local;

  _name  = "not used";

  o << pre_line << "{// toString of vector " << name << "\n";
  o << pre_line << "\to << \"[\" ;\n";
  o << pre_line << "\tfor(int i = 0 ; i < " << name << "_length ; i++)\n";
  o << pre_line << "\t{\n";
  pre_line_local = pre_line + "\t";
  o << getToStringCommandIndividual(pre_line_local, name + "[i]");
  o << pre_line << "\t\t o << \" \";\n";
  o << pre_line << "\t}\n";
  o << pre_line << "\to << \"]\" ;\n";

  o << pre_line << "}\n";
  return o.str();
}

string DataType::getToStringCommand(string pre_line) {
  ostringstream o;
  string add_despl = "";

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
    add_despl = "\t";
  }


  if (isVector()) {
    o << getToStringCommandVector(pre_line, name);
  } else {
    o << pre_line << getToStringCommandIndividual(pre_line, name);
  } return o.str();
}

string DataType::getToStringJSONCommand(string pre_line) {
  ostringstream o;
  string add_despl = "";

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
    add_despl = "\t";
  }

  if (isVector()) {
    o << pre_line << "{ // toStringJSON of vector " << name << "\n";
    o << pre_line << "\to << \"\\\"" << name << "\\\": \"" << ";\n";
    o << pre_line << "\to << \"[\" ;\n";
    o << pre_line << "\tfor (int i = 0 ; i < " << name << "_length ; i++)\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\to << " << name << "[i].strJSON();\n";
    o << pre_line << "\t\tif (i != " << name << "_length-1)\n";
    o << pre_line << "\t\t{\n";
    o << pre_line << "\t\t\to << \",\";\n";
    o << pre_line << "\t\t}\n";
    o << pre_line << "\t}\n";
    o << pre_line << "\to << \"]\" ;\n";
    o << pre_line << "}\n";
  } else {
    o << pre_line << "{ // toStringJSON " << name << "\n";
    o << pre_line << "\to << \"\\\"" << name << "\\\": \"<<" <<  name << ".strJSON();\n";
    o << pre_line << "}\n";
  }

  return o.str();
}

// getToStringXMLCommand Command
string DataType::getToStringXMLCommand(string pre_line) {
  ostringstream o;
  string add_despl = "";

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
    add_despl = "\t";
  }
  if (isVector()) {
    o << pre_line << "{ // toStringXML of vector " << name << "\n";
    o << pre_line << "\tfor(int i = 0 ; i < " << name << "_length ; i++)\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\to << \"<" << name << ">\" << " << name << "[i].strXML() << \"</" << name << ">\\n\";\n";
    o << pre_line << "\t}\n";
    o << pre_line << "}\n";
  } else {
    o << pre_line << "{ // toStringXML " << name << "\n";
    o << pre_line << "\to << \"<" << name << ">\" << " << name << ".strXML() << \"</" << name << ">\\n\";\n";
    o << pre_line << "}\n";
  }
  return o.str();
}

// getToStringHTMLCommand Command
string DataType::getToStringHTMLCommand(string pre_line) {
  ostringstream o;

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
  }
  if (isVector()) {
    o << pre_line << "{ // toStringHTML of vector " << name << "\n";
    o << pre_line << "\to << \"<h\" << level_html_heading << \">\" <<" << "\"" << name <<
    "</h\" << level_html_heading << \">\\n\";\n";
    o << pre_line << "\tlevel_html_heading++;\n";
    o << pre_line << "\tfor (int i = 0 ; i < " << name << "_length ; i++)\n";
    o << pre_line << "\t{\n";
    string name_component = name + "[i]";
    o << pre_line << "\t\t{ // toStringHTML " << name << "\n";
    o << pre_line << "\t\t\to << \"<h\" <<level_html_heading << \">\" <<" << "\"" << name <<
    "[\" << i << \"]\" << \"</h\" << level_html_heading << \">\\n\";\n";
    o << pre_line << "\t\t\to << " << name_component << ".strHTML(level_html_heading+1);\n";
    o << pre_line << "\t\t\to << \"\\n\";\n";
    o << pre_line << "\t\t}\n";
    o << pre_line << "\t}\n";
    o << pre_line << "}\n";
    o << pre_line << "\to << \"\\n\";\n";
  } else {
    o << pre_line << "{ // toStringHTML " << name << "\n";
    o << pre_line << "\to << \"<h\" <<level_html_heading << \">\" <<" << "\"" << name <<
    "</h\" << level_html_heading << \">\\n\";\n";
    o << pre_line << "\to << " << name << ".strHTML(level_html_heading+1);\n";
    o << pre_line << "\to << \"\\n\";\n";
    o << pre_line << "}\n";
  }

  return o.str();
}

string DataType::getTo_paint_header_Command(string pre_line) {
  ostringstream o;
  string add_despl = "";

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
    add_despl = "\t";
  }
  if (isVector()) {
    o << pre_line << "{ // paint_header  vector " << name << "\n";
    o << pre_line << "\tif (" << name << "_length > 0)\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\tend_col = init_col +  " << name << "[0].num_basic_fields();\n";
    o << pre_line << "\t\to << \"<th colspan=\" << end_col << \">" << name << "</th>\\n\";\n";
    o << pre_line << "\t\tinit_col = end_col + 1;\n";
    o << pre_line << "\t}else{\n";
    o << pre_line << "\t\to << \"<th colspan=\" << end_col << \">" << "</th>\\n\";\n";
    o << pre_line << "\t}\n";
    o << pre_line << "}\n";
  } else {
    o << pre_line << "{ // paint_header  " << name << "\n";
    o << pre_line << "\tend_col = init_col +  " << name << ".num_basic_fields();\n";
    o << pre_line << "\to << \"<th colspan=end_col>" << name << "</th>\\n\";\n";
    o << pre_line << "\tinit_col = end_col + 1;\n";
    o << pre_line << "}\n";
  }

  return o.str();
}

string DataType::getTo_paint_header_basic_Command(string pre_line) {
  ostringstream o;
  string add_despl = "";

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
    add_despl = "\t";
  }
  if (isVector()) {
    o << pre_line << "{ // paint_header_basic  vector " << name << "\n";
    o << pre_line << "\tif (" << name << "_length > 0)\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\tif (" << name << "[0].is_terminal())\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\to << \"<th>" << name << "</th>\\n\";\n";
    o << pre_line << "\t}\n";
    o << pre_line << "\telse\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\to << " << name << "[0].paint_header_basic(init_col);\n";
    o << pre_line << "\t}\n";
    o << pre_line << "\t}else{\n";
    o << pre_line << "\t\to << \"<th></th>\\n\";\n";
    o << pre_line << "\t}\n";
    o << pre_line << "}\n";
  } else {
    o << pre_line << "{ // paint_header_basic  " << name << "\n";
    o << pre_line << "\tif (" << name << ".is_terminal())\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\to << \"<th>" << name << "</th>\\n\";\n";
    o << pre_line << "\t}\n";
    o << pre_line << "\telse\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\to << " << name << ".paint_header_basic(init_col);\n";
    o << pre_line << "\t}\n";
    o << pre_line << "}\n";
  }

  return o.str();
}

string DataType::getTo_paint_value_Command(string pre_line) {
  ostringstream o;
  string add_despl = "";

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
    add_despl = "\t";
  }
  if (isVector()) {
    o << pre_line << "{ // paint_value of vector " << name << "\n";
    o << pre_line << "\t\tif ((index_row >= 0) && (index_row < " << name << "_length))\n";
    o << pre_line << "\t\t{\n";
    o << pre_line << "\t\t\to << " << name << "[index_row].paint_value(0) << \"\\n\";\n";
    o << pre_line << "\t\t}\n";
    o << pre_line << "\t\telse\n";
    o << pre_line << "\t\t{\n";
    o << pre_line << "\t\t\tif (" << name << "_length > 0)\n";
    o << pre_line << "\t\t\t{\n";
    o << pre_line << "\t\t\t\to << " << name << "[0].paint_value(-1) << \"\\n\";\n";
    o << pre_line << "\t\t\t}\n";
    o << pre_line << "\t\t\telse\n";
    o << pre_line << "\t\t\t{\n";
    o << pre_line << "\t\t\t\to << \"<td></td>\\n\";\n";
    o << pre_line << "\t\t\t}\n";
    o << pre_line << "\t\t}\n";
    o << pre_line << "}\n";
  } else {
    o << pre_line << "{ // paint_value " << name << "\n";
    o << pre_line << "\t\tif ((index_row >= 0) && (index_row < " << name << ".max_num_values()))\n";
    o << pre_line << "\t\t{\n";
    o << pre_line << "\t\t\to << " << name << ".paint_value(index_row) << \"\\n\";\n";
    o << pre_line << "\t\t}\n";
    o << pre_line << "\t\telse\n";
    o << pre_line << "\t\t{\n";
    o << pre_line << "\t\t\to << \"<td></td>\\n\";\n";
    o << pre_line << "\t\t}\n";
    o << pre_line << "}\n";
  }
  return o.str();
}

string DataType::getTo_num_basic_fields_Command(string pre_line) {
  ostringstream o;
  string add_despl = "";

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
    add_despl = "\t";
  }
  if (isVector()) {
    o << pre_line << "{ // num_basic_fields of vector " << name << "\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\tif (" << name << "_length > 0)\n";
    o << pre_line << "\t\tn_basic_fields += " << name << "[0].num_basic_fields();\n";
    o << pre_line << "\t}\n";
    o << pre_line << "}\n";
  } else {
    o << pre_line << "{ // num_basic_fields " << name << "\n";
    o << pre_line << "\t\tn_basic_fields += " << name << ".num_basic_fields();\n";
    o << pre_line << "}\n";
  }
  return o.str();
}

string DataType::getTo_max_depth_Command(string pre_line) {
  ostringstream o;
  string add_despl = "";

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
    add_despl = "\t";
  }
  if (isVector()) {
    o << pre_line << "{ // max_depth of vector " << name << "\n";
    o << pre_line << "\tif (" << name << "_length > 0)\n";
    o << pre_line << "\t{\n";
    o << pre_line << "\t\tint depth = " << name << "[0].max_depth();\n";
    o << pre_line << "\t\tif (depth > m_depth) m_depth = depth; \n";
    o << pre_line << "\t}\n";
    o << pre_line << "}\n";
  } else {
    o << pre_line << "{ // max_depth " << name << "\n";
    o << pre_line << "\t\tint depth = " << name << ".max_depth();\n";
    o << pre_line << "\t\tif (depth > m_depth) m_depth = depth; \n";
    o << pre_line << "}\n";
  }
  return o.str();
}

string DataType::getTo_max_num_values_Command(string pre_line) {
  ostringstream o;
  string add_despl = "";

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
    add_despl = "\t";
  }
  if (isVector()) {
    o << pre_line << "{ // max_num_values of vector " << name << "\n";
    o << pre_line << "\t\tint num_values = " << name << "_length;\n";
    o << pre_line << "\t\tif (num_values > m_num_values) m_num_values = num_values; \n";
    o << pre_line << "}\n";
  } else {
    o << pre_line << "{ // max_num_values " << name << "\n";
    o << pre_line << "\t\tint num_values = " << name << ".max_num_values();\n";
    o << pre_line << "\t\tif (num_values > m_num_values) m_num_values = num_values; \n";
    o << pre_line << "}\n";
  }
  return o.str();
}

// getSetFromString Command

string DataType::getSetFromStringCommandIndividual(string pre_line, string _name) {
  ostringstream o;

  o << pre_line << "\t" << _name << ".setFromString(p_item);\n";
  return o.str();
}

string DataType::getSetFromStringCommandVector(string pre_line, string _name) {
  ostringstream o;
  string pre_line_local;

  _name  = "not used";

  o << pre_line << "\tconst char *p_item = p_item_vector;" << "\n";
  o << pre_line <<
  "\tif ((p_item_vector=strchr(p_item_vector, '[')) != NULL){p_item_vector++;}else{p_item_vector=p_item;}" << "\n";
  o << pre_line << "\tconst char *p_item_prev = p_item_vector;" << "\n";
  o << pre_line << "\tint n_items = 0;" << "\n";
  o << pre_line << "\twhile ((*p_item_prev != ']') && (*p_item_prev != '\\0'))" << "\n";
  o << pre_line << "\t{" << "\n";
  pre_line_local = pre_line + "\t";
  o << pre_line << "\t\t" <<
  "if ((p_item=strchr(p_item_prev, ' ')) != NULL) { n_items++; p_item_prev = p_item+1;}else if ((p_item=strchr(p_item_prev, ',')) != NULL) { n_items++; p_item_prev = p_item+1; while (*p_item_prev == ' '){p_item_prev++;}} else if ((p_item=strchr(p_item_prev, ']')) != NULL)  { n_items++; break;} else { break;} "
    << "\n";
  o << pre_line << "\t}" << "\n";
  o << pre_line << "\t" << name << "SetLength(n_items);\n";
  o << pre_line << "\tp_item = p_item_vector;" << "\n";
  o << pre_line << "\tfor (int i = 0 ; i < (int)" << name << "_length ; i++)\n";
  o << pre_line << "\t{" << "\n";
  pre_line_local = pre_line + "\t";
  o << getSetFromStringCommandIndividual(pre_line_local, name + "[i]") << "\n";
  o << pre_line << "\t\t" <<
  "if ((p_item=strchr(p_item_vector, ' ')) != NULL) { p_item++; }else{ if ((p_item=strchr(p_item_vector, ',')) != NULL) { p_item++; if (*p_item == ' '){p_item++;}} else { p_item = p_item_vector; }}"
    << "\n";
  o << pre_line << "\t\tp_item_vector = p_item;" << "\n";
  o << pre_line << "\t}" << "\n";

  return o.str();
}

string DataType::getSetFromStringCommand(string pre_line) {
  ostringstream o;

  if (strcmp(name.c_str(), NAME_FILLEDOPTIONALFIELDS) == 0) {
    return "";
  }

  if (isVector()) {
    o << pre_line << "{ //Setting vector " << name << "\n";

    o << pre_line << "\t" << "const char *p_item_vector;" << "\n";
    o << pre_line << "\t" << "if ((p_item_vector=strstr(p_value_data, \"" << name <<
    ":[\")) != NULL) { p_item_vector += strlen(\"" <<
    name << ":[\");} else if (oneFieldNamed == true) { p_item_vector = \"\";} else {p_item_vector = p_value_data;}" <<
    "\n";

    o << getSetFromStringCommandVector(pre_line, name) << "\n";
    o << pre_line << "}\n";
  } else {
    o << pre_line << "{ //Setting " << name << "\n";
    o << pre_line << "\t" << "const char *p_item;" << "\n";
    o << pre_line << "\t" << "if ((p_item=strstr(p_value_data, \"" << name << ":\")) != NULL) { p_item += strlen(\"" <<
    name <<
    ":\");} else if (oneFieldNamed == true) { p_item = \"\";} else {p_item = p_value_data;}" << "\n";

    o << getSetFromStringCommandIndividual(pre_line, name) << "\n";
    o << pre_line << "}\n";
  }

  return o.str();
}

string DataType::checkSetFromStringNamed(string pre_line) {
  ostringstream o;

  if (strcmp(name.c_str(), NAME_FILLEDOPTIONALFIELDS) == 0) {
    return "";
  }

  o << pre_line << "\t" << "if ((p_item=strstr(p_value_data, \"" << name <<
  ":\")) != NULL) { oneFieldNamed = true;}" << "\n";

  return o.str();
}

// getDataPath Command

string DataType::getGetDataPath(string pre_line, int index) {
  ostringstream o;

  o << pre_line << "{ //Scanning " << name << ", terminal and non-terminal\n";
  o << pre_line << "\tif (strcmp(dataPathCharP, \"" << name << "\") == 0)\n";
  o << pre_line << "\t{\n";
  o << pre_line << "\t\t*dataPathIntP = " << index << ";\n";
  o << pre_line << "\t\t*(dataPathIntP+1) = -1;\n";
  o << pre_line << "\t\treturn (0);\n";
  o << pre_line << "\t}\n";
  o << pre_line << "\tif (strncmp(dataPathCharP, \"" << name << ".\", strlen(\"" << name << ".\")) == 0)\n";
  o << pre_line << "\t{\n";
  o << pre_line << "\t\t*dataPathIntP = " << index << ";\n";
  o << pre_line << "\t\treturn (" << module << "::" << type << "::getDataPath(dataPathCharP+strlen(\"" << name <<
  ".\"),dataPathIntP+1));\n";
  o << pre_line << "\t}\n";
  o << pre_line << "}\n";

  return o.str();
}

string DataType::getGetTypeFromStr(string pre_line, int index) {
  ostringstream o;

  index = 0;

  o << pre_line << "{ //Scanning " << name << ", terminal and non-terminal\n";
  o << pre_line << "\tif (strcmp(dataPathCharP, \"" << name << "\") == 0)\n";
  o << pre_line << "\t{\n";
  o << pre_line << "\treturn(\"" << module << "." << type << "\");\n";
  o << pre_line << "\t}\n";
  o << pre_line << "\tif (strncmp(dataPathCharP, \"" << name << ".\", strlen(\"" << name << ".\")) == 0)\n";
  o << pre_line << "\t{\n";
  o << pre_line << "\treturn(" << module << "::" << type << "::getTypeFromPathStatic(dataPathCharP+strlen(\"" <<
  name << ".\")));\n";
  o << pre_line << "\t}\n";
  o << pre_line << "}\n";
  return o.str();
}

// getGetType Command

string DataType::getGetType(string pre_line, int index) {
  ostringstream o;

  o << pre_line << "case " << index << ":\n";
  o << pre_line << "\treturn(" << module << "::" << type << "::getTypeFromPathStatic(dataPathIntP+1));\n";
  o << pre_line << "\tbreak;\n";

  return o.str();
}

string DataType::getGetInstance(string pre_line, int index) {
  ostringstream o;

  o << pre_line << "case " << index << ":\n";
  if (optional) {
    o << pre_line << "\tif (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
    if (isVector()) {
      o << pre_line << "\t\treturn((*" << name << ").getDataInstanceFromPath(dataPathIntP+1));\n";
    } else {
      o << pre_line << "\t\treturn(" << name << ".getDataInstanceFromPath(dataPathIntP+1));\n";
    }
    o << pre_line << "\telse\n";
    o << pre_line << "\t\treturn (NULL);\n";
  } else {
    if (isVector()) {
      o << pre_line << "\treturn((*" << name << ").getDataInstanceFromPath(dataPathIntP+1));\n";
    } else {
      o << pre_line << "\treturn(" << name << ".getDataInstanceFromPath(dataPathIntP+1));\n";
    }
  }
  o << pre_line << "\tbreak;\n";

  return o.str();
}

// CopyFrom Command



string DataType::getCopyFromCommandIndividual(string pre_line, string _name) {
  ostringstream o;

  o << pre_line << "{ //Copying " << name << "\n";
  o << pre_line << "\t" << _name << ".copyFrom(&_otherField->" << _name << ");\n";
  o << pre_line << "}\n";
  return o.str();
}

string DataType::getCopyFromCommandVector(string pre_line, string _name) {
  ostringstream o;
  string pre_line_local;

  _name  = "not used";

  o << pre_line << "{ // CopyFrom field " << name << "\n";
  o << pre_line << "\t" << name << "SetLength( _otherField->" << name << "_length);\n";
  o << pre_line << "\tfor (int i = 0 ; i < " << name << "_length ; i++)\n";
  pre_line_local = pre_line + "\t";
  o << getCopyFromCommandIndividual(pre_line_local, name + "[i]");
  o << pre_line << "}\n";
  return o.str();
}

string DataType::getCopyFromCommand(string pre_line) {
  ostringstream o;

  if (optional) {
    o << pre_line << "if (" << NAME_FILLEDOPTIONALFIELDS << ".value & " << hex << showbase << valMask << ")\n";
  }
  if (isVector()) {
    o << getCopyFromCommandVector(pre_line, name) << "\n";
  } else {
    o << getCopyFromCommandIndividual(pre_line, name) << "\n";
  } return o.str();
}

#pragma mark -----

std::vector<std::string> tockenizeWithDots(std::string myString) {
  if (myString.length() > 1000) {
    fprintf(stderr, "samsonModuleParser: tockenizeWithDots received a string with more than 1000 dot separated words");
    exit(1);
  }

  char tmp[1000];

  strcpy(tmp, myString.c_str());
  std::vector<std::string> tokens;

  char *p = strtok(tmp, ".");
  while (p) {
    tokens.push_back(std::string(p));
    p = strtok(NULL, " ");
  }

  return tokens;
}

std::string getModuleFromFullName(std::string fullName) {
  std::vector<std::string>  tokens = tockenizeWithDots(fullName);
  ostringstream output;

  for (size_t i = 0; i < (tokens.size() - 1); i++) {
    output << tokens[i];
    if (i < (tokens.size() - 2)) {
      output << ".";
    }
  }

  return output.str();
}

std::string getNameFromFullName(std::string fullName) {
  std::vector<std::string> tokens = tockenizeWithDots(fullName);
  return tokens[tokens.size() - 1];
}
}
