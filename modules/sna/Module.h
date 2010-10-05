/*

	Module SNA operations (cdrs, graph, cliques, communities,...) (sna)

	File: /mnt/sda9/kzangeli/sb/samson/01/trunk/modules/sna/Module.h
	NOTE: This file has been generated with the samson_module tool, please do not modify

*/

#ifndef _H_SS_sna_Module
#define _H_SS_sna_Module


#include <samson/samson_lib.h>



namespace ss
{
namespace sna{

// Datas defined in this module

class Date : public ss::DataInstance{

public:
	ss_uint8 year;
	ss_uint8 month;
	ss_uint8 day;
	ss_uint8 week_day;


	Date() : ss::DataInstance(){
	}

	static DataInstance *createFunction(){
		return new Date();
	}
	inline static bool compareByValue( KV* kv1 , KV* kv2)
	{
	size_t offset_1 = 0;
	size_t offset_2 = 0;
	int c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );
	return (c<0);
	}
	~Date() {
	}

	int parse(char *data){
		int offset=0;
		year=  *( (ss_uint8*) (data+offset) ); offset +=  sizeof(ss_uint8);
		month=  *( (ss_uint8*) (data+offset) ); offset +=  sizeof(ss_uint8);
		day=  *( (ss_uint8*) (data+offset) ); offset +=  sizeof(ss_uint8);
		week_day=  *( (ss_uint8*) (data+offset) ); offset +=  sizeof(ss_uint8);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		*( (ss_uint8*) (data+offset) )=year; offset +=  sizeof(ss_uint8);
		*( (ss_uint8*) (data+offset) )=month; offset +=  sizeof(ss_uint8);
		*( (ss_uint8*) (data+offset) )=day; offset +=  sizeof(ss_uint8);
		*( (ss_uint8*) (data+offset) )=week_day; offset +=  sizeof(ss_uint8);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += sizeof(ss_uint8);
		offset += sizeof(ss_uint8);
		offset += sizeof(ss_uint8);
		offset += sizeof(ss_uint8);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return year%max_num_partitions;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing year
			if  ( *((ss_uint8*)(data1 + (*offset1))) < *((ss_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((ss_uint8*)(data1 + (*offset1))) > *((ss_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(ss_uint8);
			*offset2 +=sizeof(ss_uint8);
		}
		{ // comparing month
			if  ( *((ss_uint8*)(data1 + (*offset1))) < *((ss_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((ss_uint8*)(data1 + (*offset1))) > *((ss_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(ss_uint8);
			*offset2 +=sizeof(ss_uint8);
		}
		{ // comparing day
			if  ( *((ss_uint8*)(data1 + (*offset1))) < *((ss_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((ss_uint8*)(data1 + (*offset1))) > *((ss_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(ss_uint8);
			*offset2 +=sizeof(ss_uint8);
		}
		{ // comparing week_day
			if  ( *((ss_uint8*)(data1 + (*offset1))) < *((ss_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((ss_uint8*)(data1 + (*offset1))) > *((ss_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(ss_uint8);
			*offset2 +=sizeof(ss_uint8);
		}
		return 0; //If everything is equal
	}

	void copyFrom( Date *other ){
		year=other->year;
		month=other->month;
		day=other->day;
		week_day=other->week_day;
	};

	std::string str(){
		std::ostringstream o;
		o << (int)year;

		o<<" ";
		o << (int)month;

		o<<" ";
		o << (int)day;

		o<<" ";
		o << (int)week_day;

		o<<" ";
		return o.str();
	}

}; //class Date

class Time : public ss::DataInstance{

public:
	ss_uint8 hour;
	ss_uint8 minute;
	ss_uint8 seconds;


	Time() : ss::DataInstance(){
	}

	static DataInstance *createFunction(){
		return new Time();
	}
	inline static bool compareByValue( KV* kv1 , KV* kv2)
	{
	size_t offset_1 = 0;
	size_t offset_2 = 0;
	int c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );
	return (c<0);
	}
	~Time() {
	}

	int parse(char *data){
		int offset=0;
		hour=  *( (ss_uint8*) (data+offset) ); offset +=  sizeof(ss_uint8);
		minute=  *( (ss_uint8*) (data+offset) ); offset +=  sizeof(ss_uint8);
		seconds=  *( (ss_uint8*) (data+offset) ); offset +=  sizeof(ss_uint8);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		*( (ss_uint8*) (data+offset) )=hour; offset +=  sizeof(ss_uint8);
		*( (ss_uint8*) (data+offset) )=minute; offset +=  sizeof(ss_uint8);
		*( (ss_uint8*) (data+offset) )=seconds; offset +=  sizeof(ss_uint8);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += sizeof(ss_uint8);
		offset += sizeof(ss_uint8);
		offset += sizeof(ss_uint8);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return hour%max_num_partitions;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing hour
			if  ( *((ss_uint8*)(data1 + (*offset1))) < *((ss_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((ss_uint8*)(data1 + (*offset1))) > *((ss_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(ss_uint8);
			*offset2 +=sizeof(ss_uint8);
		}
		{ // comparing minute
			if  ( *((ss_uint8*)(data1 + (*offset1))) < *((ss_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((ss_uint8*)(data1 + (*offset1))) > *((ss_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(ss_uint8);
			*offset2 +=sizeof(ss_uint8);
		}
		{ // comparing seconds
			if  ( *((ss_uint8*)(data1 + (*offset1))) < *((ss_uint8*)(data2 + (*offset2))) ) return -1;
			if  ( *((ss_uint8*)(data1 + (*offset1))) > *((ss_uint8*)(data2 + (*offset2))) ) return 1;
			*offset1 +=sizeof(ss_uint8);
			*offset2 +=sizeof(ss_uint8);
		}
		return 0; //If everything is equal
	}

	void copyFrom( Time *other ){
		hour=other->hour;
		minute=other->minute;
		seconds=other->seconds;
	};

	std::string str(){
		std::ostringstream o;
		o << (int)hour;

		o<<" ";
		o << (int)minute;

		o<<" ";
		o << (int)seconds;

		o<<" ";
		return o.str();
	}

}; //class Time

class CDR : public ss::DataInstance{

public:
	ss_uint node;
	::ss::sna::Date date;
	Time time;
	ss_uint duration;


	CDR() : ss::DataInstance(){
	}

	static DataInstance *createFunction(){
		return new CDR();
	}
	inline static bool compareByValue( KV* kv1 , KV* kv2)
	{
	size_t offset_1 = 0;
	size_t offset_2 = 0;
	int c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );
	return (c<0);
	}
	~CDR() {
	}

	int parse(char *data){
		int offset=0;
		offset += ss::staticVarIntParse( data+offset , &node);
		offset += date.parse(data+offset);
		offset += time.parse(data+offset);
		offset += ss::staticVarIntParse( data+offset , &duration);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += ss::staticVarIntSerialize( data+offset , node);
		offset += date.serialize(data+offset);
		offset += time.serialize(data+offset);
		offset += ss::staticVarIntSerialize( data+offset , duration);
		return offset;
	}

	int size(char *data){
		int offset=0;
		offset += ss::staticVarIntParse( data+offset , &node);
		offset += date.size(data+offset);
		offset += time.size(data+offset);
		offset += ss::staticVarIntParse( data+offset , &duration);
		return offset;
	}

	int getPartition(int max_num_partitions){
		return node%max_num_partitions;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing node
			ss_uint _value1;
			ss_uint _value2;
			*offset1 += ss::staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += ss::staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		{ // comparing date
			::ss::sna::Date _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing time
			Time _tmp_data;
			int tmp = _tmp_data.compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing duration
			ss_uint _value1;
			ss_uint _value2;
			*offset1 += ss::staticVarIntParse(data1 + (*offset1), &_value1);
			*offset2 += ss::staticVarIntParse(data2 + (*offset2), &_value2);
			if( _value1 < _value2 ) return -1;
			if( _value1 > _value2 ) return  1;
		}
		return 0; //If everything is equal
	}

	void copyFrom( CDR *other ){
		node=other->node;
		date.copyFrom(&other->date);
		time.copyFrom(&other->time);
		duration=other->duration;
	};

	std::string str(){
		std::ostringstream o;
		o << node;

		o<<" ";
		o << date.str();

		o<<" ";
		o << time.str();

		o<<" ";
		o << duration;

		o<<" ";
		return o.str();
	}

}; //class CDR


// Operations defined in this module

#pragma mark CLASS parse_cdrs : map parse_cdrs


class parse_cdrs : public ss::Map {

	public:


	//Main function to implement
	void run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs );

	std::string help(){
		std::ostringstream o;
		return o.str();
	}


	// Implement this functions:
	//void ss::sna::parse_cdrs::run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs ){};
};
#pragma mark CLASS spread_cdrs_to_links : map spread_cdrs_to_links


class spread_cdrs_to_links : public ss::Map {

	public:


	//Main function to implement
	void run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs );

	std::string help(){
		std::ostringstream o;
		o<<"Spread cdrs at the input in the sense that emit two key-values at the output for each cdr.\n";
		o<<"For every CDR (A-B) two key-values are emitted A,B and B,A\n";
		o<<"This operation is tippically concatenated with sna_compute_graph to generate the graph from cdrs.\n";
		return o.str();
	}


	// Implement this functions:
	//void ss::sna::spread_cdrs_to_links::run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs ){};
};
#pragma mark CLASS accumulate_links : reduce accumulate_links


class accumulate_links : public ss::Reduce {

	public:


	//Main function to implement
	void run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs );

	std::string help(){
		std::ostringstream o;
		return o.str();
	}


	// Implement this functions:
	//void ss::sna::accumulate_links::run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs ){};
};

// Module definition

class Module : public ss::Module{

public:
	Module() : ss::Module("sna","0.1","Andreu Urruela"){


		//Add datas
	{
		Data *data = new Data( "Date" , Date::createFunction );
		data->setCompareByValueFunction( Date::compareByValue );
		add( data );
	}
	{
		Data *data = new Data( "Time" , Time::createFunction );
		data->setCompareByValueFunction( Time::compareByValue );
		add( data );
	}
	{
		Data *data = new Data( "CDR" , CDR::createFunction );
		data->setCompareByValueFunction( CDR::compareByValue );
		add( data );
	}


		//Add operatons
		{
		ss::Operation * operation = new ss::Operation( "parse_cdrs" , ss::Operation::map , au::factory<parse_cdrs> );
		operation->inputFormats.push_back( ss::KVFormat::format("system.String" ,"system.Void") );
		operation->outputFormats.push_back( ss::KVFormat::format("base.UInt" ,"sna.CDR") );
		operation->setHelpLine("");
		std::ostringstream o;
		operation->setHelp( o.str() );
		add( operation );
		}
		{
		ss::Operation * operation = new ss::Operation( "spread_cdrs_to_links" , ss::Operation::map , au::factory<spread_cdrs_to_links> );
		operation->inputFormats.push_back( ss::KVFormat::format("base.UInt" ,"sna.CDR") );
		operation->outputFormats.push_back( ss::KVFormat::format("base.UInt2" ,"system.Void") );
		operation->setHelpLine("");
		std::ostringstream o;
		o << "Spread cdrs at the input in the sense that emit two key-values at the output for each cdr.\n";
		o << "For every CDR (A-B) two key-values are emitted A,B and B,A\n";
		o << "This operation is tippically concatenated with sna_compute_graph to generate the graph from cdrs.\n";
		operation->setHelp( o.str() );
		add( operation );
		}
		{
		ss::Operation * operation = new ss::Operation( "accumulate_links" , ss::Operation::reduce , au::factory<accumulate_links> );
		operation->inputFormats.push_back( ss::KVFormat::format("base.UInt2" ,"system.Void") );
		operation->inputFormats.push_back( ss::KVFormat::format("base.UInt2" ,"base.UInt") );
		operation->outputFormats.push_back( ss::KVFormat::format("base.UInt2" ,"base.UInt") );
		operation->setHelpLine("");
		std::ostringstream o;
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

//parse_cdrs
//void ss::sna::parse_cdrs::run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs ){};

//spread_cdrs_to_links
//void ss::sna::spread_cdrs_to_links::run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs ){};

//accumulate_links
//void ss::sna::accumulate_links::run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs ){};

