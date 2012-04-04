
#ifndef _H_SAMSON_SYSTEM_VALUE_Value
#define _H_SAMSON_SYSTEM_VALUE_Value

#include <string.h>
#include <cctype>
#include "smaz.h"
#include "logMsg/logMsg.h"
#include "au/Pool.h"
#include "samson/module/DataInstance.h"

#define VALUE_CODE 1219561887489248771ULL

namespace samson{
    namespace system{
        
        // Constant word serialization
        int get_constant_word_code( const char * word );
        const char* get_constant_word( int c );

        
        class Value : public samson::DataInstance 
        {
            // Pool of Vaues for vector and map
            static au::Pool<Value> pool_values;
            
        public:
            
            // How data is serialized
            typedef enum 
            {
                // Serialitzation of <void>
                // ------------------------------------------------------------
                ser_void,

                // Serialitzation of numbers
                // ------------------------------------------------------------
                ser_int_positive, // Variable length possitive numbers
                ser_int_negative, // Variable length negative numbers
                ser_int_value_0,  // Concrete values
                ser_int_value_1,
                ser_int_value_2,
                ser_int_value_3,
                ser_int_value_4,
                ser_int_value_5,
                ser_int_value_6,
                ser_int_value_7,
                ser_int_value_8,
                ser_int_value_9,
                ser_int_value_10,
                ser_int_value_minus_1,
                
                ser_double_positive_1_decimal,       // Double possitve or negative with a fixed number of decimals
                ser_double_positive_2_decimal,      
                ser_double_positive_3_decimal,      
                ser_double_positive_4_decimal,      
                ser_double_positive_5_decimal,      
                
                ser_double_negative_1_decimal,      
                ser_double_negative_2_decimal,      
                ser_double_negative_3_decimal,      
                ser_double_negative_4_decimal,      
                ser_double_negative_5_decimal,      

                ser_double,       // Generic double otherwise

                // Serialitzation of string
                // ------------------------------------------------------------
                ser_string,
                ser_string_constant,   // Constant words frequently used ( user, log, url, ...)
                ser_string_smaz,       // Compressed using smaz
                  
                // Serialitzation of vector
                // ------------------------------------------------------------
                ser_vector,
                ser_vector_len_2, // Vector with a particular length
                ser_vector_len_3,
                ser_vector_len_4,
                ser_vector_len_5,

                // Serialitzation of map
                // ------------------------------------------------------------
                ser_map,
                ser_map_len_1,    // Map with a particular number of elements inside
                ser_map_len_2,
                ser_map_len_3,
                ser_map_len_4,
                ser_map_len_5,
                
            } SerialitzationCode;
            
        private:
            
            // Value types ( on memory )
            typedef enum 
            {
                value_void,        // No content

                value_number,      // Generic number content  ( using _value_double )
                value_string,      // Generic string content  ( using _value_string )
                value_vector,      // A vector of values      ( using _value_vector )
                value_map,         // A map of values         ( using _value_map    )

                
            } ValueType;
            
            // Global value type
            // ------------------------------------------------------------
            ValueType value_type;

            // Internal representation of the value
            // ------------------------------------------------------------
            
            // Number
            double _value_double;
            
            // String
            std::string _value_string;
            
            // Vector
            std::vector<Value*> _value_vector;

            // Map
            au::map<std::string,Value> _value_map;
            
        public:
            
            Value() : samson::DataInstance()
            {    
            }
            
            ~Value() {
            }
            
            std::string getName()
            {
                return "system.Value";
            }
            
            // ------------------------------------------------------------
            // PARSE Operations
            // ------------------------------------------------------------
            
            inline int parse_void( char* data )
            {
                change_value_type( value_void );
                return 1; // Void is always serialized in 1 byte
            }
                        
            inline int parse_number( char*data )
            {
                SerialitzationCode code = (SerialitzationCode)data[0];
                
                // Common init to value int
                change_value_type( value_number );
                
                switch (code) 
                {
                    // Constant values
                    case ser_int_value_0:
                        _value_double = 0;
                        return 1; // Codified in the serialization code
                        break;
                    case ser_int_value_1:
                        _value_double = 1;
                        return 1; // Codified in the serialization code
                        break;
                    case ser_int_value_2:
                        _value_double = 2;
                        return 1; // Codified in the serialization code
                        break;
                    case ser_int_value_3:
                        _value_double = 3;
                        return 1; // Codified in the serialization code
                        break;
                    case ser_int_value_4:
                        _value_double = 4;
                        return 1; // Codified in the serialization code
                        break;
                    case ser_int_value_5:
                        _value_double = 5;
                        return 1; // Codified in the serialization code
                        break;
                    case ser_int_value_6:
                        _value_double = 6;
                        return 1; // Codified in the serialization code
                        break;
                    case ser_int_value_7:
                        _value_double = 7;
                        return 1; // Codified in the serialization code
                        break;
                    case ser_int_value_8:
                        _value_double = 8;
                        return 1; // Codified in the serialization code
                        break;
                    case ser_int_value_9:
                        _value_double = 9;
                        return 1; // Codified in the serialization code
                        break;
                    case ser_int_value_10:
                        _value_double = 10;
                        return 1; // Codified in the serialization code
                        break;
                    case ser_int_value_minus_1:
                        _value_double = -1;
                        return 1; // Codified in the serialization code
                        break;
                        
                    case ser_int_positive:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = tmp;
                        return total;
                    }
                        
                    case ser_int_negative:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = -tmp;
                        return total;
                    }
                        
                    case ser_double_positive_1_decimal:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = (double)tmp/10.0;
                        return total;
                    }
                    case ser_double_positive_2_decimal:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = (double)tmp/100.0;
                        return total;
                    }
                    case ser_double_positive_3_decimal:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = (double)tmp/1000.0;
                        return total;
                    }
                    case ser_double_positive_4_decimal:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = (double)tmp/10000.0;
                        return total;
                    }
                    case ser_double_positive_5_decimal:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = (double)tmp/100000.0;
                        return total;
                    }

                    case ser_double_negative_1_decimal:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = -(double)tmp/10.0;
                        return total;
                    }
                    case ser_double_negative_2_decimal:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = -(double)tmp/100.0;
                        return total;
                    }
                    case ser_double_negative_3_decimal:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = -(double)tmp/1000.0;
                        return total;
                    }
                    case ser_double_negative_4_decimal:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = -(double)tmp/10000.0;
                        return total;
                    }
                    case ser_double_negative_5_decimal:
                    {
                        size_t tmp;
                        int total = 1 + samson::staticVarIntParse( data + 1 , &tmp);
                        _value_double = -(double)tmp/100000.0;
                        return total;
                    }
                        
                        
                    case ser_double:
                        _value_double = *( (double*) (data+1) );
                        return 1 + sizeof(double);

                    default:
                        LM_X(1, ("Internal error"));
                        return 0;
                        
                }
                
                LM_X(1, ("Internal error"));
                return 0;
                
            }            
            
            inline int parse_string( char*data )
            {
                //printf("Parsing string %p\n" , data);
                
                SerialitzationCode code = (SerialitzationCode)data[0];
                
                // Common init to value int
                change_value_type( value_string );
                
                switch (code) 
                {
                    case ser_string:
                        _value_string = &data[1];
                        return  1 + _value_string.length() +1 ; // serializtion code, string, '\0'

                    case ser_string_constant:
                        _value_string = get_constant_word( ((unsigned char) data[1]) );
                        return 2; // Always length 2 ( serialization char and word index )
                        
                    case ser_string_smaz:
                    {
                        char line[8192];
                        
                        // We skip the serialization char
                        size_t offset = 1;
                        
                        size_t len_in = 0;
                        offset += samson::staticVarIntParse( data + offset , &len_in);
                        
                        int len = smaz_decompress( data + offset , len_in, (char*)line, 8192 );
                        
                        //printf("Parsing compress string length %lu // Original string length %d\n" , len_in , len );
                        
                        line[len] = '\0';
                        _value_string = line;

                        offset += len_in;
                        return offset;
                    }
                        
                    default:
                        LM_X(1, ("Internal error"));
                }
                
                LM_X(1, ("Internal error"));
                return 0;
                
            }
            
            inline int parse_vector( char *data )
            {
                //printf("Parsing vector %p\n" , data);
                SerialitzationCode code = (SerialitzationCode)data[0];
                
                // Common init to value int
                set_as_vector();

                // Skip serialization code
                size_t offset = 1; 
                
                // Length of the vector ( decoded in different ways )
                size_t _length = 0;

                switch (code) 
                {
                    case ser_vector_len_2:
                        _length = 2;
                        break;
                    case ser_vector_len_3:
                        _length = 3;
                        break;
                    case ser_vector_len_4:
                        _length = 4;
                        break;
                    case ser_vector_len_5:
                        _length = 5;
                        break;
                    case ser_vector:
                    {
                        // Recover the number of elements we have serialized
                        offset += samson::staticVarIntParse( data + offset , &_length );                        
                        break;
                    }
                        
                    default:
                        LM_X(1, ("Internal error"));
                }

                // Parse all components of the vector
                for( size_t i = 0 ; i < _length ; i++ )
                {
                    Value * new_value = add_value_to_vector();
                    offset += new_value->parse( data + offset );
                }
                return offset;

                
                LM_X(1, ("Internal error"));
                return 0;
                
            }
            
            inline int parse_map( char *data )
            {
                //printf("Parsing vector %p\n" , data);
                SerialitzationCode code = (SerialitzationCode)data[0];
                
                // Common init to value int
                set_as_map();
                
                // Skip serialization code
                size_t offset = 1; 
                
                // Length of the vector ( decoded in different ways )
                size_t _length = 0;
                
                switch (code) 
                {
                    case ser_map_len_1:
                        _length = 2;
                    case ser_map_len_2:
                        _length = 2;
                        break;
                    case ser_map_len_3:
                        _length = 3;
                        break;
                    case ser_map_len_4:
                        _length = 4;
                        break;
                    case ser_map_len_5:
                        _length = 5;
                        break;
                    case ser_map:
                    {
                        // Recover the number of elements we have serialized
                        offset += samson::staticVarIntParse( data + offset , &_length );                        
                        break;
                    }
                        
                    default:
                        LM_X(1, ("Internal error"));
                }
                
                // Parse all components of the vector
                Value tmp_key; // Component to parse keys...
                for( size_t i = 0 ; i < _length ; i++ )
                {
                    // Parse key
                    offset += tmp_key.parse( data + offset );
                    
                    Value * new_value = add_value_to_map( tmp_key.get_string() );
                    offset += new_value->parse( data + offset );
                }
                return offset;
                
                
                LM_X(1, ("Internal error"));
                return 0;
                
            }

            
            int parse( char *data )
            {
                // Get serialization code
                SerialitzationCode code = (SerialitzationCode)data[0];

                switch (code) 
                {
                    case ser_void:
                        return parse_void(data);
                        
                    case ser_int_positive:
                    case ser_int_negative:
                    case ser_int_value_0:
                    case ser_int_value_1:
                    case ser_int_value_2:
                    case ser_int_value_3:
                    case ser_int_value_4:
                    case ser_int_value_5:
                    case ser_int_value_6:
                    case ser_int_value_7:
                    case ser_int_value_8:
                    case ser_int_value_9:
                    case ser_int_value_10:
                    case ser_int_value_minus_1:
                    case ser_double:
                    case ser_double_positive_1_decimal:
                    case ser_double_positive_2_decimal:
                    case ser_double_positive_3_decimal:
                    case ser_double_positive_4_decimal:
                    case ser_double_positive_5_decimal:
                    case ser_double_negative_1_decimal:
                    case ser_double_negative_2_decimal:
                    case ser_double_negative_3_decimal:
                    case ser_double_negative_4_decimal:
                    case ser_double_negative_5_decimal:
                        return parse_number(data);
                        
                    case ser_string:
                    case ser_string_constant:
                    case ser_string_smaz:
                        return parse_string(data);

                    case ser_vector:
                    case ser_vector_len_2:
                    case ser_vector_len_3:
                    case ser_vector_len_4:
                    case ser_vector_len_5:
                        return parse_vector(data);
                        
                    case ser_map:
                    case ser_map_len_1:
                    case ser_map_len_2:
                    case ser_map_len_3:
                    case ser_map_len_4:
                    case ser_map_len_5:
                        return parse_map(data);
                }
                
                LM_X(1, ("Internal error"));
                return 0;
                
            }
            
            // ------------------------------------------------------------
            // SERIALIZE Operations
            // ------------------------------------------------------------


            int serialize_void(char *data)
            {
                data[0] = (char) ser_void;
                return 1;
            }
            

            int serialize_number(char *data)
            {
                
                if( _value_double == 0 )
                {
                    data[0] = (char) ser_int_value_0;
                    return 1; // Codified in 1 byte
                }
                if( _value_double == 1 )
                {
                    data[0] = (char) ser_int_value_1;
                    return 1; // Codified in 1 byte
                }
                if( _value_double == 2 )
                {
                    data[0] = (char) ser_int_value_2;
                    return 1; // Codified in 1 byte
                }
                if( _value_double == 3 )
                {
                    data[0] = (char) ser_int_value_3;
                    return 1; // Codified in 1 byte
                }
                if( _value_double == 4 )
                {
                    data[0] = (char) ser_int_value_4;
                    return 1; // Codified in 1 byte
                }
                if( _value_double == 5 )
                {
                    data[0] = (char) ser_int_value_5;
                    return 1; // Codified in 1 byte
                }
                if( _value_double == 6 )
                {
                    data[0] = (char) ser_int_value_6;
                    return 1; // Codified in 1 byte
                }
                if( _value_double == 7 )
                {
                    data[0] = (char) ser_int_value_7;
                    return 1; // Codified in 1 byte
                }
                if( _value_double == 8 )
                {
                    data[0] = (char) ser_int_value_8;
                    return 1; // Codified in 1 byte
                }
                if( _value_double == 9 )
                {
                    data[0] = (char) ser_int_value_9;
                    return 1; // Codified in 1 byte
                }
                if( _value_double == 10 )
                {
                    data[0] = (char) ser_int_value_10;
                    return 1; // Codified in 1 byte
                }
                
                if( _value_double == -1 )
                {
                    data[0] = (char) ser_int_value_minus_1;
                    return 1; // Codified in 1 byte
                }
                
                // Integuer numbers...
                // Generic variable lengh codification
                if( (double) ((ssize_t) _value_double) == _value_double )
                {
                    if( _value_double >= 0 )
                    {
                        data[0] = (char) ser_int_positive;
                        return 1 + samson::staticVarIntSerialize( data + 1 , _value_double );
                    }
                    else
                    {
                        data[0] = (char) ser_int_negative;
                        return 1 + samson::staticVarIntSerialize( data + 1 , - _value_double );
                    }
                }
                
                // Limited number of decimals
                
                if( (double) ( (double) ( (ssize_t) (_value_double*10.0) ) / 10.0 ) == _value_double )
                {
                    if( _value_double >= 0 )
                    {
                        data[0] = (char) ser_double_positive_1_decimal;
                        return 1 + samson::staticVarIntSerialize( data + 1 , _value_double*10 );
                    }
                    else
                    {
                        data[0] = (char) ser_double_negative_1_decimal;
                        return 1 + samson::staticVarIntSerialize( data + 1 , - _value_double*10 );
                    }
                }

                if( (double) ( (double) ( (ssize_t) (_value_double*100.0) ) / 100.0 ) == _value_double )
                {
                    if( _value_double >= 0 )
                    {
                        data[0] = (char) ser_double_positive_2_decimal;
                        return 1 + samson::staticVarIntSerialize( data + 1 , _value_double*100 );
                    }
                    else
                    {
                        data[0] = (char) ser_double_negative_2_decimal;
                        return 1 + samson::staticVarIntSerialize( data + 1 , - _value_double*100 );
                    }
                }

                
                if( (double) ( (double) ( (ssize_t) (_value_double*1000.0) ) / 1000.0 ) == _value_double )
                {
                    if( _value_double >= 0 )
                    {
                        data[0] = (char) ser_double_positive_3_decimal;
                        return 1 + samson::staticVarIntSerialize( data + 1 , _value_double*1000 );
                    }
                    else
                    {
                        data[0] = (char) ser_double_negative_3_decimal;
                        return 1 + samson::staticVarIntSerialize( data + 1 , - _value_double*1000 );
                    }
                }
                if( (double) ( (double) ( (ssize_t) (_value_double*10000.0) ) / 10000.0 ) == _value_double )
                {
                    if( _value_double >= 0 )
                    {
                        data[0] = (char) ser_double_positive_4_decimal;
                        return 1 + samson::staticVarIntSerialize( data + 1 , _value_double*10000 );
                    }
                    else
                    {
                        data[0] = (char) ser_double_negative_4_decimal;
                        return 1 + samson::staticVarIntSerialize( data + 1 , - _value_double*10000 );
                    }
                }
                if( (double) ( (double) ( (ssize_t) (_value_double*100000.0) ) / 100000.0 ) == _value_double )
                {
                    if( _value_double >= 0 )
                    {
                        data[0] = (char) ser_double_positive_5_decimal;
                        return 1 + samson::staticVarIntSerialize( data + 1 , _value_double*100000 );
                    }
                    else
                    {
                        data[0] = (char) ser_double_negative_5_decimal;
                        return 1 + samson::staticVarIntSerialize( data + 1 , - _value_double*100000 );
                    }
                }

                
                // Generic double codification
                data[0] = (char) ser_double;
                *( (double*) (data+1) ) = _value_double;
                return 1 + sizeof( double );         
            }

            int serialize_string(char *data)
            {
                // If constant word, prefered method
                int index = get_constant_word_code( _value_string.c_str() );
                if( index != -1 )
                {
                    data[0] = (char) ser_string_constant;
                    data[1] = (unsigned char) index;
                    return 2; // Always 2 length serialization ( serialization code + index word )
                }
                
                if( _value_string.length() < 4096 )
                {
                    // Try compressed vertion
                    char line[8192];
                    size_t len = ::smaz_compress((char*) _value_string.c_str(), _value_string.length() , line, 8192);
                    
                    if( len < _value_string.length() )
                    {
                        // Serialize using compression
                        size_t offset = 0;
                        
                        data[offset] = (char) ser_string_smaz;
                        offset++;
                        
                        offset += samson::staticVarIntSerialize( data + offset , len );

                        memcpy( data+offset, line , len );
                        offset += len;
                        
                        //printf("Serialized string length %lu // Sent len %lu" , _value_string.length() , len  );
                        
                        return offset;
                    }
                    
                }

                // Default serialization of string
                data[0] = (char) ser_string;
                strcpy( data+1, _value_string.c_str() );
                int total  = 1 + ( _value_string.length() + 1 ); // Serialization code - strin - '\0'                        
                return total;
            }
            
            int serialize_vector(char *data)
            {
                if( _value_vector.size() == 0 )
                {
                    // If no elements, serialize as void
                    data[0] = (char) ser_void;
                    return 1;
                }
                
                if( _value_vector.size() == 1)
                {
                    // Serialize as a single element
                    return _value_vector[0]->serialize(data);
                }
                
                size_t offset = 1; // 
                if( _value_vector.size() == 2)
                    data[0] = (char) ser_vector_len_2;
                else if( _value_vector.size() == 3)
                    data[0] = (char) ser_vector_len_3;
                else if( _value_vector.size() == 4)
                    data[0] = (char) ser_vector_len_4;
                else if( _value_vector.size() == 5)
                    data[0] = (char) ser_vector_len_5;
                else
                {
                    data[0] = (char) ser_vector;
                    offset += samson::staticVarIntSerialize( data + offset , _value_vector.size() );
                }

                // Serialize all the components
                for ( size_t i = 0 ; i < _value_vector.size() ; i++ )
                {
                    Value * value = _value_vector[i];
                    offset += value->serialize( data + offset );
                }
                return offset;
            }

            int serialize_map( char *data )
            {
                if( _value_map.size() == 0 )
                {
                    // If no elements, serialize as void
                    data[0] = (char) ser_void;
                    return 1;
                }
                                
                size_t offset = 1; // 
                if( _value_vector.size() == 1)
                    data[0] = (char) ser_map_len_1;
                if( _value_vector.size() == 2)
                    data[0] = (char) ser_map_len_2;
                else if( _value_vector.size() == 3)
                    data[0] = (char) ser_map_len_3;
                else if( _value_vector.size() == 4)
                    data[0] = (char) ser_map_len_4;
                else if( _value_vector.size() == 5)
                    data[0] = (char) ser_map_len_5;
                else
                {
                    data[0] = (char) ser_map;
                    offset += samson::staticVarIntSerialize( data + offset , _value_map.size() );
                }
                
                // Serialize all the components
                Value tmp_key; // Value to serialize key
                au::map<std::string,Value>::iterator it;
                for( it = _value_map.begin() ; it != _value_map.end() ; it++ )
                {
                    tmp_key.set_string( it->first );
                    Value * value = it->second;
                    offset += tmp_key.serialize( data + offset );
                    offset += value->serialize( data + offset );
                }
                return offset;
            }
            
            
            int serialize(char *data)
            {
             
                //printf("Serialize %s\n" , str().c_str() );
                
                switch (value_type) 
                {
                    case value_void:
                        return serialize_void(data);
                        break;
                    case value_number:
                    {
                        int r = serialize_number(data);
                        //printf("serialize int '%s' in %d bytes\n" , str().c_str() , r );
                        //print_data( data , r );
                        return r;
                        break;
                    }
                    case value_string:
                    {
                        int r = serialize_string(data);
                        //printf("serialize string '%s' in %d bytes\n" , str().c_str(), r );
                        //print_data( data , r );
                        return r;
                        break;
                    }
                    case value_vector:
                    {
                        int r = serialize_vector(data);
                        //printf("serialize vector '%s' in %d bytes\n" , str().c_str(), r );
                        //print_data( data , r );
                        return r;
                        break;
                    }
                    case value_map:
                        return serialize_map(data);
                        break;
                        
                }
                
                LM_X(1, ("Internal error"));
                return 0;
            }
            
            // ------------------------------------------------------------
            // HASH Operations
            // ------------------------------------------------------------

            int hash_void(int max_num_partitions)
            {
                return 0;
            }

            int hash_number(int max_num_partitions)
            {
                if( _value_double >= 0 )
                    return ((size_t)_value_double)%max_num_partitions;
                else
                    return ((size_t)-_value_double)%max_num_partitions;
            }

            int hash_string(int max_num_partitions)
            {
                static const size_t InitialFNV = 2166136261U;
                static const size_t FNVMultiple = 16777619;
                
                size_t hash = InitialFNV;
                for(size_t i = 0; i < _value_string.length(); i++)
                {
                    hash = hash ^ (_value_string[i]);
                    hash = hash * FNVMultiple;     
                }
                return hash%max_num_partitions;                        
            }
            
            int hash_vector(int max_num_partitions)
            {
                if( _value_vector.size() == 0 )
                    return 0;
                return _value_vector[0]->hash(max_num_partitions);
            }
            
            int hash_map(int max_num_partitions)
            {
                return 0;
            }

            int hash(int max_num_partitions)
            {
                switch (value_type)
                {
                    case value_void:
                        return hash_void(max_num_partitions);
                    case value_number:
                        return hash_number(max_num_partitions);
                    case value_string:
                        return hash_string(max_num_partitions);
                    case value_vector:
                        return hash_vector(max_num_partitions);
                    case value_map:
                        return hash_map(max_num_partitions);
                        
                        
                }
                
                return 0;
            }
            
            // ------------------------------------------------------------
            // SIZE Operations
            // ------------------------------------------------------------
            
            static int size( char *data )
            {
                // Simple implementation...
                Value value;
                return value.parse( data );
            }
            
            // ------------------------------------------------------------
            // Compare
            // ------------------------------------------------------------

            bool operator==(const Value &other) const 
            {
                return compare(other) == 0;
            }
            
            bool operator!=(const Value &other) const 
            {
                return compare(other) != 0;
            }

            bool operator<=(const Value &other) const 
            {
                return compare(other) <= 0;
            }
            
            bool operator>=(const Value &other) const 
            {
                return compare(other) >= 0;
            }

            bool operator<(const Value &other) const 
            {
                return compare(other) < 0;
            }
            
            bool operator>(const Value &other) const 
            {
                return compare(other) > 0;
            }
            
            const Value operator+(const Value &other) const 
            {
                Value result = *this;       // Make a copy of myself. 

                
                switch (value_type) 
                {
                    case value_number:
                        result.set_double( getDouble() + other.getDouble() );
                        break;
                        
                    case value_string:
                        result.set_string( get_string() + other.get_string() );
                        break;
                        
                    case value_void:
                    case value_vector:
                    case value_map:
                        result.set_as_void();
                        break;
                }
                return result;              // All done!
            }

            const Value operator-(const Value &other) const 
            {
                Value result = *this;       // Make a copy of myself. 
                
                switch (value_type) 
                {
                    case value_number:
                        result.set_double( getDouble() - other.getDouble() );
                        break;
                        
                    case value_string:
                    case value_void:
                    case value_vector:
                    case value_map:
                        result.set_as_void();
                        break;
                }
                
                return result;              // All done!
            }

            const Value operator*(const Value &other) const 
            {
                Value result = *this;       // Make a copy of myself. 
                
                switch (value_type) 
                {
                    case value_number:
                        result.set_double( getDouble() * other.getDouble() );
                        break;
                        
                    case value_string:
                    case value_void:
                    case value_vector:
                    case value_map:
                        result.set_as_void();
                        break;
                }
                
                return result;              // All done!
            }

            const Value operator/(const Value &other) const 
            {
                Value result = *this;       // Make a copy of myself. 
                
                switch (value_type) 
                {
                    case value_number:
                        result.set_double( getDouble() / other.getDouble() );
                        break;
                        
                    case value_string:
                    case value_void:
                    case value_vector:
                    case value_map:
                        result.set_as_void();
                        break;
                }
                
                return result;              // All done!
            }

            
            inline int compare( const Value& other ) const
            {
                
                if( value_type != other.value_type )
                    return value_type - other.value_type;
                
                // Same type
                switch ( value_type ) 
                {
                    case value_void:
                        return 0;
                    case value_number:
                        if( _value_double > other._value_double )
                            return 1;
                        else if( _value_double < other._value_double )
                            return -1;
                        else
                            return 0;
                        
                    case value_string:
                    {
                        if( _value_string > other._value_string )
                            return 1;
                        else  if( _value_string < other._value_string )
                            return -1;
                        else
                            return 0;
                    }
                        
                    case value_vector:
                    {
                        if( _value_vector.size() != other._value_vector.size() )
                        {
                            if( _value_vector.size() > other._value_vector.size() )
                                return 1;
                            else
                                return -1;
                        }
                        
                        for ( size_t i = 0 ; i < _value_vector.size() ; i++ )
                        {
                            int c = _value_vector[i]->compare( *other._value_vector[i] );
                            if( c!=0 )
                                return c;
                        }
                        
                        return 0; // all the elements are the same.
                        
                    }
                        
                    case value_map:
                        LM_X(1, ("Unimplemenrted"));
                        
                        
                }
                
                LM_X(1, ("Internal error"));
                return 0;
            }
            
            // ------------------------------------------------------------
            // Data compare 
            // ------------------------------------------------------------
            
            inline static int compare( char* data1 , char* data2 , size_t* offset1 , size_t* offset2  )
            {
                Value tmp_value1;
                Value tmp_value2;
                
                size_t size1 = tmp_value1.parse( data1 + *offset1 );
                size_t size2 = tmp_value2.parse( data2 + *offset2 );
                
                *offset1 = *offset1 + size1;
                *offset1 = *offset1 + size2;
                

                return tmp_value1.compare( tmp_value2 );
                
            }

            inline static int compare( char* data1 , char* data2 )
            {
                size_t offset1=0;
                size_t offset2=0;
                
                return compare( data1, data2, &offset1 , &offset2 );
            }
            
            int serial_compare( char* data1 , char* data2 )
            {
                return compare( data1, data2 );
            }
            
            // ------------------------------------------------------------
            // Checks
            // ------------------------------------------------------------
            
            
            static const char *getTypeStatic()
            {
                return ("system.Value");
            }
            
            const char *getType()
            {
                return ("system.Value");
            }
            
            static bool checkTypeStatic(const char *type)
            {
                if (strcmp(type, "system.Value") == 0)
                    return true;
                return false;
            }
            
            bool checkType(const char *type)
            {
                if (strcmp(type, "system.Value") == 0)
                    return true;
                return false;
            }
            
            static size_t getHashTypeStatic()
            {
                return(VALUE_CODE);
            }
            
            size_t getHashType()
            {
                return(VALUE_CODE);
            }
            
            static bool checkHashTypeStatic(size_t valType)
            {
                if (valType == VALUE_CODE)
                {
                    return true;
                }		return false;
            }
            
            bool checkHashType(size_t valType)
            {
                if (valType == VALUE_CODE)
                {
                    return true;
                }		return false;
            }
            
            // ------------------------------------------------------------
            // Set and get functions 
            // ------------------------------------------------------------
            
            
            void setFromString( const char *_data )
            {
                change_value_type( value_string );
                _value_string = _data;
            }
            
            // ------------------------------------------------------------
            // COPY Function 
            // ------------------------------------------------------------

            
            void copyFrom( Value *other )
            {
                change_value_type( other->value_type );
                
                switch (value_type) 
                {
                    case value_void:
                        break;
                        
                    case value_number:
                        _value_double = other->_value_double;
                        break;
                        
                    case value_string:
                        _value_string = other->_value_string;
                        break;
                        
                    case value_vector:
                    {
                        set_as_vector();
                        for ( size_t i = 0 ; i < other->_value_vector.size() ; i++ )
                            add_value_to_vector()->copyFrom( other->_value_vector[i] );
                        break;
                    }
                        
                    case value_map:
                    {
                        set_as_map();
                        au::map<std::string,Value>::iterator it;
                        for( it = other->_value_map.begin() ; it != other->_value_map.end() ; it++ )
                            add_value_to_map(it->first)->copyFrom( it->second );
                    }
                        break;

                }
                
            };

            // ------------------------------------------------------------
            // STR Function 
            // ------------------------------------------------------------

            std::string str() 
            {
                switch (value_type) 
                {
                    case value_void:
                        return "<void>";
                        
                    case value_number:
                    {
                        std::ostringstream output;
                        output << _value_double;
                        return output.str();
                    }
                        
                    case value_string:
                        return "\"" + _value_string + "\"";
                        
                    case value_vector:
                    {
                        std::ostringstream output;
                        output << "[ ";
                        for( size_t i = 0 ; i < _value_vector.size() ; i++ )
                        {
                            output << _value_vector[i]->str();
                            output << " ";
                        }
                        output << "]";
                        return output.str();
                    }
                        
                    case value_map:
                    {
                        std::ostringstream output;
                        output << "{ ";
                        au::map<std::string,Value>::iterator it;
                        for( it = _value_map.begin() ; it != _value_map.end() ; it++ )
                        {
                            output << it->first << ":";
                            output << it->second->str();
                            output << " ";
                        }
                        output << "}";
                        return output.str();
                    }

                        
                }
                
                LM_X(1, ("Internal error"));
                return "unknown";
            }
            
            // ------------------------------------------------------------
            // JSON & XML Functions
            // ------------------------------------------------------------

            std::string strJSON(std::string _varNameInternal)
            {
                return strJSON();
            }
            
            std::string strJSON()
            {
                std::ostringstream output;
                _strJSON( output );
                return output.str();
                
            }
            
            void _strJSON( std::ostream &output )
            {
                switch (value_type) 
                {
                    case value_number:
                        output << _value_double;
                        break;
                    case value_string:
                        output << "\"" <<  _value_string << "\"";
                        break;
                    case value_vector:
                        output << "[";
                        for ( size_t i = 0 ; i < _value_vector.size() ; )
                        {
                            _value_vector[i]->_strJSON(output);
                            i++;
                            if ( i != _value_vector.size() )
                                output << ",";
                        }
                        output << "]";
                        break;
                    case value_map:
                    {
                        output << "{";
                        au::map<std::string,Value>::iterator it;
                        for( it = _value_map.begin() ; it != _value_map.end() ; it++ )
                        {
                            if ( it != _value_map.begin() )
                                output << ",";
                            
                            output << it->first << ":";
                            it->second->_strJSON(output);

                        }
                        output << "}";
                        break;
                    }
                    case value_void:
                        output << "null";
                        break;
                }
                
            }
            
            std::string strJSONInternal(std::string _varNameInternal, bool vectorMember)
            {
                return "TO BE DEFINED";
            }
            
            std::string strXML(std::string _varNameInternal)
            {
                return "TO BE DEFINED";
            }
            
            std::string strXMLInternal(std::string _varNameInternal)
            {
                return "TO BE DEFINED";
            }
            
            
            // ----------------------------------------------------------------------------------------
            // void functions
            // ----------------------------------------------------------------------------------------
            
            void set_as_void()
            {
                change_value_type( value_void );
            }
            
            // ----------------------------------------------------------------------------------------
            // Vector functions
            // ----------------------------------------------------------------------------------------
            
            bool isVector()
            {
                return value_type == value_vector;
            }
            
            void clear_vector()
            {
                for( size_t i = 0 ; i < _value_vector.size() ; i++ )
                    pool_values.push( _value_vector[i] );
                _value_vector.clear();
                
            }
            
            void set_as_vector()
            {
                change_value_type( value_vector );
            }
            
            Value* add_value_to_vector()
            {
                // Just make sure we are in vector mode
                if( value_type != value_vector )
                    set_as_vector();
                
                // Get a new instance of Value and push it to the vector
                Value* value = pool_values.pop();
                _value_vector.push_back( value );

                // Alwyas return a void obnject
                value->set_as_void();
                return value;
            }
            
            Value* get_value_from_vector( size_t pos )
            {
                if( value_type != value_vector )
                {
                    if( pos == 0 )
                        return this;
                    else
                        return NULL;
                }
                
                if( pos >= _value_vector.size() )
                    return NULL;
                else
                    return _value_vector[pos];
            }
            
            size_t get_vector_size()
            {
                return _value_vector.size();
            }
            
            // ----------------------------------------------------------------------------------------
            // Map functions
            // ----------------------------------------------------------------------------------------
            
            bool isMap()
            {
                return value_type == value_map;
            }

            void clear_map()
            {
                au::map<std::string,Value>::iterator it;
                for( it = _value_map.begin() ; it != _value_map.end() ; it++ )
                    pool_values.push( it->second );
                _value_map.clear();
            }

            void set_as_map()
            {
                change_value_type( value_map );
            }
            
            Value* add_value_to_map( std::string key )
            {
                // Just make sure we are in vector mode
                if( value_type != value_map )
                    set_as_map();
                                
                // Get a new instance of Value and push it to the vector
                Value* value = pool_values.pop();
                _value_map.insertInMap(key, value );
                
                // Alwyas return a void object
                value->set_as_void();
                return value;
            }

            Value* get_value_from_map( const char* key )
            {
                std::string _key = key;
                return _value_map.findInMap(_key);
            }
            
            Value* get_value_from_map( std::string& key )
            {
                return _value_map.findInMap(key);
            }
            
            
            std::vector<std::string> get_keys_from_map()
            {
                return _value_map.getKeysVector();
            }
            
            // ----------------------------------------------------------------------------------------
            // CONVERT functions
            // ----------------------------------------------------------------------------------------
            /*
            bool is_int( const char* data )
            {
                bool found_sign  = false;
                bool found_digit = false;
                
                size_t pos = 0;
                while( true )
                {
                    int c = data[pos];
                    pos++;
                    
                    if( c == '\0' )
                        return true;
                    
                    // Signs
                    if( ( c == '-' ) || ( c == '+' ) )
                    {
                        if( found_sign )
                            return false; // Double sign
                        if ( found_digit )
                            return false; // Sign after digits
                        found_sign = true;
                        continue;
                    }
                   
                    if( isdigit(c) )
                    {
                        found_digit = true;
                        continue;
                    }

                    return false;
                }

                LM_X(1,("Internal error"));
                return true;
            }
             
             */
            
            bool is_double( const char* data )
            {
                bool found_sign  = false;
                bool found_dot   = false;
                bool found_digit = false;
                
                size_t pos = 0;
                while( true )
                {
                    int c = data[pos];
                    pos++;
                    
                    if( c == '\0' )
                        return true;
                    
                    
                    // Dot
                    if( c == '.' )
                    {
                        found_dot = true;
                        continue;
                    }
                    
                    // Signs
                    if( ( c == '-' ) || ( c == '+' ) )
                    {
                        if( found_dot )
                            return false; // sign after dot
                        if( found_sign )
                            return false; // Double sign
                        if ( found_digit )
                            return false; // Sign after digits
                        found_sign = true;
                        continue;
                    }
                    
                    if( isdigit(c) )
                    {
                        found_digit = true;
                        continue;
                    }
                    
                    return false;
                }
                
                LM_X(1,("Internal error"));
                return true;
            }
            
            void convert_to_number()
            {
                switch (value_type) 
                {
                    case value_void:
                    case value_number:
                        // It is already a number
                        return;
                        
                    case value_vector:
                        set_double( 0 );
                        return;
                        
                    case value_map:
                        set_double( 0 );
                        return;
                        
                    case value_string:
                        set_double( atof( _value_string.c_str() ));
                        return;
                }
            }
            
            void convert_to_string()
            {
                if( value_type !=  value_string )
                    set_string( get_string() );
            }
            
            
            void change_value_type( ValueType new_value_type )
            {
                // Clear any previous staff
                switch (value_type) 
                {
                    case value_void:
                    case value_string:
                    case value_number:
                        break;

                    case value_vector:
                        clear_vector();                    
                        break;
                        
                    case value_map:
                        clear_map();                    
                        break;
                }

                // Assign the new value_type
                value_type = new_value_type;
            }
            

            
            // ----------------------------------------------------------------------------------------
            // SET AND GET functions
            // ----------------------------------------------------------------------------------------

            void operator=( const char* _value ) 
            {
                // Set as integer
                change_value_type( value_string );
                _value_string = _value;
            }
            
            
            void set_string( std::string _value )
            {
                change_value_type( value_string );
                _value_string = _value;
            }
            
            void operator=( double _value ) 
            {
                change_value_type( value_number );
                _value_double = _value;
            }
            
            void set_double( double _value ) 
            {
                change_value_type( value_number );
                _value_double = _value;
            }
            
            void set_string( const char * _value )
            {
                change_value_type( value_string );
                _value_string = _value;
            }
            
            void operator++ () 
            {
                switch (value_type) 
                {
                    case value_number:
                        _value_double++;
                        break;
                        
                    default:
                        break;
                }
            }
            
            
            bool isNumber() const
            {
                return ( value_type ==  value_number );
            }

            bool isString() const
            {
                return ( value_type ==  value_string );
            }
                        
            double getDouble() const
            {
                switch ( value_type )
                {
                    case value_void:
                        return 0;
                    case value_number:
                        return _value_double;
                        
                    case value_string:
                        return atof(_value_string.c_str());
                        
                    case value_vector:
                    {
                        if( _value_vector.size() > 0 )
                            return _value_vector[0]->getDouble();
                        else
                            return 0;
                    }
                        
                    case value_map:
                        return 0;
                }

                LM_X(1, ("Internal error"));
                return 0;
            }
            
            std::string get_string() const
            {
                switch ( value_type )
                {
                    case value_void:
                        return "";
                    case value_number:
                        return au::str("%f", _value_double);
                    case value_string:
                        return _value_string;
                        
                    case value_vector:
                    {
                        std::ostringstream output;
                        output << "[ ";
                        for( size_t i = 0 ; i < _value_vector.size() ; i++ )
                            output << _value_vector[i]->str() << " ";
                        output << "]";
                        return output.str();
                    }
                        
                    case value_map:
                        return 0;
                }
                
                LM_X(1, ("Internal error"));
                return 0;                
            }
            
            void append_string( Value* value )
            {
                // Convert this to string
                convert_to_string();
                _value_string.append( value->get_string() );
            }
        
            
            static const char * strSerialitzationCode( SerialitzationCode code )
            {
                switch (code) 
                {
                    case ser_void: return "ser_void";
                        
                    case ser_int_positive: return "ser_int_positive";
                    case ser_int_negative: return "ser_int_negative";
                    case ser_int_value_0: return "ser_int_value_0";
                    case ser_int_value_1: return "ser_int_value_1";
                    case ser_int_value_2: return "ser_int_value_2";
                    case ser_int_value_3: return "ser_int_value_3";
                    case ser_int_value_4: return "ser_int_value_4";
                    case ser_int_value_5: return "ser_int_value_5";
                    case ser_int_value_6: return "ser_int_value_6";
                    case ser_int_value_7: return "ser_int_value_7";
                    case ser_int_value_8: return "ser_int_value_8";
                    case ser_int_value_9: return "ser_int_value_9";
                    case ser_int_value_10: return"ser_int_value_10";
                    case ser_int_value_minus_1: return "ser_int_value_minus_1";
                        
                    case ser_double_positive_1_decimal: return "ser_double_positive_1_decimal";
                    case ser_double_positive_2_decimal: return "ser_double_positive_2_decimal";     
                    case ser_double_positive_3_decimal: return "ser_double_positive_3_decimal";     
                    case ser_double_positive_4_decimal: return "ser_double_positive_4_decimal";     
                    case ser_double_positive_5_decimal: return "ser_double_positive_5_decimal";     
                        
                    case ser_double_negative_1_decimal: return "ser_double_negative_1_decimal";     
                    case ser_double_negative_2_decimal: return "ser_double_negative_2_decimal";      
                    case ser_double_negative_3_decimal: return "ser_double_negative_3_decimal";      
                    case ser_double_negative_4_decimal: return "ser_double_negative_4_decimal";      
                    case ser_double_negative_5_decimal: return "ser_double_negative_5_decimal";      
                        
                    case ser_double: return "ser_double";     
                        
                    case ser_string: return "string";
                    case ser_string_constant: return "string_constant";
                    case ser_string_smaz: return "string_smaz";
                        
                    case ser_vector: return "vector";
                    case ser_vector_len_2: return "ser_vector_len_2";
                    case ser_vector_len_3: return "ser_vector_len_3";
                    case ser_vector_len_4: return "ser_vector_len_4";
                    case ser_vector_len_5: return "ser_vector_len_5";
                        
                    case ser_map: return "ser_map";
                    case ser_map_len_1: return "ser_map_len_1";
                    case ser_map_len_2: return "ser_map_len_2";
                    case ser_map_len_3: return "ser_map_len_3";
                    case ser_map_len_4: return "ser_map_len_4";
                    case ser_map_len_5: return "ser_map_len_5";
                }
                
                return "Unknown";
                
            }
            
            
            const char* strType()
            {
                switch (value_type) 
                {
                    case value_void: return "void";
                    case value_string: return "string";
                    case value_number: return "number";
                    case value_vector: return "vector";
                    case value_map: return "map";
                }
                return "Unknown";
            }
            
        };

        
        
    } // end of namespace samson
} // end of namespace system

#endif
