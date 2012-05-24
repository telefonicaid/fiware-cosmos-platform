#ifndef SAMSON_DATA_INSTANCE_H
#define SAMSON_DATA_INSTANCE_H

/* ****************************************************************************
*
* FILE                     DataInstance.h - 
*
* Data used to parse and serialize content in SAMSON Platform
*
* All data types are subclasses of this one. The two major functions are parse and serialize that convert the content of this
* data type into a stream of bytes
*
* hash and partition are the functions used to distribute key-values across the cluster.
* Equal keys should return equal values in hash and partition functions
* 
*/

#include <string>               // std::string
#include <sstream>				// std::ostringstream
#include <samson/module/var_int.h>	

namespace samson {
    
	class DataInstance 
	{
	public:
		virtual int          parse(char *data) = 0;						// Parse input buffer (return bytes read)
		virtual int          serialize(char *data)= 0;					// Serialize to a vector (returns bytes written)

		virtual int          hash(int max_max) = 0;						// Hash function when this is used as key
        virtual int          partition( int num_partitions )
        {
            return hash(num_partitions);                                // Default implementation
        };

        // Methods for select a particular component inside this datatype
        // virtual int *getDataPath(const std::string &dataPathString) = 0; // Function to select fields
        // virtual std::string getTypeFromPath(const std::string &dataPathString) = 0;
        // virtual std::string getTypeFromPath(const int *dataPathIntP) = 0;
        //virtual DataInstance * getDataInstanceFromPath(const int *dataPathIntP) = 0;        
		
		virtual const char *getType() = 0;
        virtual bool checkType(const char *type) = 0;
        virtual int serial_compare( char* data1 , char* data2 )=0;        
        virtual void setFromString( const char* str )
        {
            if (str == NULL)
                return;
            // Set value from a char* ( used to lookup by key )
        }
        
        // Hash type check to avoid errors
        virtual size_t getHashType() = 0;
        virtual bool checkHashType(size_t valType) = 0;

        // Get the name of the data 
        virtual std::string getName()=0;
        
        // Function  to get content of this value in a string
		virtual std::string  str()= 0;		
        
        // JSON Version of this data instance
		virtual std::string strJSON()=0;
        
        // XML Version of this data instance
        virtual std::string strXML()=0;

        // HTML version of this data type
        virtual std::string strHTML(int level_html_heading)=0;
        
        // HTML table version of this data type
        virtual std::string strHTMLTable(std::string name)=0;

        virtual std::string paint_header(int init_col)=0;
        virtual std::string paint_header_basic(int init_col)=0;
        virtual std::string paint_value(int init_col)=0;

        virtual int num_fields()=0;
        virtual int num_basic_fields()=0;
        virtual int max_depth()=0;
        virtual int max_num_values()=0;
        virtual bool is_terminal()=0;

		virtual ~DataInstance(){}                                       // Virtual destructor necessary since delete is called from parent class
	};
    
    class KVFormatDataInstances
    {
        
    public:
        
        DataInstance *key;
        DataInstance *value;
        
        KVFormatDataInstances( DataInstance *_key , DataInstance *_value )
        {
            key = _key;
            value = _value;
        }
        
        void destroy()
        {
            if( key ) 
                delete key;
            if( value ) 
                delete value;
        }
        
    };
	
} // ss namespace

#endif
