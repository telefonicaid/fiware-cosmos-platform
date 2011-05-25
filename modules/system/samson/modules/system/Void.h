
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_Void
#define _H_SAMSON_system_Void


namespace samson{
namespace system{


	class Void : public samson::DataInstance{
		
		
	public:
		Void() : samson::DataInstance(){
		}
		
		~Void() {
		}
		
		int parse(char *data){
			return 0;
		}
		
		int serialize(char *data){
			return 0;
		}
		
		int hash(int max_num_partitions){
			return rand()%max_num_partitions;	// This has never to be used unless a random distribution is desired
		}
		
		static int size(char *data){
			return 0;
		}
		
		inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
			return 0; // No unnecessary movement for sorting
		}
		
		inline static int compare( KV* kv1 , KV*kv2 )
		{
			size_t offset_1=0;
			size_t offset_2=0;
			return compare( kv1->key , kv2->key , &offset_1 , &offset_2 );
		}
		
		inline static int compare( char* data1 , char* data2 )
		{
			// No necessary to compare
			return 0;
		}
		
		void copyFrom( Void *other ){
			// Nothing to do here
		};
		
		std::string str(){
			return "";
		}
		
	}; 	

} // end of namespace ss
} // end of namespace system

#endif
