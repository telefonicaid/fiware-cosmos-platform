
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_url_ServerCountVector_BASE
#define _H_SAMSON_url_ServerCountVector_BASE


#include <samson/modules/system/UInt.h>
#include <samson/modules/url/ServerCount.h>


namespace samson{
namespace url{


	class ServerCountVector_base : public samson::DataInstance{

	public:
	::samson::url::ServerCount *serversCount;
	int serversCount_length;
	int serversCount_max_length;
	::samson::system::UInt countRef;

	ServerCountVector_base() : samson::DataInstance(){
		serversCount_length=0;
		serversCount_max_length=0;
		serversCount = NULL;
	}

	~ServerCountVector_base() {
		if( serversCount )
			delete[] serversCount ;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector serversCount
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
		 	serversCountSetLength( _length );
			for (int i = 0 ; i < (int)serversCount_length ; i++){
				offset += serversCount[i].parse(data+offset);
			}
		}
		offset += countRef.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector serversCount
			offset += samson::staticVarIntSerialize( data+offset , serversCount_length );
			for (int i = 0 ; i < (int)serversCount_length ; i++){
				offset += serversCount[i].serialize(data+offset);
			}
		}
		offset += countRef.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Getting size of vector serversCount
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
			::samson::url::ServerCount _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += ::samson::url::ServerCount::size(data+offset);
			}
		}
		offset += ::samson::system::UInt::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		if( serversCount_length > 0 ){
		return serversCount[0].hash(max_num_partitions);
		} else return 0;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // Comparing vector serversCount
			size_t _length1,_length2;
			*offset1 += samson::staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += samson::staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing serversCount[i]
					int tmp = ::samson::url::ServerCount::compare(data1,data2,offset1 , offset2);
					if( tmp != 0) return tmp;
				}
			}
		}
		{ // comparing countRef
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
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

	void serversCountSetLength(int _length){
		if( _length > serversCount_max_length){ 
			::samson::url::ServerCount *_previous = serversCount;
			int previous_length = serversCount_length;
			if(serversCount_max_length == 0) serversCount_max_length = _length;
			while(serversCount_max_length < _length) serversCount_max_length *= 2;
			serversCount = new ::samson::url::ServerCount[serversCount_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					serversCount[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		serversCount_length=_length;
	}

	::samson::url::ServerCount* serversCountAdd(){
		serversCountSetLength( serversCount_length + 1 );
		return &serversCount[serversCount_length-1];
	}

	void copyFrom( ServerCountVector_base *other ){
			{ // CopyFrom field serversCount
				serversCountSetLength( other->serversCount_length);
				for (int i = 0 ; i < serversCount_length ; i++){
					serversCount[i].copyFrom(&other->serversCount[i]);
				}
			}
		countRef.copyFrom(&other->countRef);
	};

	std::string str(){
		std::ostringstream o;
		{// toString of vector serversCount
			for(int i = 0 ; i < serversCount_length ; i++){
				o << serversCount[i].str();
				 o << " ";
			}
		}
		o<<" ";
		o << countRef.str();

		o<<" ";
		return o.str();
	}

	}; //class ServerCountVector_base

} // end of namespace samson
} // end of namespace url

#endif
