
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_UInt
#define _H_SAMSON_system_UInt


namespace samson{
namespace system{


		/** 
		 UInt  Data Instance 
		 */
		
		class UInt : public samson::DataInstance{
			
		public:
			
			size_t value;

			UInt() : samson::DataInstance(){
			}
			
			~UInt() {
			}
			
			int parse(char *data){
				return samson::staticVarIntParse( data , &value);
			}
			
			int serialize(char *data){
				return samson::staticVarIntSerialize( data , value);
			}
			
			int hash(int max_num_partitions){
				return value%max_num_partitions;
			}
			
	
			static int size(char *data){
				size_t _value;
				return samson::staticVarIntParse( data , &_value);
			}

			int toInt()
			{
				return (value);
			}
	
			inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
				{ // comparing b
					size_t _value1;
					size_t _value2;
					*offset1 += samson::staticVarIntParse(data1 + (*offset1), &_value1);
					*offset2 += samson::staticVarIntParse(data2 + (*offset2), &_value2);
					if( _value1 < _value2 ) return -1;
					if( _value1 > _value2 ) return  1;
					return 0;
				}
				return 0; //If everything is equal
			}
			
			inline static int compare( char* data1 , char* data2 )
			{
				size_t offset_1=0;
				size_t offset_2=0;
				return compare( data1 , data2 , &offset_1 , &offset_2 );
			}
			
			void copyFrom( UInt *other ){
				value = other->value;
			};
			
			std::string str(){
				std::ostringstream o;
				o << value;
				return o.str();
			}
			/*
			void operator= (int _value) {
				value = _value;
			}			
			*/
			
			void operator++ () {
				value++;
			}			

			// Comparation with size_t
			
			void operator= (size_t _value) {
				value = _value;
			}			
			
			bool operator< (size_t _value) {
				return ( value < _value);
			}			
			
			bool operator> (size_t _value) {
				return ( value > _value);
			}			

			bool operator<= (size_t _value) {
				return ( value <= _value);
			}			
			
			bool operator>= (size_t _value) {
				return ( value >= _value);
			}			
			
			bool operator== (size_t _value) {
				return ( value == _value);
			}			

			bool operator!= (size_t _value) {
				return ( value != _value);
			}
			
			UInt& operator+=(size_t _value) {
				value+=_value;
				return *this;
			}			
			
			UInt& operator-=(size_t _value) {
				value-=_value;
				return *this;
			}			
			
			
			// Self comparison
			
			UInt& operator+=(const UInt &o) {
				value+=o.value;
				return *this;
			}			
			
			UInt& operator=(const UInt &o) {
				value=o.value;
				return *this;
			}			

			bool operator== (UInt& o) {
				return ( value == o.value);
			}			
			
			bool operator!= (UInt& o) {
				return ( value != o.value);
			}

			bool operator< (UInt& o) {
				return ( value < o.value);
			}

			bool operator> (UInt& o) {
				return ( value > o.value);
			}

			bool operator<= (UInt& o) {
				return ( value <= o.value);
			}

			bool operator>= (UInt& o) {
				return ( value >= o.value);
			}
			
			
		}; 	
	

} // end of namespace samson
} // end of namespace system

#endif
