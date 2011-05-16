
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_url_CategoryVector_BASE
#define _H_SAMSON_url_CategoryVector_BASE


#include <samson/modules/system/UInt8.h>
#include <samson/modules/url/Category.h>


namespace ss{
namespace url{


	class CategoryVector_base : public ss::DataInstance{

	public:
	::ss::url::Category *category;
	int category_length;
	int category_max_length;
	::ss::system::UInt8 fixed;

	CategoryVector_base() : ss::DataInstance(){
		category_length=0;
		category_max_length=0;
		category = NULL;
	}

	~CategoryVector_base() {
		if( category )
			delete[] category ;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector category
			size_t _length;
			offset += ss::staticVarIntParse( data+offset , &_length );
		 	categorySetLength( _length );
			for (int i = 0 ; i < (int)category_length ; i++){
				offset += category[i].parse(data+offset);
			}
		}
		offset += fixed.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector category
			offset += ss::staticVarIntSerialize( data+offset , category_length );
			for (int i = 0 ; i < (int)category_length ; i++){
				offset += category[i].serialize(data+offset);
			}
		}
		offset += fixed.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Getting size of vector category
			size_t _length;
			offset += ss::staticVarIntParse( data+offset , &_length );
			::ss::url::Category _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += ::ss::url::Category::size(data+offset);
			}
		}
		offset += ::ss::system::UInt8::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		if( category_length > 0 ){
		return category[0].hash(max_num_partitions);
		} else return 0;
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // Comparing vector category
			size_t _length1,_length2;
			*offset1 += ss::staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += ss::staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing category[i]
					int tmp = ::ss::url::Category::compare(data1,data2,offset1 , offset2);
					if( tmp != 0) return tmp;
				}
			}
		}
		{ // comparing fixed
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

	void categorySetLength(int _length){
		if( _length > category_max_length){ 
			::ss::url::Category *_previous = category;
			int previous_length = category_length;
			if(category_max_length == 0) category_max_length = _length;
			while(category_max_length < _length) category_max_length *= 2;
			category = new ::ss::url::Category[category_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					category[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		category_length=_length;
	}

	::ss::url::Category* categoryAdd(){
		categorySetLength( category_length + 1 );
		return &category[category_length-1];
	}

	void copyFrom( CategoryVector_base *other ){
			{ // CopyFrom field category
				categorySetLength( other->category_length);
				for (int i = 0 ; i < category_length ; i++){
					category[i].copyFrom(&other->category[i]);
				}
			}
		fixed.copyFrom(&other->fixed);
	};

	std::string str(){
		std::ostringstream o;
		{// toString of vector category
			for(int i = 0 ; i < category_length ; i++){
				o << category[i].str();
				 o << " ";
			}
		}
		o<<" ";
		o << fixed.str();

		o<<" ";
		return o.str();
	}

	}; //class CategoryVector_base

} // end of namespace ss
} // end of namespace url

#endif
