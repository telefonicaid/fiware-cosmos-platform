
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_url_PathCount_BASE
#define _H_SAMSON_url_PathCount_BASE


#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>


namespace ss{
namespace url{


	class PathCount_base : public ss::DataInstance{

	public:
	::ss::system::String path;
	::ss::system::UInt count;

	PathCount_base() : ss::DataInstance(){
	}

	~PathCount_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += path.parse(data+offset);
		offset += count.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += path.serialize(data+offset);
		offset += count.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::system::String::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return path.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing path
			int tmp = ::ss::system::String::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
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

	void copyFrom( PathCount_base *other ){
		path.copyFrom(&other->path);
		count.copyFrom(&other->count);
	};

	std::string str(){
		std::ostringstream o;
		o << path.str();

		o<<" ";
		o << count.str();

		o<<" ";
		return o.str();
	}

	}; //class PathCount_base

} // end of namespace ss
} // end of namespace url

#endif
