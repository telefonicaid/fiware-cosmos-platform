/* ****************************************************************************
 *
 * FILE                     DataType.h
 *
 * DESCRIPTION				Definition of a basic data-type
 *
 * ***************************************************************************/

#pragma once



#include <string>
#include <sstream>
#include <assert.h>
#include <vector>
#include <iostream>

using namespace std;

namespace ss {

	// Basic operations with "." separated names
	std::vector<std::string> tockenizeWithDots( std::string myString );
	std::string getModuleFromFullName( std::string fullName );
	std::string getNameFromFullName( std::string fullName );
	
	class DataType
	{
	public:
		
		string fullType;
		string module;
		string type;
		string name;
		bool vector;
		
		DataType( std::string _full_type , std::string _name , bool _vector  )
		{
			vector = _vector;
			fullType = _full_type;
			module = getModuleFromFullName( _full_type );
			type = getNameFromFullName( _full_type );
			
			if( tockenizeWithDots(fullType).size() <= 1)
			{
				std::cerr << "Error: Please specify a full data-type name (ex sna.Link). Input: " << fullType << "\n";
				exit(1);
			}
			
			name = _name;
		}
		
		
		/**
		 Get the inlude file that is necessary to use this data type
		 */
		
		std::string getInclude()
		{
			std::ostringstream output;
			output << "<samson/modules/" << module << "/" << type << ".h>";
			return output.str();
		}
		
		
		/** 
		 Function to give us the name of a particular class
		 */
		
		string classNameForType(  )
		{
			ostringstream o;

			std::vector<std::string> tockens = tockenizeWithDots( fullType );
			o << "::ss::";
			for (size_t i = 0 ; i <= tockens.size()-2 ; i++)
				o << tockens[i] << "::";
			o << tockens[tockens.size()-1];	// Last element
			
			return o.str();
		}
		
		/* Function to show the declaration of the field */
		
		string getDeclaration(string pre_line)
		{
			ostringstream o;
			
			if( vector )
			{	
				// Basically a pointer to hold the elements
				o << pre_line << classNameForType(  ) << " *" << name << ";\n";
				
				o << pre_line << "int " << name << "_length;\n";			//Current lenght of the element
				o << pre_line << "int " << name << "_max_length;\n";		//Max lengh of this vector
				
				return o.str();
			}
			
			//Simple types;
			o << pre_line << classNameForType(  ) << " " << name << ";\n";
			
			return o.str();
		}
		
		/* Set length function... only in vectors */
		
		string	getSetLengthFunction( string pre_line )
		{
			assert(vector);
			ostringstream o;
			
			o << pre_line << "void " << name <<"SetLength(int _length){\n";
			o << pre_line << "\tif( _length > " << name << "_max_length){ \n";
			
			{
				o << pre_line << "\t\t" << classNameForType() << " *_previous = " << name << ";\n";
				o << pre_line << "\t\tint previous_length = " << name << "_length;\n";
				
				o << pre_line << "\t\tif(" << name << "_max_length == 0) " << name << "_max_length = _length;\n";
				o << pre_line << "\t\twhile(" << name << "_max_length < _length) "<< name << "_max_length *= 2;\n";
				o << pre_line << "\t\t" << name << " = new " << classNameForType(  ) << "[" << name << "_max_length ];\n";
				
				
				o << pre_line << "\t\tif( _previous ){\n";
				
				o << pre_line << "\t\t\tfor (int i = 0 ; i < previous_length ; i++)\n";
				o << pre_line << "\t\t\t\t"<<name<<"[i].copyFrom( &_previous[i] );\n";
				o << pre_line << "\t\t\tdelete[] _previous;\n";
				o << pre_line << "\t\t}\n";
				
				
			}
			
			o << pre_line << "\t}\n";
			o << pre_line << "\t" << name << "_length=_length;\n";
			o << pre_line << "}\n\n";
			
			
			return o.str();
		}
		
		
		/* Add element function... only in vectors */
		
		string	getAddFunction( string pre_line )
		{
			assert(vector);
			ostringstream o;
			
			o << pre_line <<  classNameForType() << "* " << name <<"Add(){\n";
			
			o << pre_line << "\t" << name << "SetLength( " << name << "_length + 1 );\n";
			
			o << pre_line << "\t" << "return &" << name << "["<<name << "_length-1];\n";
			
			o << pre_line << "}\n\n";
			
			return o.str();
		}	
		
		
		
		
		/* Initialization inside the constructor */
		
		string getInitialization(string pre_line)
		{
			ostringstream o;
			
			//Only requited in vectors
			if( vector )
			{	
				o << pre_line << name << "_length=0;\n";			//Current lenght of the element
				o << pre_line << name << "_max_length=0;\n";		//Max lengh of this vector
				o << pre_line << name << " = NULL;\n";				//Point to null
				return o.str();
			}
			
			return o.str();
		}	
		
		string getDestruction(string pre_line)
		{
			ostringstream o;
			
			//Only requited in vectors
			if( vector )
			{	
				//The same with a vector of classes
				o << pre_line << "if( " << name << " )\n";
				o << pre_line << "\tdelete[] " << name << " ;\n";
			}
			
			return o.str();
		}	
		
		string getParseCommandIndividual( string _name )
		{
			ostringstream o;
			o << "offset += " << _name << ".parse(data+offset);";
			return o.str();	
		}
		
		string getParseCommand(string pre_line)
		{
			ostringstream o;
			
			if( vector )
			{
				//First get the length of the vector...
				
				o << pre_line << "{ //Parsing vector "<<name<<"\n";
				o << pre_line << "\tsize_t _length;\n";
				o << pre_line << "\toffset += ss::staticVarIntParse( data+offset , &_length );\n";
				o << pre_line << " \t"<<name<<"SetLength( _length );\n";	
				o << pre_line << "\tfor (int i = 0 ; i < (int)"<<name<<"_length ; i++){\n";
				o << pre_line << "\t\t" << getParseCommandIndividual( name + "[i]" ) << "\n";
				o << pre_line << "\t}\n";
				o << pre_line << "}\n";
				
			}
			else
			{
				//Simple command to parse
				o << pre_line << getParseCommandIndividual(name) << "\n";
			}
			
			return o.str();
			
		}
		
		
		string getSerializationCommandIndividual( string _name)
		{
			ostringstream o;
			o << "offset += " << _name << ".serialize(data+offset);";
			return o.str();
		}
		
		string getSerializeCommand( string pre_line )
		{
			ostringstream o;
			
			if( vector )
			{			
				o << pre_line << "{ //Serialization vector "<<name<<"\n";
				o << pre_line << "\toffset += ss::staticVarIntSerialize( data+offset , "<< name <<"_length );\n";
				o << pre_line << "\tfor (int i = 0 ; i < (int)"<<name<<"_length ; i++){\n";
				o << pre_line << "\t\t" << getSerializationCommandIndividual( name + "[i]" ) << "\n";
				o << pre_line << "\t}\n";
				o << pre_line << "}\n";
				
			}
			else
				o << pre_line << getSerializationCommandIndividual(name) << "\n";
			
			return o.str();
			
		}
		
		string getSizeCommandIndividual( string _name)
		{
			ostringstream o;
			o << "offset += " << classNameForType() << "::size(data+offset);";
			return o.str();
		}
		
		
		string getSizeCommand(string pre_line)
		{
			ostringstream o;
			
			if( vector )
			{			
				o << pre_line << "{ //Getting size of vector "<<name<<"\n";
				o << pre_line << "\tsize_t _length;\n";
				o << pre_line << "\toffset += ss::staticVarIntParse( data+offset , &_length );\n";
				
				o << pre_line << "\t" << classNameForType() << " _tmp;\n";
				o << pre_line << "\tfor (int i = 0 ; i < (int)_length ; i++){\n";
				o << pre_line << "\t\t" << getSizeCommandIndividual( "_tmp" ) << "\n";
				o << pre_line << "\t}\n";
				
				o << pre_line << "}\n";
				
				return o.str();
			}
			else
				o << pre_line << getSizeCommandIndividual(name) << "\n";
			
			
			
			return o.str();
			
		}
		
		
		string getPartitionCommandIndividual(string _name)
		{
			ostringstream o;
			o << "return " << _name <<".hash(max_num_partitions);";
			return o.str();
		}
		
		
		string getPartitionCommand( string pre_line )
		{
			ostringstream o;
			
			
			if( vector )
			{
				o << pre_line <<  "if( " << name << "_length > 0 ){\n";
				o << pre_line << getPartitionCommandIndividual( name + "[0]" ) << "\n";
				o  << pre_line << "} else return 0;\n";
				
			}
			else
				o << pre_line << getPartitionCommandIndividual(name) << "\n";
			
			return o.str();
			
			//No basic type;
			assert(false);
			
			
		}
		
		
		string getCompareCommandIndividual( string pre_line ,  string _name )
		{
			ostringstream o;
			
			//Simple types;
			o << pre_line << "{ // comparing " << _name << "\n";
			o << pre_line << "\tint tmp = "<<classNameForType()<<"::compare(data1,data2,offset1 , offset2);\n";
			o << pre_line << "\tif( tmp != 0) return tmp;\n";
			o << pre_line << "}\n";	
			
			return o.str();
			
		}
		
		
		string getCompareCommand( string pre_line )
		{
			ostringstream o;
			
			if( vector )
			{			
				o << pre_line << "{ // Comparing vector " << name << "\n";
				
				o << pre_line << "\tsize_t _length1,_length2;\n";
				o << pre_line << "\t*offset1 += ss::staticVarIntParse( data1+(*offset1) , &_length1 );\n";
				o << pre_line << "\t*offset2 += ss::staticVarIntParse( data2+(*offset2) , &_length2 );\n";
				
				o << pre_line << "\tif( _length1 < _length2 ) return -1;\n";
				o << pre_line << "\tif( _length1 > _length2 ) return 1;\n";
				
				o << pre_line << "\tfor (int i = 0 ; i < (int)_length1 ; i++){\n";
				o << getCompareCommandIndividual( pre_line + "\t\t" , name + "[i]" );
				o << pre_line << "\t}\n";
				o << pre_line << "}\n";
				
			}
			else
				o << getCompareCommandIndividual(pre_line, name);
			
			return o.str();
			
		}
		
		string getToStringCommandIndividual(string _name)
		{
			ostringstream o;
			o << "o << " << _name << ".str();\n";
			return o.str();
		}
		
		string getToStringCommand( string pre_line )
		{
			
			ostringstream o;
			
			if( vector )
			{ 
				o << pre_line << "{// toString of vector " << name << "\n";
				o << pre_line << "\tfor(int i = 0 ; i < " << name << "_length ; i++){\n";
				o << pre_line << "\t\t" << getToStringCommandIndividual(name+"[i]");
				o << pre_line << "\t\t o << \" \";\n";
				o << pre_line << "\t}\n";
				o << pre_line << "}\n";
			}
			else
				o << pre_line << getToStringCommandIndividual(name) << "\n";
			
			return o.str();
			
		}
		
		
		// CopyFrom Command
		
		
		
		string getCopyFromCommandIndividual(string _name)
		{
			ostringstream o;
			o << _name << ".copyFrom(&other->"<<_name<<");\n";
			return o.str();
		}
		
		string getCopyFromCommand(string pre_line)
		{
			ostringstream o;
			
			
			if( vector )
			{
				o << pre_line << "\t{ // CopyFrom field " << name << "\n";
				o << pre_line << "\t\t" << name << "SetLength( other->" << name << "_length);\n";
				o << pre_line << "\t\tfor (int i = 0 ; i < " << name << "_length ; i++){\n";
				o << pre_line << "\t\t\t" << getCopyFromCommandIndividual(name + "[i]");
				o << pre_line << "\t\t}\n";
				
				o << pre_line << "\t}\n";
			}
			else
				o << pre_line << getCopyFromCommandIndividual(name);
			
			return o.str();
		}
	};
}


