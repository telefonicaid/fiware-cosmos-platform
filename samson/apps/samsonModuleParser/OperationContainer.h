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
 * FILE                     OperationContainer.h
 *
 * DESCRIPTION				Information about an operation  (module definition)
 *
 * ***************************************************************************/

#pragma once

#include <unistd.h>             /* _exit, ...                              */
#include <string.h>             /* strtok, strcpy, ...                     */
#include <cstdio>
#include <map>
#include <string>
#include <iostream>
#include <string>
#include <vector>

#include <samson/module/KVFormat.h>
#include "DataType.h"
#include "AUTockenizer.h"           // Re-tockenize header info to test consistency between module and implementation



namespace samson
{
class DataCreator;
class AUTockenizer;



/**
 * Container to hold information of a dataset
 */
class DataSet
{
public:

    KVFormat key_values;

    std::string compareKeyFunction;
    std::string compareValueFunction;

    DataSet ( KVFormat _key_values )
    {
        key_values = _key_values;

        compareKeyFunction = "compare";
        compareValueFunction = "compare";
    }

    DataSet ( KVFormat _key_values, std::string _compareKeyFunction, std::string _compareValueFunction )
    {
        key_values = _key_values;

        if ( _compareKeyFunction  == "" )
            compareKeyFunction = "compare";
        else
            compareKeyFunction = _compareKeyFunction;

        if (_compareValueFunction  == "")
            compareValueFunction = "compare";
        else
            compareValueFunction = _compareValueFunction;
    }

};

/**
	 Container to hold information for an operation
 */

class OperationContainer
{

public:

    bool setup;

    std::string module;		            // Name of the module
    std::string name;		            // Name of the operation

    std::string type;                   // Type of operation ( map , reduce , .... )

    std::vector <DataSet> inputs;       // Inputs and outputs for this operation
    std::vector <DataSet> outputs;

    std::string helpLine;				// Help in one line
    std::vector <std::string> help;		// More extended help

    std::string info;

    std::string code;		            // Code for scripts

    std::string file;		            // File where this operation is defined

    std::vector <std::string> functions;

    bool top;
    bool dynamic_input_formats;
    bool destructor;

    OperationContainer( std::string _module , std::string _type, std::string _name )
    {
        module = _module;
        type = _type;
        name = _name;

        setup = false;
        top =  false;
        dynamic_input_formats = false;
        destructor = false;
    }

    void addInput( DataSet input_dataset )
    {
        if(( type == "parser" ) || (type == "simpleParser"))
        {
            std::cerr << "Error in operation " << name << ": Not possible to add inputs to a parser. The unique txt-txt input will be added automatically\n";
            exit(1);
        }

        if( type == "map" && inputs.size() > 0)
        {
            std::cerr << "samsonModuleParser: Error in operation " << name << ": Maps can only contain one input\n";
            exit(1);
        }

        if (type != "script")
        {
            size_t found=input_dataset.key_values.keyFormat.find('.');
            if (found == std::string::npos)
            {
                std::cerr << "samsonModuleParser: Error in operation " << name << ": Input key without module specification(" << input_dataset.key_values.keyFormat << ")\n";
                exit(1);
            }
            found=input_dataset.key_values.valueFormat.find('.');
            if (found == std::string::npos)
            {
                std::cerr << "samsonModuleParser: Error in operation " << name << ": Input value without module specification(" << input_dataset.key_values.valueFormat << ")\n";
                exit(1);
            }
        }

        inputs.push_back(input_dataset);
    }

    void addOutput( DataSet output_dataset )
    {

        //printf("Adding output %s ( %s )\n", name.c_str() , type.c_str() );

        if( ( type == "parserOut" ) || ( type == "parserOutReduce" ) )
        {
            std::cerr << "samsonModuleParser: Error in operation " << name << ": Not possible to add outputs to a parserOut or parserOutReduce. The unique txt-txt output will be added automatically\n";
            exit(1);
        }

        outputs.push_back(output_dataset);
    }

    bool check()
    {
        // Check everything is correct

        //Checking the use of reserved words in the name
        std::string arr_reserved_words[] = {"auto", "const", "double", "float", "int", "short", "struct", "unsigned", "break", "continue", "else", "for", "long", "signed", "switch", "void", "case", "default", "enum",
                                        "goto", "register", "sizeof", "typedef", "volatile", "char", "do", "extern", "if", "return", "static", "union", "while", "asm", "dynamic_cast", "namespace", "reinterpret_cast",
                                        "try", "bool", "explicit", "new", "static_cast", "typeid", "catch", "false", "operator", "template", "typename", "class", "friend", "private", "this", "using",
                                        "const_cast", "inline", "public", "throw", "virtual", "delete", "mutable", "protected", "true", "wchar_t", "and", "bitand", "compl", "not_eq", "or_eq", "xor_eq",
                                        "and_eq", "bitor", "not", "or", "xor", "cin", "endl", "INT_MIN", "iomanip", "main", "npos", "std", "cout", "include", "INT_MAX", "iostream", "MAX_RAND", "NULL", "string"};

        std::set<std::string>   reserved_words;

        for (unsigned int i = 0; (i < (sizeof(arr_reserved_words)/sizeof(arr_reserved_words[0]))); i++)
        {
            reserved_words.insert(arr_reserved_words[i]);
        }

        if (reserved_words.find(name) != reserved_words.end())
        {
            std::cerr << "samsonModuleParser: Error in operation: '" << name << "'. It is a c++ reserved word. Please chose a different name\n";
            return false;
        }

        // Andreu: Now operation can have no output to export data to other system ( i.e. mongoDb )
        /*
            if ( ( type == "map" ) || ( type == "reduce" ) || ( type == "parser" ) )
                if (outputs.size() == 0)
                {
                    std::cerr << "samsonModuleParser: Error in operation " << name << ": Operation needs an output\n";
                    exit(1);
                }
         */

        if ( ( type == "map" ) || ( type == "reduce" ) || ( type == "parserOut" ) || ( type == "parserOutReduce" ) )
            if (inputs.size() == 0)
            {
                std::cerr << "samsonModuleParser: Error in operation " << name << ": Operation needs an input\n";
                return false;
            }

        if (type == "splitter")
        {
            if (inputs.size() != 0)
            {
                std::cerr << "samsonModuleParser: Error in operation " << name << ": Operation does not require any input\n";
                return false;
            }
            if (outputs.size() != 0)
            {
                std::cerr << "samsonModuleParser: Error in operation " << name << ": Operation does not require any output\n";
                return false;
            }
        }

        if ((type == "reduce") || (type == "parserOutreduce"))
        {
            std::string prevKeyFormat = inputs[0].key_values.keyFormat;

            for (size_t i = 1 ; i < inputs.size() ; i++)
            {
                if (prevKeyFormat != inputs[i].key_values.keyFormat)
                {
                    fprintf(stderr, "samsonModuleParser: Error in operation '%s' ('%s' type). keyFormat must agree for all inputs ('%s' != '%s')\n", name.c_str(), type.c_str(), prevKeyFormat.c_str(), inputs[i].key_values.keyFormat.c_str());
                    return false;
                }
            }

            std::string prevKeyCompareFunction = inputs[0].compareKeyFunction;

            for (size_t i = 1 ; i < inputs.size() ; i++)
            {
                if (prevKeyCompareFunction != inputs[i].compareKeyFunction)
                {
                    fprintf(stderr, "samsonModuleParser: Error in operation '%s' ('%s' type). compareKeyFunction must agree for all inputs ('%s' != '%s')\n", name.c_str(), type.c_str(), prevKeyCompareFunction.c_str(), inputs[i].compareKeyFunction.c_str());
                    return false;
                }
            }
        }
        return true;

    }

    void parse( AUTockenizer *module_creator , int begin , int end );

    std::string parentClass()
    {
        if( type == "generator")
            return "samson::Generator";

        if( type == "map")
            return "samson::Map";

        if( type == "reduce")
            return "samson::Reduce";

        if( type == "splitter")
            return "samson::Splitter";

        if( type == "script")
            return "samson::Script";

        if( type == "parser")
            return "samson::Parser";

        if( type == "simpleParser")
            return "samson::system::SimpleParser";

        if( type == "parserOut")
            return "samson::ParserOut";

        if( type == "parserOutReduce")
            return "samson::ParserOutReduce";


        fprintf(stderr, "samsonModuleParser: Error: Unknown type of operation in the operation section (%s)\n" , type.c_str());
        _exit(1);

        return "no sense";

    }

    void printFile( std::string directory )
    {
        std::ostringstream fileName;
        fileName << directory;	// base directory
        fileName << "operations/";
        mkdir( fileName.str().c_str() , 0755 );
        fileName << name << ".h";

        std::ifstream _file( fileName.str().c_str() );
        if( _file.is_open() )
        {
            if (checkOperationFile(_file, info) == false)
            {
                std::cerr << "Error, " << fileName.str() << ": module declaration does not agree with header comment in implementation." << std::endl;
                std::cerr << "\t\tPlease check and update the wrong declaration (module or implementation)." << std::endl;
                exit(-1);
            }
            _file.close();
            /*
                if( verbose )
                    std::cout << "File " << name << ".h is not generated because it already exist\n";
             */
            return;
        }

        /*
            if( verbose )
                std::cout << "Creating file " << name << ".h" << std::endl;
         */

        std::ofstream file( fileName.str().c_str() );

        file << "\n";
        file << "/**\n";
        file << " File autogenerated with samsonModuleParser. Please, edit to complete this operation\n";
        file << "*/\n";
        file << "\n";

        file << "#ifndef _H_SAMSON_" << module << "_" << name << "\n";
        file << "#define _H_SAMSON_" << module << "_" << name << "\n";

        file << "\n";
        file << "\n";

        file << "#include <samson/module/samson.h>\n";
        if (type != "script")
        {
            std::set<std::string> includes ;
            getIncludes(includes);
            for( std::set<std::string>::iterator iter = includes.begin() ; iter != includes.end() ; iter++)
                file << *iter;
        }
        if (type == "simpleParser")
        {
            file << "#include <samson/modules/system/SimpleParser.h>" << std::endl;
        }


        file << "\n";
        file << "\n";

        // Name space
        file << "namespace samson{\n";
        file << "namespace " << module << "{\n";

        file << "\n";
        file << "\n";

        // Class definition
        file << "\tclass " << name << " : public " << parentClass() << "\n";
        file << "\t{\n";

        file << "\n";

        // Public tag
        file << "\tpublic:\n";
        file << "\n\n";

#define INFO_MODULE " INFO_MODULE"
#define END_INFO_MODULE " END_INFO_MODULE"
#define INFO_COMMENT "#ifdef INFO_COMMENT"
#define END_INFO_COMMENT "#endif // de INFO_COMMENT"

        //file << "#ifdef INFO_COMMENT //Just to include a comment without conflicting anything\n";
        file << "// " << INFO_MODULE << std::endl;
        file << "// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)" << std::endl;
        file << "// Please, do not remove this comments, as it will be used to check consistency on module declaration" << std::endl;
        file << "//\n";

        size_t found_prev = 0;
        size_t found=info.find_first_of("\n");
        std::string line;
        while (found!=string::npos)
        {
            line = info.substr(found_prev, found - found_prev);
            file << "//  " << line << std::endl;
            found_prev = found+1;
            found=info.find_first_of("\n", found+1);
        }
        line = info.substr(found_prev, found - found_prev);
        file << "// " << line << std::endl;
        file << "// " << END_INFO_MODULE << std::endl;
        //file << "#endif // of INFO_COMMENT" << "\n";
        file << "\n";


        if( type != "script")
        {

            if( (type == "map") || (type == "reduce") )
            {
                file << "\t\tvoid init( samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
                file << "\t\tvoid run( samson::KVSetStruct* inputs , samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
                file << "\t\tvoid finish( samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
            }
            if( type == "generator" )
            {
                file << "\t\tvoid init( samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
                file << "\t\tvoid setup( int worker , int num_workers, int process , int num_processes )\n\t\t{\n\t\t}\n\n";

                file << "\t\tvoid run( samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
                file << "\t\tvoid finish( samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
            }
            if( type == "parser" )
            {
                file << "\t\tvoid init( samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
                file << "\t\tvoid run( char *data , size_t length , samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
                file << "\t\tvoid finish( samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
            }
            if( type == "simpleParser" )
            {
                file << "\t\tvoid init( samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
                file << "\t\tvoid parseLine( char * line, samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
                file << "\t\tvoid finish( samson::KVWriter *writer )\n\t\t{\n\t\t}\n\n";
            }
            if( ( type == "parserOut" ) || ( type == "parserOutReduce" ) )
            {
                file << "\t\tvoid init( TXTWriter *writer )\n\t\t{\n\t\t}\n\n";
                file << "\t\tvoid run( KVSetStruct* inputs , TXTWriter *writer )\n\t\t{\n\t\t}\n\n";
                file << "\t\tvoid finish( TXTWriter *writer )\n\t\t{\n\t\t}\n\n";
            }
            if( type == "splitter" )
            {
                file << "\t\tint split( char * inData, size_t inLength , bool finished , char ** nextData , SplitterEmitter* emitter )\n\t\t{\n\t\t\treturn 0;\n\t\t}\n\n";
            }

        }

        file << "\n\n";

        // End of the class
        file <<  "\t};\n";


        file << "\n";
        file << "\n";

        // End of namespace
        file << "} // end of namespace " << module << "\n";
        file << "} // end of namespace samson\n";


        file << "\n";

        file << "#endif\n";


        file.close();

    }

    std::string getCompareFunctionForData( std::string data , std::string compareFunction);
    std::string getIncludeForData( std::string data );

    void getIncludes( std::set<std::string>& includes )
    {
        for (size_t i = 0 ; i < inputs.size() ; i++)
        {
            includes.insert( getIncludeForData( inputs[i].key_values.keyFormat ) );
            includes.insert( getIncludeForData( inputs[i].key_values.valueFormat ) );
        }
        for (size_t i = 0 ; i < outputs.size() ; i++)
        {
            includes.insert( getIncludeForData( outputs[i].key_values.keyFormat ) );
            includes.insert( getIncludeForData( outputs[i].key_values.valueFormat ) );
        }
    }


    std::string getCompareFunction()
    {
        std::ostringstream output;

        output << "\tint " << getCompareFunctionName() << "(KV* kv1 , KV* kv2)" << std::endl;
        output << "\t{" << std::endl;

        output << "\t\tint res_key = " << getCompareFunctionForData( inputs[0].key_values.keyFormat, inputs[0].compareKeyFunction ) << "(kv1->key , kv2->key);\n";

        output << "\t\tif(res_key!=0)\n";
        output << "\t\t\treturn (res_key<0);\n";

        output << "\t\tif( kv1->input != kv2->input )\n";
        output << "\t\t\treturn (kv1->input < kv2->input);\n";

        output << "\t\tswitch (kv1->input) {\n";

        for (size_t i = 0 ; i < inputs.size() ; i++)
            output << "\t\tcase "<<i<<": return ("<< getCompareFunctionForData( inputs[i].key_values.valueFormat, inputs[i].compareValueFunction ) <<"(kv1->value , kv2->value)<0); break;\n";

        output << "\t\tdefault: exit(1); break;\n";
        output << "\t\t}\n";

        output << "\t\treturn 0;\n";

        output << "\t}" << std::endl;

        return output.str();
    }

    std::string getCompareFunctionName()
    {
        std::ostringstream output;
        output << "compare_" << module << "_" << name;
        return output.str();
    }

    std::string getCompareByKeyFunction()
    {
        std::ostringstream output;

        output << "\tint " << getCompareByKeyFunctionName() << "(KV* kv1 , KV* kv2)" << std::endl;
        output << "\t{" << std::endl;
        output << "\t\treturn " << getCompareFunctionForData( inputs[0].key_values.keyFormat, inputs[0].compareKeyFunction) << "(kv1->key , kv2->key);\n";
        output << "\t}" << std::endl;

        return output.str();
    }

    std::string getCompareByKeyFunctionName()
    {
        std::ostringstream output;
        output << "compare_" << module << "_" << name << "_by_key";
        return output.str();
    }


    bool checkOperationHeaderItems(std::vector <DataSet> _inputs, std::vector <DataSet> _outputs, std::string info, std::vector <DataSet> inputs_impl, std::vector <DataSet> outputs_impl, std::string infoFile)
    {
        bool errorDetected = false;

        if (infoFile != infoFile)
           LM_E(("avoiding a strict warning ..."));

        if (_inputs.size() != inputs_impl.size() )
        {
            cerr << ">>> Error in number of inputs to the operation: "  << name << std::endl;
            cerr << ">>>       module declaration: " << _inputs.size() << std::endl;
            cerr << ">>>       .h implementation: " << inputs_impl.size() << std::endl;
            cerr << ">>>" << std::endl;

            errorDetected = true;
        }
        else
        {
            for (unsigned int i = 0; (i < _inputs.size()); i++)
            {
                if (_inputs[i].key_values.keyFormat != inputs_impl[i].key_values.keyFormat)
                {
                    cerr << ">>>     Error in key format at input # " << i << "in operation: " << name << std::endl;
                    cerr << ">>>     module declaration:       " << _inputs[i].key_values.keyFormat << std::endl;
                    cerr << ">>>     operation implementation: " << inputs_impl[i].key_values.keyFormat << std::endl;
                    cerr << ">>>" << std::endl;
                    errorDetected = true;
                }
                if (_inputs[i].key_values.valueFormat != inputs_impl[i].key_values.valueFormat)
                {
                    cerr << ">>>     Error in value format at input # " << i << "in operation: " << name << std::endl;
                    cerr << ">>>     module declaration:       " << _inputs[i].key_values.valueFormat << std::endl;
                    cerr << ">>>     operation implementation: " << inputs_impl[i].key_values.valueFormat << std::endl;
                    cerr << ">>>" << std::endl;
                    errorDetected = true;
                }
                if (_inputs[i].compareKeyFunction != inputs_impl[i].compareKeyFunction)
                {
                    cerr << ">>>     Error in compareKeyFunction at input # " << i << "in operation: " << name << std::endl;
                    cerr << ">>>     module declaration:       " << _inputs[i].compareKeyFunction << std::endl;
                    cerr << ">>>     operation implementation: " << inputs_impl[i].compareKeyFunction << std::endl;
                    cerr << ">>>" << std::endl;
                    errorDetected = true;
                }
                if (_inputs[i].compareValueFunction != inputs_impl[i].compareValueFunction)
                {
                    cerr << ">>>     Error in compareValueFunction at input # " << i << "in operation: " << name << std::endl;
                    cerr << ">>>     module declaration:       " << _inputs[i].compareValueFunction << std::endl;
                    cerr << ">>>     operation implementation: " << inputs_impl[i].compareValueFunction << std::endl;
                    cerr << ">>>" << std::endl;
                    errorDetected = true;
                }

            }
        }
        if (_outputs.size() != outputs_impl.size() )
        {
            cerr << ">>> Error in number of outputs to the operation: "  << name << std::endl;
            cerr << ">>>       module declaration: " << _outputs.size() << std::endl;
            cerr << ">>>       .h implementation: " << outputs_impl.size() << std::endl;
            cerr << ">>>" << std::endl;

            errorDetected = true;
        }
        else
        {
            for (unsigned int i = 0; (i < _outputs.size()); i++)
            {
                if (_outputs[i].key_values.keyFormat != outputs_impl[i].key_values.keyFormat)
                {
                    cerr << ">>>     Error in key format at output # " << i << "in operation: " << name << std::endl;
                    cerr << ">>>     module declaration:       " << _outputs[i].key_values.keyFormat << std::endl;
                    cerr << ">>>     operation implementation: " << outputs_impl[i].key_values.keyFormat << std::endl;
                    cerr << ">>>" << std::endl;
                    errorDetected = true;
                }
                if (_outputs[i].key_values.valueFormat != outputs_impl[i].key_values.valueFormat)
                {
                    cerr << ">>>     Error in value format at output # " << i << "in operation: " << name << std::endl;
                    cerr << ">>>     module declaration:       " << _outputs[i].key_values.valueFormat << std::endl;
                    cerr << ">>>     operation implementation: " << outputs_impl[i].key_values.valueFormat << std::endl;
                    cerr << ">>>" << std::endl;
                    errorDetected = true;
                }
            }
        }




        if (errorDetected == true)
        {
            cerr << ">>> " << "Changed INFO_MODULE header" << std::endl;
            cerr << ">>> " << "In .h:" << std::endl;

            for (unsigned int i = 0; (i < inputs_impl.size()); i++)
            {
                cerr << ">>>     //  " << "input: " << inputs_impl[i].key_values.keyFormat << " " << inputs_impl[i].key_values.valueFormat << " compareKeyFunction=" << inputs_impl[i].compareKeyFunction << " compareValueFunction=" << inputs_impl[i].compareValueFunction << std::endl;
            }

            for (unsigned int i = 0; (i < outputs_impl.size()); i++)
            {
                cerr << ">>>     //  " << "output: " << outputs_impl[i].key_values.keyFormat << " " << outputs_impl[i].key_values.valueFormat << std::endl;
            }

            cerr << ">>> " << "Expected: " << std::endl;
            size_t found_prev = 0;
            size_t found=info.find_first_of("\n");
            std::string line;

            while (found!=string::npos)
            {
                line = info.substr(found_prev, found - found_prev);
                if ((line.find("input:") != string::npos) || (line.find("output:") != string::npos))
                {
                    cerr << ">>>     //  " << line << std::endl;
                }
                found_prev = found+1;
                found=info.find_first_of("\n", found+1);
            }
            cerr << ">>> " << std::endl;
            return false;
        }
        else
        {
            return true;
        }

    }


    bool checkOperationFile(std::ifstream &file, std::string info)
    {


        if( !file.is_open() )
        {
            std::cerr << "checkOperationFile: file should already be opened" << "\n";
            exit(-1);
        }

        ostringstream o;
        char buffer[1001];
        bool header_info = false;
        bool info_comment = false;
        std::string infoFile;

        std::vector <DataSet> inputs_impl;
        std::vector <DataSet> outputs_impl;

        while (!file.eof())
        {
            file.getline( buffer , 1000);
            //cout << "Parsing:'" << buffer << std::endl;

            if ((strstr(buffer, INFO_MODULE) != NULL) || (strstr(buffer, INFO_COMMENT) != NULL))
            {
                //cout << "Detected header" << std::endl;
                if (strstr(buffer, INFO_COMMENT) != NULL)
                {
                    info_comment = true;
                }
                header_info = true;
            }
            else if (header_info)
            {
                if ((strstr(buffer, END_INFO_MODULE)) || (strstr(buffer, END_INFO_COMMENT) != NULL))
                {
                    //cout << "Detected end of header" << std::endl;
                    //cout << "Checking: info'" << info << std::endl;

                    return checkOperationHeaderItems(inputs, outputs, info, inputs_impl, outputs_impl, infoFile);
                }

                // We create a mini tokenize
                std::vector<AUToken> total_items = AUTockenizer::tockenize(buffer);

                std::vector<AUToken> items_impl = AUTockenizer::removeSpacesAndCommentsAndReturns( total_items );

                if (items_impl.size() < 3)
                {
                    continue;
                }

                unsigned int pos = 0;
                std::string option = items_impl[pos++].str;

                if (info_comment == false)
                {
                    if (option != "//")
                    {
                        continue;
                    }
                    else
                    {
                        option = items_impl[pos++].str;
                    }
                }

                if ((option == "input:") || (option == "in"))
                {
                    //cout << "detected input" << std::endl;
                    std::string keyFormat = items_impl[pos++].str;
                    std::string valueFormat = items_impl[pos++].str;

                    std::string compareKeyFunction = "";
                    std::string compareValueFunction = "";

                    if (pos < items_impl.size())
                    {
                        std::string optional_compareKeyFunction = items_impl[pos++].str;

#define COMPAREKEYFUNCTION "compareKeyFunction="
#define COMPAREVALUEFUNCTION "compareValueFunction="

                        if (optional_compareKeyFunction.compare(0,strlen(COMPAREKEYFUNCTION),COMPAREKEYFUNCTION) == 0)
                        {
                            size_t pos_equal = optional_compareKeyFunction.find("=");
                            pos_equal++;
                            compareKeyFunction = optional_compareKeyFunction.substr(pos_equal);
                        }
                        else
                        {
                            pos--;
                            optional_compareKeyFunction = "";
                            compareKeyFunction = "";
                        }
                    }


                    if (pos < items_impl.size())
                    {
                        std::string optional_compareValueFunction = items_impl[pos++].str;

                        if (optional_compareValueFunction.compare(0,strlen(COMPAREVALUEFUNCTION),COMPAREVALUEFUNCTION) == 0)
                        {
                            size_t pos_equal = optional_compareValueFunction.find("=");
                            pos_equal++;
                            compareValueFunction = optional_compareValueFunction.substr(pos_equal);
                        }
                        else
                        {
                            pos--;
                            optional_compareValueFunction = "";
                            compareValueFunction = "";
                        }
                    }

                    //cout << "added input" << std::endl;
                    DataSet dataset(KVFormat::format(keyFormat, valueFormat), compareKeyFunction, compareValueFunction);
                    inputs_impl.push_back(dataset);
                }
                else if ((option == "output:") || (option == "out"))
                {
                    std::string keyFormat = items_impl[pos++].str;
                    std::string valueFormat = items_impl[pos++].str;

                    DataSet dataset(KVFormat::format(keyFormat, valueFormat));
                    outputs_impl.push_back(dataset);
                }
            }
        }

        return true;
    }

};
}


