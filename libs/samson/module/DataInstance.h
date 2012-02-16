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
		virtual int          serialize(char *data)= 0;					// Serialize to a vector (returns bytes writed)

		virtual int          hash(int max_max) = 0;						// Hash function when this is used as key
        virtual int          partition( int num_partitions )
        {
            return hash(num_partitions);                                // Default implementation
        };

        // Methods for select a particular component inside this datatype
        virtual int *getDataPath(const std::string &dataPathString) = 0; // Function to select fields
        virtual std::string getTypeFromPath(const std::string &dataPathString) = 0;
        virtual std::string getTypeFromPath(const int *dataPathIntP) = 0;
        virtual const char *getType() = 0;
        virtual bool checkType(const char *type) = 0;
        virtual DataInstance * getDataInstanceFromPath(const int *dataPathIntP) = 0;

        virtual void set( const char* str )
        {
            // Set value from a char* ( used to lookup by key )
        }
        
        // Hash type check to avoid errors
        virtual size_t getHashType() = 0;
        virtual bool checkHashType(size_t valType) = 0;

        // Get the name of the data 
        virtual std::string getName()=0;
        
		virtual std::string  str()= 0;									// Function  to get a debug-like string with the content
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
