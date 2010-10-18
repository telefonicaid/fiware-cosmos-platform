#ifndef SAMSON_DATA_INSTANCE_H
#define SAMSON_DATA_INSTANCE_H

/* ****************************************************************************
*
* FILE                     DataInstance.h - 
*
* Data used to parse and serialize content
*
*
*/
#include <string>                /* std::string                              */
#include <sstream>				/*std::ostringstream*/
#include <samson/var_int.h>	

namespace ss {
	class DataInstance 
	{
	public:
		virtual int          parse(char *data) = 0;						// Parse input buffer (return bytes read)
		virtual int          serialize(char *data)= 0;					// Serialize to a vector (returns bytes writed)
		virtual int          hash(int max_max) = 0;						// Hash function when this is used as key
		virtual std::string  str()= 0;									// Function  to get a debug-like string with the content
		virtual ~DataInstance(){}
	};
	
	
	
namespace system {
	
		/** 
		 UInt  Data Instance 
		 */
		
		class UInt : public ss::DataInstance{
			
			size_t value;
			
		public:
			UInt() : ss::DataInstance(){
			}
			
			~UInt() {
			}
			
			int parse(char *data){
				return ss::staticVarIntParse( data , &value);
			}
			
			int serialize(char *data){
				return ss::staticVarIntSerialize( data , value);
			}
			
			int hash(int max_num_partitions){
				return value%max_num_partitions;
			}
			
	
			static int size(char *data){
				size_t _value;
				return ss::staticVarIntParse( data , &_value);
			}
	
			inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
				{ // comparing b
					size_t _value1;
					size_t _value2;
					*offset1 += ss::staticVarIntParse(data1 + (*offset1), &_value1);
					*offset2 += ss::staticVarIntParse(data2 + (*offset2), &_value2);
					if( _value1 < _value2 ) return -1;
					if( _value1 > _value2 ) return  1;
					return 0;
				}
				return 0; //If everything is equal
			}
			
			inline static int compare( KV* kv1 , KV*kv2 )
			{
				size_t offset_1=0;
				size_t offset_2=0;
				return compare( kv1->key , kv2->key , &offset_1 , &offset_2 );
			}
			
			void copyFrom( UInt *other ){
				value = other->value;
			};
			
			std::string str(){
				std::ostringstream o;
				o << value;
				return o.str();
			}
			
			void operator= (size_t _value) {
				value = _value;
			}			
			void operator= (int _value) {
				value = _value;
			}			
			
		}; 	
	
	
	/** 
	 UInt  Data Instance 
	 */
	
	template <typename T>
	class FixedLengthDataInstance : public ss::DataInstance{

	protected:
		T value;
		
	public:
		FixedLengthDataInstance() : ss::DataInstance(){
		}
		
		~FixedLengthDataInstance() {
		}
		
		int parse(char *data){
			value = *((T*)data);
			return sizeof(T);
		}
		
		int serialize(char *data){
			*((T*)data) = value;
			return sizeof(T);
		}
		
		static int size(char *data){
			return sizeof(T);
		}
		
		inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){

			T _value1,_value2;
			
			_value1 = *((T*)data1);
			_value2 = *((T*)data2);
			
			*offset1 += sizeof(T);
			*offset2 += sizeof(T);

			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
			return 0;
		}
		
		inline static int compare( KV* kv1 , KV*kv2 )
		{
			size_t offset_1=0;
			size_t offset_2=0;
			return compare( kv1->key , kv2->key , &offset_1 , &offset_2 );
		}
		
		void copyFrom( FixedLengthDataInstance<T> *other ){
			value = other->value;
		};
		
		std::string str(){
			std::ostringstream o;
			o << value;
			return o.str();
		}

		void operator= (T _value) {
			value = _value;
		}			
		
	}; 	
	
	
	class Int32 : public FixedLengthDataInstance<int>
	{
	public:
		int hash(int max_num_partitions){
			return abs(value)%max_num_partitions;
		}
		
		void operator= (size_t _value) {
			value = _value;
		}			
		void operator= (int _value) {
			value = _value;
		}			
		
	};

	class Double : public FixedLengthDataInstance<double>
	{
	public:
		int hash(int max_num_partitions)
		{
			return abs((int) value) % max_num_partitions;
		}
		
	};
	
	
	
	
	}		// system namespace
} // ss namespace

#endif
