
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_url_ServerPathVector_BASE
#define _H_SAMSON_url_ServerPathVector_BASE


#include <samson/modules/system/UInt.h>
#include <samson/modules/url/ServerPathCount.h>


namespace samson{
namespace url{


	class ServerPathVector_base : public samson::DataInstance{

	public:
	::samson::url::ServerPathCount *serverPath;
	int serverPath_length;
	int serverPath_max_length;
	::samson::system::UInt countRef;

	ServerPathVector_base() : samson::DataInstance(){
		serverPath_length=0;
		serverPath_max_length=0;
		serverPath = NULL;
	}

	~ServerPathVector_base() {
		if( serverPath )
			delete[] serverPath ;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector serverPath
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
		 	serverPathSetLength( _length );
			for (int i = 0 ; i < (int)serverPath_length ; i++){
				offset += serverPath[i].parse(data+offset);
			}
		}
		offset += countRef.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector serverPath
			offset += samson::staticVarIntSerialize( data+offset , serverPath_length );
			for (int i = 0 ; i < (int)serverPath_length ; i++){
				offset += serverPath[i].serialize(data+offset);
			}
		}
		offset += countRef.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Getting size of vector serverPath
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
			::samson::url::ServerPathCount _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += ::samson::url::ServerPathCount::size(data+offset);
			}
		}
		offset += ::samson::system::UInt::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		if( serverPath_length > 0 ){
		return serverPath[0].hash(max_num_partitions);
		} else return 0;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // Comparing vector serverPath
			size_t _length1,_length2;
			*offset1 += samson::staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += samson::staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing serverPath[i]
					int tmp = ::samson::url::ServerPathCount::compare(data1,data2,offset1 , offset2);
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

	void serverPathSetLength(int _length){
		if( _length > serverPath_max_length){ 
			::samson::url::ServerPathCount *_previous = serverPath;
			int previous_length = serverPath_length;
			if(serverPath_max_length == 0) serverPath_max_length = _length;
			while(serverPath_max_length < _length) serverPath_max_length *= 2;
			serverPath = new ::samson::url::ServerPathCount[serverPath_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					serverPath[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		serverPath_length=_length;
	}

	::samson::url::ServerPathCount* serverPathAdd(){
		serverPathSetLength( serverPath_length + 1 );
		return &serverPath[serverPath_length-1];
	}

	void copyFrom( ServerPathVector_base *other ){
			{ // CopyFrom field serverPath
				serverPathSetLength( other->serverPath_length);
				for (int i = 0 ; i < serverPath_length ; i++){
					serverPath[i].copyFrom(&other->serverPath[i]);
				}
			}
		countRef.copyFrom(&other->countRef);
	};

	std::string str(){
		std::ostringstream o;
		{// toString of vector serverPath
			for(int i = 0 ; i < serverPath_length ; i++){
				o << serverPath[i].str();
				 o << " ";
			}
		}
		o<<" ";
		o << countRef.str();

		o<<" ";
		return o.str();
	}

	}; //class ServerPathVector_base

} // end of namespace samson
} // end of namespace url

#endif
