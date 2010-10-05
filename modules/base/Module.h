/*

	Module Base Module for the SAMSON platform (base)

	File: /mnt/sda9/kzangeli/sb/samson/01/trunk/modules/base/Module.h
	NOTE: This file has been generated with the samson_module tool, please do not modify

*/

#ifndef _H_SS_base_Module
#define _H_SS_base_Module


#include <samson/samson_lib.h>



namespace ss
{
namespace base{

// Datas defined in this module

class UInt : public ss::DataInstance{

public:
	ss_uint value;


	UInt() : ss::DataInstance(){
	}

	static DataInstance *createFunction(){
		return new UInt();
	}
	inline static bool compareByValue( KV* kv1 , KV* kv2)
	{
	size_t offset_1 = 0;
	size_t offset_2 = 0;
	int c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );
	return (c<0);
	}
	~UInt() {
	}

	int parse(char *data){
		int offset=0;
		offset += ss::staticVarIntParse( data+offset , &value);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += ss::staticVarIntSerialize( data+offset , value);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += ss::staticVarIntParse( data+offset , &value);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return value%max_num_partitions;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing value
			ss_uint _value1;
			ss_uint _value2;
			*offset1 += ss::staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += ss::staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( UInt *other ){
		value=other->value;
	};

	std::string str(){
		std::ostringstream o;
		o << value;

		o<<" ";
		return o.str();
	}

}; //class UInt

class UInt2 : public ss::DataInstance{

public:
	ss_uint value;
	ss_uint value2;


	UInt2() : ss::DataInstance(){
	}

	static DataInstance *createFunction(){
		return new UInt2();
	}
	inline static bool compareByValue( KV* kv1 , KV* kv2)
	{
	size_t offset_1 = 0;
	size_t offset_2 = 0;
	int c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );
	return (c<0);
	}
	~UInt2() {
	}

	int parse(char *data){
		int offset=0;
		offset += ss::staticVarIntParse( data+offset , &value);
		offset += ss::staticVarIntParse( data+offset , &value2);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += ss::staticVarIntSerialize( data+offset , value);
		offset += ss::staticVarIntSerialize( data+offset , value2);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += ss::staticVarIntParse( data+offset , &value);
		offset += ss::staticVarIntParse( data+offset , &value2);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return value%max_num_partitions;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing value
			ss_uint _value1;
			ss_uint _value2;
			*offset1 += ss::staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += ss::staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing value2
			ss_uint _value1;
			ss_uint _value2;
			*offset1 += ss::staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += ss::staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( UInt2 *other ){
		value=other->value;
		value2=other->value2;
	};

	std::string str(){
		std::ostringstream o;
		o << value;

		o<<" ";
		o << value2;

		o<<" ";
		return o.str();
	}

}; //class UInt2

class Float : public ss::DataInstance{

public:
	ss_float value;


	Float() : ss::DataInstance(){
	}

	static DataInstance *createFunction(){
		return new Float();
	}
	inline static bool compareByValue( KV* kv1 , KV* kv2)
	{
	size_t offset_1 = 0;
	size_t offset_2 = 0;
	int c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );
	return (c<0);
	}
	~Float() {
	}

	int parse(char *data){
		int offset=0;
		value=  *( (ss_float*) (data+offset) ); offset +=  sizeof(ss_float);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		*( (ss_float*) (data+offset) )=value; offset +=  sizeof(ss_float);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += sizeof(ss_float);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return abs( (int) (value*100))%max_num_partitions;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing value
			if  ( *((ss_float*)(data1 + (*offset1))) < *((ss_float*)(data2 + (*offset2))) ) return -1;
			if  ( *((ss_float*)(data1 + (*offset1))) > *((ss_float*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(ss_float);
			*offset2 +=sizeof(ss_float);
		}
		return 0; //If everything is equal
	}

	void copyFrom( Float *other ){
		value=other->value;
	};

	std::string str(){
		std::ostringstream o;
		o << value;

		o<<" ";
		return o.str();
	}

}; //class Float

class Uint32 : public ss::DataInstance{

public:
	ss_uint32 value;


	Uint32() : ss::DataInstance(){
	}

	static DataInstance *createFunction(){
		return new Uint32();
	}
	inline static bool compareByValue( KV* kv1 , KV* kv2)
	{
	size_t offset_1 = 0;
	size_t offset_2 = 0;
	int c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );
	return (c<0);
	}
	~Uint32() {
	}

	int parse(char *data){
		int offset=0;
		value=  *( (ss_uint32*) (data+offset) ); offset +=  sizeof(ss_uint32);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		*( (ss_uint32*) (data+offset) )=value; offset +=  sizeof(ss_uint32);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += sizeof(ss_uint32);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return value%max_num_partitions;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing value
			if  ( *((ss_uint32*)(data1 + (*offset1))) < *((ss_uint32*)(data2 + (*offset2))) ) return -1;
			if  ( *((ss_uint32*)(data1 + (*offset1))) > *((ss_uint32*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(ss_uint32);
			*offset2 +=sizeof(ss_uint32);
		}
		return 0; //If everything is equal
	}

	void copyFrom( Uint32 *other ){
		value=other->value;
	};

	std::string str(){
		std::ostringstream o;
		o << value;

		o<<" ";
		return o.str();
	}

}; //class Uint32

class Uint64 : public ss::DataInstance{

public:
	ss_uint64 value;


	Uint64() : ss::DataInstance(){
	}

	static DataInstance *createFunction(){
		return new Uint64();
	}
	inline static bool compareByValue( KV* kv1 , KV* kv2)
	{
	size_t offset_1 = 0;
	size_t offset_2 = 0;
	int c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );
	return (c<0);
	}
	~Uint64() {
	}

	int parse(char *data){
		int offset=0;
		value=  *( (ss_uint64*) (data+offset) ); offset +=  sizeof(ss_uint64);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		*( (ss_uint64*) (data+offset) )=value; offset +=  sizeof(ss_uint64);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += sizeof(ss_uint64);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return value%max_num_partitions;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing value
			if  ( *((ss_uint64*)(data1 + (*offset1))) < *((ss_uint64*)(data2 + (*offset2))) ) return -1;
			if  ( *((ss_uint64*)(data1 + (*offset1))) > *((ss_uint64*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(ss_uint64);
			*offset2 +=sizeof(ss_uint64);
		}
		return 0; //If everything is equal
	}

	void copyFrom( Uint64 *other ){
		value=other->value;
	};

	std::string str(){
		std::ostringstream o;
		o << value;

		o<<" ";
		return o.str();
	}

}; //class Uint64

class Vector_UInt : public ss::DataInstance{

public:
	ss_uint *values;
	int values_length;
	int values_max_length;


	Vector_UInt() : ss::DataInstance(){
		values_length=0;
		values_max_length=0;
		values = NULL;
	}

	static DataInstance *createFunction(){
		return new Vector_UInt();
	}
	inline static bool compareByValue( KV* kv1 , KV* kv2)
	{
	size_t offset_1 = 0;
	size_t offset_2 = 0;
	int c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );
	return (c<0);
	}
	~Vector_UInt() {
		if( values )
			free( values );
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector values
			size_t _length;
			offset += ss::staticVarIntParse( data+offset , &_length );
		 	valuesSetLength( _length );
			for (int i = 0 ; i < (int)values_length ; i++){
				offset += ss::staticVarIntParse( data+offset , &values[i]);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector values
			offset += ss::staticVarIntSerialize( data+offset , values_length );
			for (int i = 0 ; i < (int)values_length ; i++){
				offset += ss::staticVarIntSerialize( data+offset , values[i]);
			}
		}
		return offset;
	}

	int size(char *data){
		int offset=0;
		{ //Getting size of vector values
			size_t _length;
			offset += ss::staticVarIntParse( data+offset , &_length );
			ss_uint _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += ss::staticVarIntParse( data+offset , &_tmp);
			}
		}
		return offset;
	}

	int getPartition(int max_num_partitions){
		if( values_length > 0 ){
		return values[0]%max_num_partitions;
		} else return 0;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // Comparing vector values
			size_t _length1,_length2;
			*offset1 += ss::staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += ss::staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing values[i]
					ss_uint _value1;
					ss_uint _value2;
					*offset1 += ss::staticVarIntParse(data1 + (*offset1), &_value1);
					*offset2 += ss::staticVarIntParse(data2 + (*offset2), &_value2);
					if( _value1 < _value2 ) return -1;
					if( _value1 > _value2 ) return  1;
				}
			}
		}
		return 0; //If everything is equal
	}

	void valuesSetLength(int _length){
		if( _length > values_max_length){ 
			ss_uint *_previous = values;
			int previous_length = values_length;
			if(values_max_length == 0) values_max_length = _length;
			while(values_max_length < _length) values_max_length *= 2;
			values = (ss_uint*) malloc( sizeof(ss_uint) * values_max_length );
			if( _previous ){
				memcpy(values, _previous, previous_length*sizeof(ss_uint));
				free( _previous );
			}
		}
		values_length=_length;
	}

	void valuesAdd(ss_uint _value){
		valuesSetLength( values_length + 1 );
		values[values_length-1] = _value;
	}

	void copyFrom( Vector_UInt *other ){
			{ // CopyFrom field values
				valuesSetLength( other->values_length);
				for (int i = 0 ; i < values_length ; i++){
					values[i]=other->values[i];
				}
			}
	};

	std::string str(){
		std::ostringstream o;
		{// toString of vector values
			for(int i = 0 ; i < values_length ; i++){
				o << values[i];
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

}; //class Vector_UInt

class Vector_UInt32 : public ss::DataInstance{

public:
	ss_uint32 *values;
	int values_length;
	int values_max_length;


	Vector_UInt32() : ss::DataInstance(){
		values_length=0;
		values_max_length=0;
		values = NULL;
	}

	static DataInstance *createFunction(){
		return new Vector_UInt32();
	}
	inline static bool compareByValue( KV* kv1 , KV* kv2)
	{
	size_t offset_1 = 0;
	size_t offset_2 = 0;
	int c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );
	return (c<0);
	}
	~Vector_UInt32() {
		if( values )
			free( values );
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector values
			size_t _length;
			offset += ss::staticVarIntParse( data+offset , &_length );
		 	valuesSetLength( _length );
			for (int i = 0 ; i < (int)values_length ; i++){
				values[i]=  *( (ss_uint32*) (data+offset) ); offset +=  sizeof(ss_uint32);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector values
			offset += ss::staticVarIntSerialize( data+offset , values_length );
			for (int i = 0 ; i < (int)values_length ; i++){
				*( (ss_uint32*) (data+offset) )=values[i]; offset +=  sizeof(ss_uint32);
			}
		}
		return offset;
	}

	int size(char *data){
		int offset=0;
		{ //Getting size of vector values
			size_t _length;
			offset += ss::staticVarIntParse( data+offset , &_length );
			offset += _length*sizeof(ss_uint32);
		}
		return offset;
	}

	int getPartition(int max_num_partitions){
		if( values_length > 0 ){
		return values[0]%max_num_partitions;
		} else return 0;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // Comparing vector values
			size_t _length1,_length2;
			*offset1 += ss::staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += ss::staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing values[i]
					if  ( *((ss_uint32*)(data1 + (*offset1))) < *((ss_uint32*)(data2 + (*offset2))) ) return -1;
					if  ( *((ss_uint32*)(data1 + (*offset1))) > *((ss_uint32*)(data2 + (*offset2))) ) return 1;
					*offset1 +=sizeof(ss_uint32);
					*offset2 +=sizeof(ss_uint32);
				}
			}
		}
		return 0; //If everything is equal
	}

	void valuesSetLength(int _length){
		if( _length > values_max_length){ 
			ss_uint32 *_previous = values;
			int previous_length = values_length;
			if(values_max_length == 0) values_max_length = _length;
			while(values_max_length < _length) values_max_length *= 2;
			values = (ss_uint32*) malloc( sizeof(ss_uint32) * values_max_length );
			if( _previous ){
				memcpy(values, _previous, previous_length*sizeof(ss_uint32));
				free( _previous );
			}
		}
		values_length=_length;
	}

	void valuesAdd(ss_uint32 _value){
		valuesSetLength( values_length + 1 );
		values[values_length-1] = _value;
	}

	void copyFrom( Vector_UInt32 *other ){
			{ // CopyFrom field values
				valuesSetLength( other->values_length);
				for (int i = 0 ; i < values_length ; i++){
					values[i]=other->values[i];
				}
			}
	};

	std::string str(){
		std::ostringstream o;
		{// toString of vector values
			for(int i = 0 ; i < values_length ; i++){
				o << values[i];
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

}; //class Vector_UInt32

class Vector_UInt64 : public ss::DataInstance{

public:
	ss_uint64 *values;
	int values_length;
	int values_max_length;


	Vector_UInt64() : ss::DataInstance(){
		values_length=0;
		values_max_length=0;
		values = NULL;
	}

	static DataInstance *createFunction(){
		return new Vector_UInt64();
	}
	inline static bool compareByValue( KV* kv1 , KV* kv2)
	{
	size_t offset_1 = 0;
	size_t offset_2 = 0;
	int c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );
	return (c<0);
	}
	~Vector_UInt64() {
		if( values )
			free( values );
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector values
			size_t _length;
			offset += ss::staticVarIntParse( data+offset , &_length );
		 	valuesSetLength( _length );
			for (int i = 0 ; i < (int)values_length ; i++){
				values[i]=  *( (ss_uint64*) (data+offset) ); offset +=  sizeof(ss_uint64);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector values
			offset += ss::staticVarIntSerialize( data+offset , values_length );
			for (int i = 0 ; i < (int)values_length ; i++){
				*( (ss_uint64*) (data+offset) )=values[i]; offset +=  sizeof(ss_uint64);
			}
		}
		return offset;
	}

	int size(char *data){
		int offset=0;
		{ //Getting size of vector values
			size_t _length;
			offset += ss::staticVarIntParse( data+offset , &_length );
			offset += _length*sizeof(ss_uint64);
		}
		return offset;
	}

	int getPartition(int max_num_partitions){
		if( values_length > 0 ){
		return values[0]%max_num_partitions;
		} else return 0;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // Comparing vector values
			size_t _length1,_length2;
			*offset1 += ss::staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += ss::staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing values[i]
					if  ( *((ss_uint64*)(data1 + (*offset1))) < *((ss_uint64*)(data2 + (*offset2))) ) return -1;
					if  ( *((ss_uint64*)(data1 + (*offset1))) > *((ss_uint64*)(data2 + (*offset2))) ) return 1;
					*offset1 +=sizeof(ss_uint64);
					*offset2 +=sizeof(ss_uint64);
				}
			}
		}
		return 0; //If everything is equal
	}

	void valuesSetLength(int _length){
		if( _length > values_max_length){ 
			ss_uint64 *_previous = values;
			int previous_length = values_length;
			if(values_max_length == 0) values_max_length = _length;
			while(values_max_length < _length) values_max_length *= 2;
			values = (ss_uint64*) malloc( sizeof(ss_uint64) * values_max_length );
			if( _previous ){
				memcpy(values, _previous, previous_length*sizeof(ss_uint64));
				free( _previous );
			}
		}
		values_length=_length;
	}

	void valuesAdd(ss_uint64 _value){
		valuesSetLength( values_length + 1 );
		values[values_length-1] = _value;
	}

	void copyFrom( Vector_UInt64 *other ){
			{ // CopyFrom field values
				valuesSetLength( other->values_length);
				for (int i = 0 ; i < values_length ; i++){
					values[i]=other->values[i];
				}
			}
	};

	std::string str(){
		std::ostringstream o;
		{// toString of vector values
			for(int i = 0 ; i < values_length ; i++){
				o << values[i];
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

}; //class Vector_UInt64


// Operations defined in this module

#pragma mark CLASS example_map : map example_map


class example_map : public ss::Map {

	public:


	//Main function to implement
	void run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs );

	std::string help(){
		std::ostringstream o;
		o<<"This is the full help for this command\n";
		o<<"Multiple lines are available\n";
		return o.str();
	}


	// Implement this functions:
	//void ss::base::example_map::run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs ){};
};

// Module definition

class Module : public ss::Module{

public:
	Module() : ss::Module("base","0.1","Andreu Urruela"){


		//Add datas
	{
		Data *data = new Data( "UInt" , UInt::createFunction );
		data->setCompareByValueFunction( UInt::compareByValue );
		add( data );
	}
	{
		Data *data = new Data( "UInt2" , UInt2::createFunction );
		data->setCompareByValueFunction( UInt2::compareByValue );
		add( data );
	}
	{
		Data *data = new Data( "Float" , Float::createFunction );
		data->setCompareByValueFunction( Float::compareByValue );
		add( data );
	}
	{
		Data *data = new Data( "Uint32" , Uint32::createFunction );
		data->setCompareByValueFunction( Uint32::compareByValue );
		add( data );
	}
	{
		Data *data = new Data( "Uint64" , Uint64::createFunction );
		data->setCompareByValueFunction( Uint64::compareByValue );
		add( data );
	}
	{
		Data *data = new Data( "Vector_UInt" , Vector_UInt::createFunction );
		data->setCompareByValueFunction( Vector_UInt::compareByValue );
		add( data );
	}
	{
		Data *data = new Data( "Vector_UInt32" , Vector_UInt32::createFunction );
		data->setCompareByValueFunction( Vector_UInt32::compareByValue );
		add( data );
	}
	{
		Data *data = new Data( "Vector_UInt64" , Vector_UInt64::createFunction );
		data->setCompareByValueFunction( Vector_UInt64::compareByValue );
		add( data );
	}


		//Add operatons
		{
		ss::Operation * operation = new ss::Operation( "example_map" , ss::Operation::map , au::factory<example_map> );
		operation->inputFormats.push_back( ss::KVFormat::format("base.UInt" ,"base.UInt") );
		operation->outputFormats.push_back( ss::KVFormat::format("base.UInt" ,"base.UInt") );
		operation->setHelpLine("A bit of help for this operation");
		std::ostringstream o;
		o << "This is the full help for this command\n";
		o << "Multiple lines are available\n";
		operation->setHelp( o.str() );
		add( operation );
		}
	}

	std::string help(){
		std::ostringstream o;
		return o.str();
	}
};

}


} // end of namespace ss

#endif


// List of functions to be implemented:

//example_map
//void ss::base::example_map::run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs ){};

