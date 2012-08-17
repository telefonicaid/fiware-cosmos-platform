
#ifndef _H_SAMSON_SYSTEM_VALUE_Value
#define _H_SAMSON_SYSTEM_VALUE_Value

#include <string.h>
#include <cctype>
#include "smaz.h"
#include "logMsg/logMsg.h"

#include "au/Pool.h"
#include "au/tables/pugi.h"

#include "json.h"

#include "samson/module/DataInstance.h"

#define VALUE_CODE 1219561887489248771ULL

namespace samson{ namespace system{
    
    // Constant word serialization
    int get_constant_word_code( const char * word );
    const char* get_constant_word( int c );
    
    
    class Value : public samson::DataInstance
    {
        // Pool of Value instances for vector and map
        static au::Pool<Value>* pool_values;
        
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
        
        ~Value() 
        {
            // This instance will be never reused but the internal values will be reused
            clear();
        }
        
        static Value * getInstance()
        {
            if( !pool_values )
                pool_values = new au::Pool<Value>();;

            Value * v = pool_values->pop();
            if( v )
                return v;
            return new Value();
        }
        
        static void reuseInstance( Value* value )
        {
            if( !pool_values )
                pool_values = new au::Pool<Value>();;
            
            value->clear();
            pool_values->push(value);
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

        void setFrom( struct json_object* json )
        {
            json_type type = json_object_get_type(json);
            switch (type) 
            {
                case json_type_boolean:
                    set_double(1);
                    break;
                case  json_type_string:
                    set_string( json_object_get_string(json) );
                    break;
                case json_type_double:
                    set_double( json_object_get_double(json) );
                    break;
                case json_type_int:
                    set_double( json_object_get_int(json) );
                    break;
                case json_type_object:
                {
                    set_as_map();
                    struct lh_table* table = json_object_get_object(json);
                    
                    lh_entry* entry = table->head;
                    while( entry )
                    {
                        // Add entry from map
                        char*                key     = (char*) entry->k;
                        struct json_object*  json2   = json_object_object_get(json, key);                        
                        add_value_to_map( key )->setFrom( json2 );
                        
                        // Go to the next one
                        entry = entry->next;
                    }
                }
                    break;
                case json_type_array:
                {
                    set_as_vector();
                    for ( int i = 0 ; i < json_object_array_length(json) ; i++ )
                    {
                        struct json_object*  json2 = json_object_array_get_idx(json, i);
                        add_value_to_vector()->setFrom( json2 );
                    }
                }
                    break;
                case json_type_null:
                    set_as_void();
                    break;
            }
            
        }
        void setFromString( const char *_data )
        {
            // BNy default json format is used
            setFromJSONString( _data );
        }
        
        void setFromJSONString( const char *_data )
        {
            struct json_object* json = json_tokener_parse( _data );            
            
            if( is_error( json ) )
            {
                set_as_void();
                return;
            }
            
            if( json )
            {
                setFrom( json );
                json_object_put(json);
            }
            else
                set_as_void();
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
        
        std::string strJSON();
        void _strJSON( std::ostream &output );
        
        std::string strHTML(int level_html_heading);
        void _strHTML(int level_html_heading , std::ostream &output);
        
        std::string strXML();
        std::string strHTMLTable(std::string _varNameInternal);
        std::string paint_header(int init_col);
        std::string paint_header_basic(int init_col);
        std::string paint_value(int index_row);
        int num_fields();
        int num_basic_fields();
        int max_depth();
        int max_num_values();
        bool is_terminal();
        
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
        void set_as_vector();
        void vectorize();
        
        Value* add_value_to_vector();
        Value* add_value_to_vector(size_t pos);
        Value* get_value_from_vector( size_t pos );
        size_t get_vector_size();
        void swap_vector_components( size_t pos , size_t pos2 );
        
        void pop_back_from_vector();
        
        // ----------------------------------------------------------------------------------------
        // Map functions
        // ----------------------------------------------------------------------------------------
        
        bool isMap();
        
        bool checkMapValue( const char* key, const char* value );
        
        void set_as_map();
        
        size_t get_num_map_values();
        
        Value* add_value_to_map( std::string key );
        Value* get_value_from_map( const char* key );
        Value* get_or_add_value_from_map( const char* key );
        
        const char* get_string_from_map( const char* key );
        size_t get_uint64_from_map( const char* key , size_t default_value = 0 );
        double get_double_from_map( const char* key , double default_value = 0 );
        
        void set_string_for_map( const char* key , const char* str );
        void set_uint64_for_map( const char* key , size_t value );
        void set_double_for_map( const char* key , double value );
        
        
        Value* get_value_from_map( std::string& key );
        
        std::vector<std::string> get_keys_from_map();
        
        void convert_to_number();
        
        void convert_to_string();
        
        void change_value_type( ValueType new_value_type );
        
        void add_double( double v );
        
        // ----------------------------------------------------------------------------------------
        // CLEAR Function
        // ----------------------------------------------------------------------------------------
        
        void clear();
        
        // ----------------------------------------------------------------------------------------
        // SET AND GET functions
        // ----------------------------------------------------------------------------------------
        
        void operator=( const char* _value );
        
        
        void set_string( std::string _value );
        
        void operator=( double _value );
        
        void set_double( double _value );
        void set_uint64( size_t _value );
        void set_string( const char * _value );
        void set_string( const char * _value , size_t len );
        
        void operator++ () ;
        
        
        bool isNumber() const;
        
        bool isString() const;
        
        double get_double( double default_value = 0) const;
        
        size_t get_uint64( size_t default_value = 0 ) const;
        
        bool isString( const char* str) const;
        
        std::string get_string();
        
        
        // Access to the string char*
        const char* c_str() const;
        
        void append_string( Value* value );
        
        
        static const char * strSerialitzationCode( SerialitzationCode code );
        
        
        
        const char* strType();
        
    };
    
} } // end of namespace samson.system

#endif
