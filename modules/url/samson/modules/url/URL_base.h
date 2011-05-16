
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_url_URL_BASE
#define _H_SAMSON_url_URL_BASE


#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/UInt8.h>


namespace ss{
namespace url{


	class URL_base : public ss::DataInstance{

	public:
	::ss::system::String server;
	::ss::system::UInt8 type;
	::ss::system::String path;
	::ss::system::UInt user;

	URL_base() : ss::DataInstance(){
	}

	~URL_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += server.parse(data+offset);
		offset += type.parse(data+offset);
		offset += path.parse(data+offset);
		offset += user.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += server.serialize(data+offset);
		offset += type.serialize(data+offset);
		offset += path.serialize(data+offset);
		offset += user.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::system::String::size(data+offset);
		offset += ::ss::system::UInt8::size(data+offset);
		offset += ::ss::system::String::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return server.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing server
			int tmp = ::ss::system::String::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing type
			int tmp = ::ss::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing path
			int tmp = ::ss::system::String::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing user
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

	void copyFrom( URL_base *other ){
		server.copyFrom(&other->server);
		type.copyFrom(&other->type);
		path.copyFrom(&other->path);
		user.copyFrom(&other->user);
	};

	std::string str(){
		std::ostringstream o;
		o << server.str();

		o<<" ";
		o << type.str();

		o<<" ";
		o << path.str();

		o<<" ";
		o << user.str();

		o<<" ";
		return o.str();
	}

	}; //class URL_base

} // end of namespace ss
} // end of namespace url

#endif
