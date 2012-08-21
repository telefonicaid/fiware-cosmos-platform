/* ****************************************************************************
 *
 * FILE                     DataCreator.cpp
 *
 * DESCRIPTION			   Creation of Data headers
 *
 */

#include "AUTockenizer.h"               // AUTockenizer
#include "DataCreator.h"                // Own interface
#include "au/string.h"

namespace samson {
ModuleInformation *ModuleInformation::parse(std::string module_file, au::ErrorManager *error) {
  // Create module_information element to be return if no error...
  ModuleInformation *module_information = new ModuleInformation();

  bool module_section_defined = false;   // Flag to detect if module section is defined...

  // Open file....
  std::string content;

  ifstream input;

  input.open(module_file.c_str());

  if (!input.is_open()) {
    error->set(au::str("%s: Not possible to open file\n", module_file.c_str()));
    delete module_information;
    return NULL;
  }

  // Read content of the file...
  ostringstream o;
  char buffer[1001];
  while (!input.eof()) {
    input.read(buffer, 1000);
    int num = input.gcount();
    buffer[num] = '\0';
    o << buffer;
  }
  input.close();
  content = o.str();

  // Parse content of file
  AUTockenizer *t = new AUTockenizer(module_file, content);

  // Parse contents of the items creating elements, datas, formats, operations, etc...
  int pos = 0;
  while (pos < (int)t->items.size()) {
    if (t->isSpecial(pos)) {
      error->set(
        au::str("%s[%d]: Error parsing file ( found %s )\n",
                module_file.c_str(),
                t->items[pos].line,
                t->items[pos].str.c_str()
                ));
      delete module_information;
      return NULL;
    }

    std::string command = t->itemAtPos(pos++).str;

    if (t->isSpecial(pos)) {
      error->set(
        au::str("%s[%d]: Error parsing file ( found %s )\n",
                module_file.c_str(),
                t->items[pos].line,
                t->items[pos].str.c_str()
                ));
      delete module_information;
      return NULL;
    }

    std::string name = t->itemAtPos(pos++).str;

    int position_start, position_finish;
    t->getScopeLimits(&pos, &position_start, &position_finish);

    if (command == "Module") {
      if (module_section_defined) {
        error->set(
          au::str("%s[%d]: Duplicated module section ( found %s )\n",
                  module_file.c_str(),
                  t->items[pos].line,
                  t->items[pos].str.c_str()
                  ));
        delete module_information;
        return NULL;
      }

      // Define this flag as true
      module_section_defined = true;

      // Get information for module section
      module_information->module.name = name;
      module_information->module.parse(t, position_start, position_finish);
    } else if (command == "data") {
      if (!module_section_defined) {
        error->set(
          au::str("%s[%d]: Module section should be the first one ( found %s )\n",
                  module_file.c_str(),
                  t->items[pos].line,
                  t->items[pos].str.c_str()
                  ));
        delete module_information;
        return NULL;
      }



      // std::cout << "Processing Data " << name << " in module file " << module_file << std::endl;


      DataContainer data_container(module_information->module.name, name);
      data_container.parse(t, position_start, position_finish);
      module_information->datas.push_back(data_container);
    } else if (
      ( command == "script" )             ||
      ( command == "generator" )          ||
      ( command == "splitter" )           ||
      ( command == "map" )                ||
      ( command == "parser" )             ||
      ( command == "simpleParser" )       ||
      ( command == "parserOut" )          ||
      ( command == "parserOutReduce" )        ||
      ( command == "reduce" )
      )
    {
      if (!module_section_defined) {
        error->set(
          au::str("%s[%d]: Module section should be the first one ( found %s )\n",
                  module_file.c_str(),
                  t->items[pos].line,
                  t->items[pos].str.c_str()
                  ));
        delete module_information;
        return NULL;
      }

      OperationContainer operation_container(module_information->module.name, command, name);
      operation_container.parse(t, position_start, position_finish);
      module_information->operations.push_back(operation_container);
    } else {
      fprintf(stderr, "samsonModuleParser: Unknown command '%s' at line:%d\n", command.c_str(), t->items[pos].line);
      exit(1);
    }
  }

  delete t;

  return module_information;
}

void ModuleInformation::printMainHeaderFile(std::string outputFileName) {
#pragma mark MODULE .h file

  std::string data_file_name =  outputFileName + ".h";

  // std::cout << "Creating file " << data_file_name << "\n";


  std::ofstream output(data_file_name.c_str());

  output << "\n\n";

  output << "/*\n\n\tModule " << " (" << module.name << ")\n\n";
  output << "\tFile: " << data_file_name << "\n";
  output << "\tNOTE: This file has been generated with the samson_module tool, please do not modify\n\n";
  output << "*/\n\n";

  output << "\n\n";

#pragma mark IFDEF

  output << "#ifndef " << module.getDefineUniqueName() << "\n";
  output << "#define " << module.getDefineUniqueName() << "\n";

  output << "\n\n";

#pragma mark INCLUDES

  output << "\n#include <samson/module/samson.h>\n";

  // Include all data files
  for (size_t i = 0; i < datas.size(); i++) {
    output << "#include " << datas[i].getIncludeFile() << "\n";
  }

  // Include all operation files
  for (size_t i = 0; i < operations.size(); i++) {
    output << "#include \"operations/" << operations[i].name << ".h\"\n";
  }

  output << "\n\n";

#pragma mark NAMESPACE

  output << "namespace samson{\n";
  output << "namespace " << module.name << "{\n";

#pragma mark MODULE

  output << "\n// Module definition\n\n";
  output << "\tclass " << module.getClassName() << " : public samson::Module\n";

  output << "\t{\n";
  output << "\tpublic:\n";

  output << "\t\t" << module.getClassName() << "();\n";

  // Help of this function
  output << "\n\t\tstd::string help(){\n";
  output << "\t\t\tstd::ostringstream o;\n";
  for (std::vector<std::string>::iterator iter = module.help.begin(); iter < module.help.end(); iter++) {
    output << "\t\t o<<\"" << (*iter) << "\";\n";
  }
  output << "\t\t\treturn o.str();\n";

  output << "\t\t}\n";

  output << "\t};\n";

  // End of all definition

  output << "\n\n";

#pragma mark END NAMESPACE

  output << "} // end of namespace " << module.name << "\n";
  output << "} // end of namespace samson\n\n";

  output << "\n\n";

  output << "#endif\n";

  output.close();
}

void ModuleInformation::printMainFile(std::string outputFileName) {
  // Print .cpp file for module definition
  std::string output_filename_cpp =  outputFileName + ".cpp";

  // std::cout << "Creating file " << output_filename_cpp << "\n";


  std::ofstream output(output_filename_cpp.c_str());

  output << "\n\n";


  output << "/**\n";
  output << "\tThis file has been autogenerated using samsonModuleParser." << std::endl;
  output << "\tPlease, do not edit!" << std::endl;
  output << "*/\n";

  output << "\n\n";

  output << "#include \"" << outputFileName << ".h\"" << std::endl;


  std::set<std::string> includes;
  for (vector <OperationContainer>::iterator iter = operations.begin(); iter < operations.end(); iter++) {
    OperationContainer op = *iter;
    if (( op.type == "reduce") || ( op.type == "parserOutReduce"))
      op.getIncludes(includes);
  }
  for (std::set<std::string>::iterator iter = includes.begin(); iter != includes.end(); iter++) {
    output << *iter;
  }




  output << "\n\n";

  output << "extern \"C\" {" << std::endl;
  output << "\tsamson::Module * moduleCreator( )" << std::endl;
  output << "\t{" << std::endl;
  output << "\t\treturn new " << module.getFullClassName() << "();" << std::endl;
  output << "\t}" << std::endl;
  output << "\tconst char* getSamsonVersion()" << std::endl;
  output << "\t{" << std::endl;
  output << "\t\treturn SAMSON_VERSION;" << std::endl;
  output << "\t}" << std::endl;
  output << "}\n\n" << std::endl;


  output << "namespace samson{\n";
  output << "namespace " << module.name << "{\n";

  output << "\n\n";

  output << "\t" << module.getClassName() << "::" << module.getClassName() << "()";
  output << " : samson::Module(\"" << module.name << "\",\"" << module.version << "\",\"" << module.author << "\")\n";
  output << "\t{";

  output << "\n";
  output << "\t\t//Add datas\n";
  for (vector <DataContainer>::iterator iter = datas.begin(); iter < datas.end(); iter++) {
    output << "\t\tadd( new DataImpl<" << iter->name << ">(\"" << iter->module << "." << iter->name << "\" )";
    output << ");\n";
  }

  output << "\n";
  output << "\t\t//Add operations\n";

  // Add Operations

#pragma mark Static operation

  for (vector <OperationContainer>::iterator iter = operations.begin(); iter < operations.end(); iter++) {
    OperationContainer op = *iter;


    output << "\t\t{\n";

    if ((op.type == "simpleParser")) {
      output << "\t\t\tsamson::Operation * operation = new samson::OperationImpl<" << op.name << ">( \"" <<
      op.module << "." << op.name <<
      "\" , samson::Operation::parser);" << std::endl;
    } else if (op.type == "script") {
      output << "\t\t\tsamson::Operation * operation = new samson::Operation( \"" << op.module << "." << op.name <<
      "\" , samson::Operation::" << op.type << ");" << std::endl;
    } else {
      output << "\t\t\tsamson::Operation * operation = new samson::OperationImpl<" << op.name << ">( \"" <<
      op.module << "." << op.name <<
      "\" , samson::Operation::" << op.type << ");" << std::endl;  // Adding input and output of the parser and parserOut
    }
    if (( op.type == "parser") || (op.type == "simpleParser"))
      output << "\t\t\toperation->inputFormats.push_back( samson::KVFormat::format(\"txt\" ,\"txt\") );" << std::endl;
    if (op.type == "parserOut")
      output << "\t\t\toperation->outputFormats.push_back( samson::KVFormat::format(\"txt\" ,\"txt\") );" << std::endl;
    if (op.type == "parserOutReduce")
      output << "\t\t\toperation->outputFormats.push_back( samson::KVFormat::format(\"txt\" ,\"txt\") );" << std::endl;
    for (size_t i = 0; i < op.inputs.size(); i++) {
      output << "\t\t\toperation->inputFormats.push_back( samson::KVFormat::format(\"" <<
      op.inputs[i].key_values.keyFormat << "\" ,\"" <<
      op.inputs[i].key_values.valueFormat << "\") );" << std::endl;
    }

    for (size_t i = 0; i < op.outputs.size(); i++) {
      output << "\t\t\toperation->outputFormats.push_back( samson::KVFormat::format(\"" <<
      op.outputs[i].key_values.keyFormat <<
      "\" ,\"" << op.outputs[i].key_values.valueFormat << "\") );" << std::endl;
    }


    // Help
    output << "\t\t\toperation->setHelpLine(\"" << op.helpLine << "\");\n";

    // Full help
    output << "\t\t\tstd::ostringstream o;\n";
    for (size_t i = 0; i < op.help.size(); i++) {
      output << "\t\t\to << \"" << op.help[i] << "\\n\";\n";
    }
    output << "\t\t\toperation->setHelp( o.str() );\n";

    // Code if any
    if (op.code.length() > 0) {
      output << std::endl;
      output << "\t\t\t// Code of this operation (usually scripts)\n";
      output << std::endl;
      std::ostringstream command;
      for (size_t i = 0; i < op.code.length(); i++) {
        if (op.code[i] != '\t') {
          if ((op.code[i] == '\n' ) || (op.code[i] == ';')) {
            if (command.str().length() > 0)
              output << "\t\t\toperation->code.push_back(\"" << command.str() <<  "\");\n"; command.str("");
          } else {
            command << op.code[i];
          }
        }
      }
      if (command.str().length() > 0)
        output << "\t\t\toperation->code.push_back(\"" << command.str() <<  "\");\n"; output << std::endl;
    }

    output << "\t\t\tadd( operation ); // Add this operation to the module\n";
    output << "\t\t}\n";
  }


#pragma mark  END OF ADDING OPERATIONS


  output << "\n";

  output << "\t}; // end of class\n";

  output << "\n";

  output << "} // end of namespace samson\n";
  output << "} // end of namespace " << module.name << "\n";;


  // Implementation of the Module constructor
  output.close();
}

void ModuleInformation::print(std::string outputDirectory, std::string outputFileName) {
  // Generate the data files
  for (size_t i = 0; i < datas.size(); i++) {
    datas[i].printFiles(outputDirectory + "/");
  }

  // Generate the operation files
  for (size_t i = 0; i < operations.size(); i++) {
    operations[i].printFile(outputDirectory + "/");
  }


  // Print the header file
  printMainHeaderFile(outputFileName);

  // Print the main .cpp file
  printMainFile(outputFileName);
}
}
