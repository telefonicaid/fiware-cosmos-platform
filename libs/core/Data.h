#ifndef SAMSON_DATA_H
#define SAMSON_DATA_H

#include <math.h>

#include "KVFormat.h"
#include "au.h"



/**
 Basic data types used in SAMSON
 */

typedef size_t			ss_uint64;
typedef unsigned int	ss_uint32;
typedef unsigned char	ss_uint8;
typedef int				ss_int32;
typedef char			ss_int8;
typedef float			ss_float;
typedef double			ss_double;
typedef size_t			ss_uint;		// Variable length unsigned integuer

namespace ss {

	/**
	 Basic type to work with key-values
	 It consists in a hash code of the key and pointers to the key and value raw data
	 */
	
	typedef struct
	{
		size_t key_size;
		size_t value_size;
		char *key;
		char *value;
	} KV;
	
	/**
	 Vector of KVs
	 Variables num_kvs and kvs are not suppoused to be modified from outside this class
	 */
	
	class KVVector
	{
	public:
		
		KV *kvs;
		size_t num_kvs;
		
		KVVector( size_t _num_kvs )
		{
			num_kvs = _num_kvs;
			kvs = (KV*) malloc( num_kvs*sizeof(KV) );
		}
		
		~KVVector()
		{
			free( kvs );
		}		
	};
	
	/**
	 Structure used as interface for the generator/map/reduce operations 
	 */
	
	typedef struct
	{
		KV **kvs;			// Vector containing Key-values ( pointers to row data and hash code )
		size_t num_kvs;
	} KVSET;
	
	/**
	 Variable length integuer definition
	 */
	
	inline int staticVarIntParse( char *data , size_t* value)
	{
		unsigned char *p = (unsigned char *)data;
		size_t base = 1;	
		
		int offset = 0;
		size_t tmp_value = 0;
		
		do
		{
			tmp_value += (size_t)(p[offset]&127)*base; 
			base = base*128;
		}
		while( p[offset++] & 128 );
		
		*value = tmp_value;
		return offset;	
	}
	
	inline int staticVarIntSerialize( char *data , size_t value)
	{
		unsigned char *p = (unsigned char *)data;
		
		size_t __value = value;
		
		//Special case
		if (__value==0)
		{
			p[0] = 0;
			return 1;
		}
		
		int offset = 0;
		unsigned char tmp;
		
		while( __value > 0 )
		{
			//cerr << "Parsign value "<< __value << "\n";
			
			tmp  = __value - (__value/128)*128;
			__value = __value/128;
			
			if( __value >0)
				tmp |= 128;	//Add flag
			
			//Push to the buffer
			p[offset++]=tmp;
		}
		return offset;		
	}
	

	/**
	 Data used to parse and serialize content
	 */

	class DataInstance 
	{
	public:
		
		virtual int parse(char *data)=0;													// Parse input buffer (return bytes read)
		virtual int serialize(char *data)=0;												// Serialize to a vector (returns bytes writed)
		virtual int getPartition(int max_num_partitions)=0;									// Hash function when this is used as key
		virtual std::string str()=0;														// Function  to get a debug-like string with the content
	};

	// Static functions necessary for a Data
	typedef DataInstance* (*DataCreationFunction)();						// Creation function
	typedef bool(* DataCompareFunction)(KV *kv1 , KV*kv2);					// Compare function key-values 
	typedef int(* DataFullCompareFunction)(KV *kv1 , KV*kv2);				// Compare key-values

	// Internal implementation to "structs"
	typedef bool(* DataCompareDataFunction)(char *data1 ,char *data2);		
	

	/** 
	 Definition of a new type of data
	 */
	
	class Data
	{		
		
	public:

		std::string _name;									// Name of this data ( inside the module defined )
		std::string _helpMessage;							// Help message shown on screen
		DataCompareFunction _compareByValueFunction;		// Global function to sort by value ( inside elements with the same key )
		DataCreationFunction _creationFunction;				// Function used to create new instances
	public:
		
		/**
		 Inform about the type of operation it is
		 */
		
		Data( std::string name , DataCreationFunction creationFunction )
		{
			_name = name;
			_helpMessage = "Help comming soon\n";
			_creationFunction = creationFunction;
			_compareByValueFunction = NULL;
		}
		
		std::string getName()
		{
			return _name;
		}
		
		/**
		 This functions inform about the fixed size of this Data
		 Otherwise -1 is return
		 */
		
		DataInstance * getInstance()
		{
			return _creationFunction();
		}
		
		std::string help()
		{
			return _helpMessage;
		};									
		
		// Set static functions

		void setCompareByValueFunction( DataCompareFunction compareAsValueFunction )
		{
			_compareByValueFunction = compareAsValueFunction;
		}
		
		// Get static function
		
		DataCompareFunction getCompareByValueFunction( )
		{
			return _compareByValueFunction;
		}
	};
		
	
	// Spetial data types
	
	namespace system {
		
		/**
		 Particular Data type for "nothing"
		 */
		
		class Void : public DataInstance	
		{
		public:
			
			Void()
			{
			}
			
			static DataInstance *createFunction(){
				return new Void();
			}
			
			virtual int parse(char *data)
			{
				return 0;
			}
			
			virtual int serialize(char *data)
			{
				return 0 ;
			}
			
			virtual int size(char *data)
			{
				return 0 ;
			}
			
			virtual int getPartition(int max_num_partitions)
			{
				//This should never be used
				return 0;
			}
			
			virtual std::string str()
			{
				return "";
			}
			
		};

		/**
		 Data to hold an string
		 */

		class  String : public DataInstance
		{
		public:
			
			std::string content;
			char buffer[10000];
			
			String()
			{
			}
			
			static DataInstance *createFunction(){
				return new String();
			}
			
			virtual int parse(char *data)
			{
				content = data;
				return content.length()+1;
				/*
				size_t len;
				int offset = staticVarIntParse( data , &len );
				memcpy(buffer, data+offset, len);
				buffer[len] = '\0';
				content = buffer;
				return offset + len;
				 */
			}
			
			virtual int serialize(char *data)
			{
				/*
				size_t len = content.length();
				assert( len < 10000);
				int offset = staticVarIntSerialize( data , len);
				memcpy(data+offset, content.c_str(), len);
				return offset+len ;
				*/
				
				size_t len = content.length();
				memcpy(data, content.c_str(), len );
				data[len] = '\0';
				return len+1;
				
			}
			
			
			virtual int getPartition(int max_num_partitions)
			{
				return content.length()%max_num_partitions;
			}
			
			inline static bool compareByValue( KV* kv1 , KV* kv2)
			{
				if( kv1->value_size != kv2->value_size )
					return kv1->value_size < kv2->value_size;
				
				for (size_t i = 0 ; i < kv1->value_size ; i++)
				{
					if ( kv1->value[i] != kv1->value[i] )
						return (kv1->value[i] < kv2->value[i]);
				}
				
				return false;
				
			}
				
			virtual std::string str()
			{
				return content;
			}
			
		};	
	}	
	
	/**
	 General interface to emit Key-values at the output
	 */
	
	class KVWriter
	{
	public:
		virtual void emit( DataInstance * key , DataInstance * value)=0;
	};
	
	
	
} // ss namespace

#endif
