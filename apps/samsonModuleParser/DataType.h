

#include <string>
#include <sstream>
#include <assert.h>

using namespace std;

#ifndef _H_DataType
#define _H_DataType

namespace ss {

	class DataType
	{
	public:
		
		string name;
		string type;
		bool vector;
		
		DataType()
		{
		}
		
		bool isBasicTypeConstantLength( )
		{
			if( type == "uint64" || type == "uint32" || type == "uint8" 
			||  type == "int32"  || type == "int8"   || type == "double" 
			||  type == "float"  )    
				return true;
			else
				return false;
			
		}
		
		
		bool isBasicType( )
		{
			if(	type == "uint" 
			   ||  type == "uint64" || type == "uint32" || type == "uint8" 
			   ||  type == "int32" || type == "int8" || type == "double" 
			   ||  type == "float" )    
				return true;
			else
				return false;
			
		}
		
		
		/** 
		 Function to give us the name of a particular class
		 */
		
		static string classNameForType( string type )
		{
			ostringstream o;

			size_t pos = type.find(".",0);
			size_t last_pos = 0;
			if( pos != std::string::npos )
			{
				ostringstream o;
				o << "::ss::";
				while (pos != std::string::npos)
				{
					o << type.substr(last_pos, pos-last_pos) << "::";
					last_pos = pos+1;
					pos = type.find(".",last_pos);
				}
				o << type.substr(last_pos, type.length()-last_pos );
				return o.str();
			}
			else
				return type;	// Inside this namespace
				
			
		}
		
		/* Function to show the declaration of the field */
		
		string getDeclaration(string pre_line)
		{
			ostringstream o;
			
			if( vector )
			{	
				// Basically a pointer to hold the elements
				if( isBasicType() )    
					o << pre_line << "ss_" << type << " *" << name << ";\n";
				else
					o << pre_line << classNameForType( type ) << " *" << name << ";\n";
				
				o << pre_line << "int " << name << "_length;\n";			//Current lenght of the element
				o << pre_line << "int " << name << "_max_length;\n";		//Max lengh of this vector
				
				return o.str();
			}
			
			//Simple types;
			if( isBasicType() )    
				o << pre_line << "ss_" << type << " " << name << ";\n";
			else
				o << pre_line << classNameForType( type ) << " " << name << ";\n";
			
			return o.str();
		}
		
		/* Set length function... only in vectors */
		
		string	getSetLengthFunction( string pre_line )
		{
			assert(vector);
			ostringstream o;
			
			o << pre_line << "void " << name <<"SetLength(int _length){\n";
			o << pre_line << "\tif( _length > " << name << "_max_length){ \n";
			
			if( isBasicType() )
			{
				o << pre_line << "\t\tss_" << type << " *_previous = " << name << ";\n";
				o << pre_line << "\t\tint previous_length = " << name << "_length;\n";
				
				o << pre_line << "\t\tif(" << name << "_max_length == 0) " << name << "_max_length = _length;\n";
				o << pre_line << "\t\twhile(" << name << "_max_length < _length) "<< name << "_max_length *= 2;\n";
				o << pre_line << "\t\t" << name << " = (ss_" << type << "*) malloc( sizeof(ss_" << type << ") * " << name << "_max_length );\n";
				
				o << pre_line << "\t\tif( _previous ){\n";
				o << pre_line << "\t\t\tmemcpy("<<name<<", _previous, previous_length*sizeof(ss_"<<type<<"));\n";
				o << pre_line << "\t\t\tfree( _previous );\n";
				o << pre_line << "\t\t}\n";
				
			}
			else
			{
				o << pre_line << "\t\t" << classNameForType(type) << " *_previous = " << name << ";\n";
				o << pre_line << "\t\tint previous_length = " << name << "_length;\n";
				
				o << pre_line << "\t\tif(" << name << "_max_length == 0) " << name << "_max_length = _length;\n";
				o << pre_line << "\t\twhile(" << name << "_max_length < _length) "<< name << "_max_length *= 2;\n";
				o << pre_line << "\t\t" << name << " = new " << classNameForType( type ) << "[" << name << "_max_length ];\n";
				
				
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
			
			if( isBasicType() )
				o << pre_line << "void " << name <<"Add(ss_" << type << " _value){\n";
			else
				o << pre_line <<  classNameForType(type) << "* " << name <<"Add(){\n";
			
			o << pre_line << "\t" << name << "SetLength( " << name << "_length + 1 );\n";
			
			if( isBasicType() )
				o << pre_line << "\t" << name << "["<<name<<"_length-1] = _value;\n";
			else
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
				if( isBasicType() )
				{
					o << pre_line << "if( " << name << " )\n";
					o << pre_line << "\tfree( " << name << " );\n";
				}
				else
				{
					//The same with a vector of classes
					o << pre_line << "if( " << name << " )\n";
					o << pre_line << "\tdelete[] " << name << " ;\n";
				}
			}
			
			return o.str();
		}	
		
		string getParseCommandIndividual( string _name )
		{
			ostringstream o;
			
			if( isBasicType() )    
			{
				//Special case
				if( type == "uint" )
				{
					o << "offset += ss::staticVarIntParse( data+offset , &"<<_name<<");";
					return o.str();
				}
				
				o << _name << "=  *( (ss_"<< type <<"*) (data+offset) ); offset +=  sizeof(ss_" << type <<");";
			}
			else
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
			
			if( isBasicType() )    
			{
				if( type == "uint" )
				{
					o << "offset += ss::staticVarIntSerialize( data+offset , "<<_name<<");";
					return o.str();
				}
				
				o << "*( (ss_"<< type <<"*) (data+offset) )=" << _name << "; offset +=  sizeof(ss_" << type <<");";
			}
			else
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
			
			//Simple types;
			if( isBasicType() )    
			{
				//Special case
				if( type == "uint" )
				{
					o << "offset += ss::staticVarIntParse( data+offset , &"<<_name<<");";
					return o.str();
				}
				
				o << "offset += sizeof(ss_" << type <<");";
			}
			else
				o << "offset += " << _name << ".size(data+offset);";
			
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
				
				if( isBasicType() )
				{
					if( isBasicTypeConstantLength() )
					{
						o << pre_line << "\toffset += _length*sizeof(ss_"<<type<<");\n";
					}
					else
					{
						o << pre_line << "\tss_" << type << " _tmp;\n";
						o << pre_line << "\tfor (int i = 0 ; i < (int)_length ; i++){\n";
						o << pre_line << "\t\t" << getSizeCommandIndividual( "_tmp" ) << "\n";
						o << pre_line << "\t}\n";
					}
				}
				else
				{
					o << pre_line << "\t" << classNameForType(type) << " _tmp;\n";
					o << pre_line << "\tfor (int i = 0 ; i < (int)_length ; i++){\n";
					o << pre_line << "\t\t" << getSizeCommandIndividual( "_tmp" ) << "\n";
					o << pre_line << "\t}\n";
				}
				
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
			
			if( !isBasicType() )
			{
				o << "return " << _name <<".getPartition(max_num_partitions);";
				return o.str();
			}
			
			
			if(	type == "uint64" || type == "uint32" || type == "uint8" || type =="uint"  )
			{
				o << "return " <<_name<<"%max_num_partitions;";
				return o.str();
			}
			
			if(	 type == "int32" || type == "int8" )
			{
				o << "return " << _name << "<0? (-" << _name << ")%max_num_partitions : "<<_name<<"%max_num_partitions;";
				return o.str();
			}
			
			if( type == "double" || type == "float" )    
			{
				o << "return abs( (int) ("<<_name<<"*100))%max_num_partitions;";
				return o.str();
			}
			
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
			if( isBasicType() )    
			{
				
				if( type == "uint")
				{
					//Spetial case
					o << pre_line << "\tss_uint _value1;\n";
					o << pre_line << "\tss_uint _value2;\n";
					o << pre_line << "\t*offset1 += ss::staticVarIntParse(data1 + (*offset1), &_value1);\n";
					o << pre_line << "\t*offset2 += ss::staticVarIntParse(data2 + (*offset2), &_value2);\n";
					o << pre_line << "\tif( _value1 < _value2 ) return -1;\n";
					o << pre_line << "\tif( _value1 > _value2 ) return  1;\n";
					
				}
				else
				{
					o << pre_line << "\tif  ( *((ss_"<< type <<"*)(data1 + (*offset1))) < *((ss_"<< type <<"*)(data2 + (*offset2))) ) return -1;\n";
					o << pre_line << "\tif  ( *((ss_"<< type <<"*)(data1 + (*offset1))) > *((ss_"<< type <<"*)(data2 + (*offset2))) ) return 1;\n";
					o << pre_line << "\t*offset1 +=sizeof(ss_" << type <<");\n";
					o << pre_line << "\t*offset2 +=sizeof(ss_" << type <<");\n";
				}
				
			}
			else
			{
				o << pre_line << "\t" << classNameForType(type) << " _tmp_data;\n";
				o << pre_line << "\tint tmp = _tmp_data.compare(data1,data2,offset1 , offset2);\n";
				o << pre_line << "\tif( tmp != 0) return tmp;\n";
			}
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
			
			if( isBasicType() )    
			{
				if (( type == "uint8") ||( type == "int8")) 	//Spcieal case
					o << "o << (int)" << _name << ";\n";
				else
					o << "o << " << _name << ";\n";
			}
			else
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
			
			if( isBasicType() )    
				o <<  _name << "=other->" << _name << ";\n";
			else
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
#endif

