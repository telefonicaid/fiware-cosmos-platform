
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_osn_Date_BASE
#define _H_SAMSON_osn_Date_BASE


#include <samson/modules/system/UInt.h>
#include <samson/modules/system/UInt8.h>


namespace ss{
namespace osn{


	class Date_base : public ss::DataInstance{

	public:
	::ss::system::UInt8 year;
	::ss::system::UInt8 month;
	::ss::system::UInt8 day;
	::ss::system::UInt days_2000;
	::ss::system::UInt8 week_day;

	Date_base() : ss::DataInstance(){
	}

	~Date_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += year.parse(data+offset);
		offset += month.parse(data+offset);
		offset += day.parse(data+offset);
		offset += days_2000.parse(data+offset);
		offset += week_day.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += year.serialize(data+offset);
		offset += month.serialize(data+offset);
		offset += day.serialize(data+offset);
		offset += days_2000.serialize(data+offset);
		offset += week_day.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::system::UInt8::size(data+offset);
		offset += ::ss::system::UInt8::size(data+offset);
		offset += ::ss::system::UInt8::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt8::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return year.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing year
			int tmp = ::ss::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing month
			int tmp = ::ss::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing day
			int tmp = ::ss::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing days_2000
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing week_day
			int tmp = ::ss::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}

	void copyFrom( Date_base *other ){
		year.copyFrom(&other->year);
		month.copyFrom(&other->month);
		day.copyFrom(&other->day);
		days_2000.copyFrom(&other->days_2000);
		week_day.copyFrom(&other->week_day);
	};

	std::string str(){
		std::ostringstream o;
		o << year.str();

		o<<" ";
		o << month.str();

		o<<" ";
		o << day.str();

		o<<" ";
		o << days_2000.str();

		o<<" ";
		o << week_day.str();

		o<<" ";
		return o.str();
	}

	}; //class Date_base

} // end of namespace ss
} // end of namespace osn

#endif
