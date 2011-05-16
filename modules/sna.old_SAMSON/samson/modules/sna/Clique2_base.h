
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_sna_Clique2_BASE
#define _H_SAMSON_sna_Clique2_BASE


#include <samson/modules/sna/Clique.h>


namespace ss{
namespace sna{


	class Clique2_base : public ss::DataInstance{

	public:
	::ss::sna::Clique clique_1;
	::ss::sna::Clique clique_2;

	Clique2_base() : ss::DataInstance(){
	}

	~Clique2_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += clique_1.parse(data+offset);
		offset += clique_2.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += clique_1.serialize(data+offset);
		offset += clique_2.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::sna::Clique::size(data+offset);
		offset += ::ss::sna::Clique::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return clique_1.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing clique_1
			int tmp = ::ss::sna::Clique::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing clique_2
			int tmp = ::ss::sna::Clique::compare(data1,data2,offset1 , offset2);
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

	void copyFrom( Clique2_base *other ){
		clique_1.copyFrom(&other->clique_1);
		clique_2.copyFrom(&other->clique_2);
	};

	std::string str(){
		std::ostringstream o;
		o << clique_1.str();

		o<<" ";
		o << clique_2.str();

		o<<" ";
		return o.str();
	}

	}; //class Clique2_base

} // end of namespace ss
} // end of namespace sna

#endif
