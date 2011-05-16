
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_url_ServerCount_BASE
#define _H_SAMSON_url_ServerCount_BASE


#include <samson/modules/system/UInt.h>


namespace ss{
namespace url{


	class ServerCount_base : public ss::DataInstance{

	public:
	::ss::system::UInt server;
	::ss::system::UInt count;

	ServerCount_base() : ss::DataInstance(){
	}

	~ServerCount_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += server.parse(data+offset);
		offset += count.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += server.serialize(data+offset);
		offset += count.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return server.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing server
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
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

	void copyFrom( ServerCount_base *other ){
		server.copyFrom(&other->server);
		count.copyFrom(&other->count);
	};

	std::string str(){
		std::ostringstream o;
		o << server.str();

		o<<" ";
		o << count.str();

		o<<" ";
		return o.str();
	}

	}; //class ServerCount_base

} // end of namespace ss
} // end of namespace url

#endif
