
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_sna_Vector_Clique_BASE
#define _H_SAMSON_sna_Vector_Clique_BASE


#include <samson/modules/sna/Clique.h>


namespace samson{
namespace sna{


	class Vector_Clique_base : public samson::DataInstance{

	public:
	::samson::sna::Clique *items;
	int items_length;
	int items_max_length;

	Vector_Clique_base() : samson::DataInstance(){
		items_length=0;
		items_max_length=0;
		items = NULL;
	}

	~Vector_Clique_base() {
		if( items )
			delete[] items ;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector items
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
		 	itemsSetLength( _length );
			for (int i = 0 ; i < (int)items_length ; i++){
				offset += items[i].parse(data+offset);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector items
			offset += samson::staticVarIntSerialize( data+offset , items_length );
			for (int i = 0 ; i < (int)items_length ; i++){
				offset += items[i].serialize(data+offset);
			}
		}
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Getting size of vector items
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
			::samson::sna::Clique _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += ::samson::sna::Clique::size(data+offset);
			}
		}
		return offset;
	}

	int hash(int max_num_partitions){
		if( items_length > 0 ){
		return items[0].hash(max_num_partitions);
		} else return 0;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // Comparing vector items
			size_t _length1,_length2;
			*offset1 += samson::staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += samson::staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing items[i]
					int tmp = ::samson::sna::Clique::compare(data1,data2,offset1 , offset2);
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

	void itemsSetLength(int _length){
		if( _length > items_max_length){ 
			::samson::sna::Clique *_previous = items;
			int previous_length = items_length;
			if(items_max_length == 0) items_max_length = _length;
			while(items_max_length < _length) items_max_length *= 2;
			items = new ::samson::sna::Clique[items_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					items[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		items_length=_length;
	}

	::samson::sna::Clique* itemsAdd(){
		itemsSetLength( items_length + 1 );
		return &items[items_length-1];
	}

	void copyFrom( Vector_Clique_base *other ){
			{ // CopyFrom field items
				itemsSetLength( other->items_length);
				for (int i = 0 ; i < items_length ; i++){
					items[i].copyFrom(&other->items[i]);
				}
			}
	};

	std::string str(){
		std::ostringstream o;
		{// toString of vector items
			for(int i = 0 ; i < items_length ; i++){
				o << items[i].str();
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

	}; //class Vector_Clique_base

} // end of namespace samson
} // end of namespace sna

#endif
