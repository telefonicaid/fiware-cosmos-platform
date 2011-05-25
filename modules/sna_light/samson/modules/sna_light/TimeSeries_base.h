
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_sna_light_TimeSeries_BASE
#define _H_SAMSON_sna_light_TimeSeries_BASE


#include <samson/modules/system/UInt.h>


namespace samson{
namespace sna_light{


	class TimeSeries_base : public samson::DataInstance{

	public:
	::samson::system::UInt linkedId;
	::samson::system::UInt *weights;
	int weights_length;
	int weights_max_length;

	TimeSeries_base() : samson::DataInstance(){
		weights_length=0;
		weights_max_length=0;
		weights = NULL;
	}

	~TimeSeries_base() {
		if( weights )
			delete[] weights ;
	}

	int parse(char *data){
		int offset=0;
		offset += linkedId.parse(data+offset);
		{ //Parsing vector weights
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
		 	weightsSetLength( _length );
			for (int i = 0 ; i < (int)weights_length ; i++){
				offset += weights[i].parse(data+offset);
			}
		}
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += linkedId.serialize(data+offset);
		{ //Serialization vector weights
			offset += samson::staticVarIntSerialize( data+offset , weights_length );
			for (int i = 0 ; i < (int)weights_length ; i++){
				offset += weights[i].serialize(data+offset);
			}
		}
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::samson::system::UInt::size(data+offset);
		{ //Getting size of vector weights
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
			::samson::system::UInt _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += ::samson::system::UInt::size(data+offset);
			}
		}
		return offset;
	}

	int hash(int max_num_partitions){
		return linkedId.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing linkedId
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // Comparing vector weights
			size_t _length1,_length2;
			*offset1 += samson::staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += samson::staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing weights[i]
					int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
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

	void weightsSetLength(int _length){
		if( _length > weights_max_length){ 
			::samson::system::UInt *_previous = weights;
			int previous_length = weights_length;
			if(weights_max_length == 0) weights_max_length = _length;
			while(weights_max_length < _length) weights_max_length *= 2;
			weights = new ::samson::system::UInt[weights_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					weights[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		weights_length=_length;
	}

	::samson::system::UInt* weightsAdd(){
		weightsSetLength( weights_length + 1 );
		return &weights[weights_length-1];
	}

	void copyFrom( TimeSeries_base *other ){
		linkedId.copyFrom(&other->linkedId);
			{ // CopyFrom field weights
				weightsSetLength( other->weights_length);
				for (int i = 0 ; i < weights_length ; i++){
					weights[i].copyFrom(&other->weights[i]);
				}
			}
	};

	std::string str(){
		std::ostringstream o;
		o << linkedId.str();

		o<<" ";
		{// toString of vector weights
			for(int i = 0 ; i < weights_length ; i++){
				o << weights[i].str();
				 o << " ";
			}
		}
		o<<" ";
		return o.str();
	}

	}; //class TimeSeries_base

} // end of namespace samson
} // end of namespace sna_light

#endif
