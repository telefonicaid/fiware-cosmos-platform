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
	 String
	 */
	
	class String : public ss::DataInstance{
		
		std::string value;
		
	public:
		String() : ss::DataInstance(){
		}
		
		~String() {
		}
		
		int parse(char *data){
			int pos = 0;
			while( data[pos] != '\0' )
				pos++;
			value = data;
			return pos;
		}
		
		int serialize(char *data){
			memcpy(data, value.c_str(), value.length());
			return value.length();
		}
		
		int hash(int max_num_partitions){
			return (value.size())%max_num_partitions;
		}
		
		static int size(char *data){
			int pos = 0;
			while( data[pos] != '\0')
				pos++;
			return pos;
		}
		
		inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 )
		{
			int pos1 = 0;
			while( data1[pos1] != '\0')
				pos1++;

			int pos2 = 0;
			while( data2[pos2] != '\0')
				pos2++;
			
			if( pos1 < pos2 )
				return -1;
			else if( pos1 > pos2 )
				return 1;

			// Same length
			
			for (int i = 0 ; i < pos1 ; i++)
			{
				if ( data1[i] < data2[i] )
					return -1;
				else if ( data1[i] > data2[i] )
					return 1;
			}
			
			// Same content
			return 0;
		}
		
		inline static int compare( KV* kv1 , KV*kv2 )
		{
			size_t offset_1=0;
			size_t offset_2=0;
			return compare( kv1->key , kv2->key , &offset_1 , &offset_2 );
		}
		
		void copyFrom( String *other ){
			value = other->value;
		};
		
		std::string str(){
			return value;
		}
		
		void operator= (std::string &_value) {
			value = _value;
		}			
		
	}; 	
	
	/**
	 Void
	 */
	 
	
	class Void : public ss::DataInstance{
		
		
	public:
		Void() : ss::DataInstance(){
		}
		
		~Void() {
		}
		
		int parse(char *data){
			return 0;
		}
		
		int serialize(char *data){
			return 0;
		}
		
		int hash(int max_num_partitions){
			return rand()%max_num_partitions;	// This has never to be used unless a random distribution is desired
		}
		
		static int size(char *data){
			return 0;
		}
		
		inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
			return 0; // No unnecessary movement for sorting
		}
		
		inline static int compare( KV* kv1 , KV*kv2 )
		{
			size_t offset_1=0;
			size_t offset_2=0;
			return compare( kv1->key , kv2->key , &offset_1 , &offset_2 );
		}
		
		void copyFrom( Void *other ){
			// Nothing to do here
		};
		
		std::string str(){
			return "";
		}
		
	}; 	
	
	/** 
	 UInt  Data Instance 
	 */
	
	template <typename T>
	class FixedLengthDataInstance : public ss::DataInstance
	{

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
	

	// Default types

	class Int32 : public FixedLengthDataInstance<int>
	{
	public:
		int hash(int max_num_partitions){
			return abs(value)%max_num_partitions;
		}
		
		void operator= (int _value) {
			value = _value;
		}			
		
		
	};

	class Int16 : public FixedLengthDataInstance<short>
	{
	public:
		int hash(int max_num_partitions){
			return abs(value)%max_num_partitions;
		}
		
	};

	class Int8 : public FixedLengthDataInstance<char>
	{
	public:
		int hash(int max_num_partitions){
			return abs(value)%max_num_partitions;
		}
	};
	
	class UInt64 : public FixedLengthDataInstance<size_t>
	{
	public:
		int hash(int max_num_partitions){
			return abs(value)%max_num_partitions;
		}
	};
	
	class UInt32 : public FixedLengthDataInstance<unsigned int>
	{
	public:
		int hash(int max_num_partitions){
			return abs(value)%max_num_partitions;
		}
	};
	
	class UInt16 : public FixedLengthDataInstance<unsigned short>
	{
	public:
		int hash(int max_num_partitions){
			return abs(value)%max_num_partitions;
		}
	};
	
	class UInt8 : public FixedLengthDataInstance<unsigned char>
	{
	public:
		int hash(int max_num_partitions){
			return abs(value)%max_num_partitions;
		}
		
	};	
	
	class Double : public FixedLengthDataInstance<double>
	{
	public:
		int hash(int max_num_partitions)
		{
			return abs((int) 1000*value) % max_num_partitions;
		}
		
	};

	class Float : public FixedLengthDataInstance<float>
	{
	public:
		int hash(int max_num_partitions)
		{
			return abs((int) 1000*value) % max_num_partitions;
		}
		
	};
	
	
	
	
	
	}		// system namespace
} // ss namespace

#endif
