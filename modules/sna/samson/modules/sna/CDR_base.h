
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_sna_CDR_BASE
#define _H_SAMSON_sna_CDR_BASE


#include <samson/modules/system/Time.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/UInt8.h>


namespace samson{
namespace sna{


	class CDR_base : public samson::DataInstance{

	public:
	::samson::system::UInt node;
	::samson::system::Time time;
	::samson::system::UInt duration;
	::samson::system::UInt8 type;
	::samson::system::UInt8 week_day;
	::samson::system::UInt8 dir;

	CDR_base() : samson::DataInstance(){
	}

	~CDR_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += node.parse(data+offset);
		offset += time.parse(data+offset);
		offset += duration.parse(data+offset);
		offset += type.parse(data+offset);
		offset += week_day.parse(data+offset);
		offset += dir.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += node.serialize(data+offset);
		offset += time.serialize(data+offset);
		offset += duration.serialize(data+offset);
		offset += type.serialize(data+offset);
		offset += week_day.serialize(data+offset);
		offset += dir.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::samson::system::UInt::size(data+offset);
		offset += ::samson::system::Time::size(data+offset);
		offset += ::samson::system::UInt::size(data+offset);
		offset += ::samson::system::UInt8::size(data+offset);
		offset += ::samson::system::UInt8::size(data+offset);
		offset += ::samson::system::UInt8::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return node.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing node
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing time
			int tmp = ::samson::system::Time::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing duration
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing type
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing week_day
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing dir
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
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

	void copyFrom( CDR_base *other ){
		node.copyFrom(&other->node);
		time.copyFrom(&other->time);
		duration.copyFrom(&other->duration);
		type.copyFrom(&other->type);
		week_day.copyFrom(&other->week_day);
		dir.copyFrom(&other->dir);
	};

	std::string str(){
		std::ostringstream o;
		o << node.str();

		o<<" ";
		o << time.str();

		o<<" ";
		o << duration.str();

		o<<" ";
		o << type.str();

		o<<" ";
		o << week_day.str();

		o<<" ";
		o << dir.str();

		o<<" ";
		return o.str();
	}

	}; //class CDR_base

} // end of namespace samson
} // end of namespace sna

#endif
