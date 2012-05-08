
#ifndef _H_SAMSON_SYSTEM_VALUE_Value
#define _H_SAMSON_SYSTEM_VALUE_Value

#include <string.h>
#include <cctype>
#include "smaz.h"
#include "logMsg/logMsg.h"

#include "au/Pool.h"
#include "au/tables/pugi.h"

#include "samson/module/DataInstance.h"

#define VALUE_CODE 1219561887489248771ULL

namespace samson{
    namespace system{
        
        // Constant word serialization
        int get_constant_word_code( const char * word );
        const char* get_constant_word( int c );

        
        class Value : public samson::DataInstance 
        {
            // Pool of Value instances for vector and map
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
                ser_vector_len_0, // Vector with a particular length
                ser_vector_len_1, 
                ser_vector_len_2, 
                ser_vector_len_3,
                ser_vector_len_4,
                ser_vector_len_5,

                // Serialitzation of map
                // ------------------------------------------------------------
                ser_map,
                ser_map_len_0,    // Map with a particular number of elements inside
                ser_map_len_1,    
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
            
            // Return the name of this data type ( syste.Value )
            std::string getName()
            {
                return "system.Value";
            }
            
            // ------------------------------------------------------------
            // PARSE Operations
            // ------------------------------------------------------------
            
            int parse_void( char* data );
            int parse_number( char*data );            
            int parse_string( char*data );            
            int parse_vector( char *data );            
            int parse_map( char *data );            
            int parse( char *data );
            
            // ------------------------------------------------------------
            // SERIALIZE Operations
            // ------------------------------------------------------------

            int serialize_void(char *data);
            int serialize_number(char *data);
            int serialize_string(char *data);
            int serialize_vector(char *data);
            int serialize_map( char *data );            
            int serialize(char *data );
            
            // ------------------------------------------------------------
            // HASH Operations
            // ------------------------------------------------------------

            int hash_void(int max_num_partitions);
            int hash_number(int max_num_partitions);
            int hash_string(int max_num_partitions);
            int hash_vector(int max_num_partitions);
            int hash_map(int max_num_partitions);
            int hash(int max_num_partitions);
            
            // ------------------------------------------------------------
            // SIZE Operations
            // ------------------------------------------------------------
            
            static int size( char *data );
            
            // ------------------------------------------------------------
            // Data compare 
            // ------------------------------------------------------------
            
            static int compare( char* data1 , char* data2 , size_t* offset1 , size_t* offset2  );
            static int compare( char* data1 , char* data2 );
            int serial_compare( char* data1 , char* data2 );
            
            // ------------------------------------------------------------
            // Checks
            // ------------------------------------------------------------
            
            static const char *getTypeStatic();
            const char *getType();
            static bool checkTypeStatic(const char *type);
            bool checkType(const char *type);
            static size_t getHashTypeStatic();
            size_t getHashType();
            static bool checkHashTypeStatic(size_t valType);
            bool checkHashType(size_t valType);
            
            // ------------------------------------------------------------
            // Operations
            // ------------------------------------------------------------
            
            bool operator==(const Value &other) const;
            bool operator!=(const Value &other) const;
            bool operator<=(const Value &other) const;
            bool operator>=(const Value &other) const;
            bool operator<(const Value &other) const ;
            bool operator>(const Value &other) const;
            const Value operator+( Value &other); //const
            const Value operator-(const Value &other) const;            
            const Value operator*(const Value &other) const;
            const Value operator/(const Value &other) const;            
            inline int compare( const Value& other ) const;

            // ------------------------------------------------------------
            // setFromString ( necessary for REST )
            // ------------------------------------------------------------
                        
            void setFromString( const char *_data )
            {
                change_value_type( value_string );
                _value_string = _data;
            }
            
            // ------------------------------------------------------------
            // COPY Function 
            // ------------------------------------------------------------
            
            void copyFrom( Value *other );

            // ------------------------------------------------------------
            // STR Function 
            // ------------------------------------------------------------
            
            std::string str();
            
            // ------------------------------------------------------------
            // JSON & XML Functions
            // ------------------------------------------------------------

            std::string strJSONInternal(std::string _varNameInternal, bool vectorMember);
            std::string strJSON(std::string _varNameInternal);
            std::string strJSON();
            void _strJSON( std::ostream &output );            
            
            std::string strXML(std::string _varNameInternal);
            std::string strXMLInternal(std::string _varNameInternal);
            
            // ----------------------------------------------------------------------------------------
            // void functions
            // ----------------------------------------------------------------------------------------
            
            void set_as_void();
            bool isVoid();
            
            // ----------------------------------------------------------------------------------------
            // number functions
            // ----------------------------------------------------------------------------------------

            bool is_number();
            
            // ----------------------------------------------------------------------------------------
            // Vector functions
            // ----------------------------------------------------------------------------------------
            
            bool isVector();
            
            void clear_vector();
            
            void set_as_vector();

            void vectorize();
            
            Value* add_value_to_vector();
            
            Value* get_value_from_vector( size_t pos );
            
            size_t get_vector_size();
            
            // ----------------------------------------------------------------------------------------
            // Map functions
            // ----------------------------------------------------------------------------------------
            
            bool isMap();

            void clear_map();

            void set_as_map();
            
            size_t get_num_map_values();
            
            Value* add_value_to_map( std::string key );
            Value* get_value_from_map( const char* key );
            Value* get_or_add_value_from_map( const char* key );

            double get_double_from_map( const char* key , double default_value );
            
            Value* get_value_from_map( std::string& key );
            
            std::vector<std::string> get_keys_from_map();
            
            void convert_to_number();
            
            void convert_to_string();
            
            void change_value_type( ValueType new_value_type );
            
            void add_double( double v );

            
            // ----------------------------------------------------------------------------------------
            // SET AND GET functions
            // ----------------------------------------------------------------------------------------

            void operator=( const char* _value );
            
            
            void set_string( std::string _value );
            
            void operator=( double _value ) ;
            
            void set_double( double _value ) ;
            
            void set_string( const char * _value );

            void set_string( const char * _value , size_t len );
            
            void operator++ () ;
            
            
            bool isNumber() const;

            bool isString() const;
                        
            double getDouble() const;

            
            std::string get_string();

            
            // Access to the string char*
            const char* c_str() const;
            
            void append_string( Value* value );
        
            
            static const char * strSerialitzationCode( SerialitzationCode code );

            
            
            const char* strType();
            
        };

        
        
    } // end of namespace samson
} // end of namespace system

#endif
