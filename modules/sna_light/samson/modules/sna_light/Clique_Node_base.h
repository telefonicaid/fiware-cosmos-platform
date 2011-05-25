
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_sna_light_Clique_Node_BASE
#define _H_SAMSON_sna_light_Clique_Node_BASE


#include <samson/modules/sna_light/Clique.h>
#include <samson/modules/sna_light/Clique_Link.h>


namespace samson{
namespace sna_light{


	class Clique_Node_base : public samson::DataInstance{

	public:
	::samson::sna_light::Clique clique;
	::samson::sna_light::Clique_Link *links;
	int links_length;
	int links_max_length;

	Clique_Node_base() : samson::DataInstance(){
		links_length=0;
		links_max_length=0;
		links = NULL;
	}

	~Clique_Node_base() {
		if( links )
			delete[] links ;
	}

	int parse(char *data){
		int offset=0;
		offset += clique.parse(data+offset);
		{ //Parsing vector links
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
		 	linksSetLength( _length );
			for (int i = 0 ; i < (int)links_length ; i++){
				offset += links[i].parse(data+offset);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += clique.serialize(data+offset);
		{ //Serialization vector links
			offset += samson::staticVarIntSerialize( data+offset , links_length );
			for (int i = 0 ; i < (int)links_length ; i++){
				offset += links[i].serialize(data+offset);
			}
		}
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::samson::sna_light::Clique::size(data+offset);
		{ //Getting size of vector links
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
			::samson::sna_light::Clique_Link _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += ::samson::sna_light::Clique_Link::size(data+offset);
			}
		}
		return offset;
	}

	int hash(int max_num_partitions){
		return clique.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing clique
			int tmp = ::samson::sna_light::Clique::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // Comparing vector links
			size_t _length1,_length2;
			*offset1 += samson::staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += samson::staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing links[i]
					int tmp = ::samson::sna_light::Clique_Link::compare(data1,data2,offset1 , offset2);
					if( tmp != 0) return tmp;
				}
			}
		}
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}

	void linksSetLength(int _length){
		if( _length > links_max_length){ 
			::samson::sna_light::Clique_Link *_previous = links;
			int previous_length = links_length;
			if(links_max_length == 0) links_max_length = _length;
			while(links_max_length < _length) links_max_length *= 2;
			links = new ::samson::sna_light::Clique_Link[links_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					links[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		links_length=_length;
	}

	::samson::sna_light::Clique_Link* linksAdd(){
		linksSetLength( links_length + 1 );
		return &links[links_length-1];
	}

	void copyFrom( Clique_Node_base *other ){
		clique.copyFrom(&other->clique);
			{ // CopyFrom field links
				linksSetLength( other->links_length);
				for (int i = 0 ; i < links_length ; i++){
					links[i].copyFrom(&other->links[i]);
				}
			}
	};

	std::string str(){
		std::ostringstream o;
		o << clique.str();

		o<<" ";
		{// toString of vector links
			for(int i = 0 ; i < links_length ; i++){
				o << links[i].str();
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

	}; //class Clique_Node_base

} // end of namespace samson
} // end of namespace sna_light

#endif
