
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_String
#define _H_SAMSON_system_String

#include <samson/modules/system/UInt16.h>
#include <limits>

#define NUM_PARTITIONS 65536

namespace samson{
    namespace sort{
        
        
      class Code : public samson::system::UInt16
	  //        class Code : public samson::DataInstance
	{

            
            typedef unsigned char uchar;
            
        public:
            
            //uchar value;
            
            // Function to get a value depending on the range of size_t    
            void set( size_t _value )
            {
                size_t threshold = std::numeric_limits<std::size_t>::max()/NUM_PARTITIONS;
                value = _value/threshold;
            }
            
            // Particular behavious for sorting                                                                                                                                             
            int partition( int num_partitions )
            {
                return ( num_partitions * value / NUM_PARTITIONS );
            }
            
	    /*

            int parse(char *data){
                value = *((uchar*)data);
                return 1;
            }
            
            int serialize(char *data){
                *((uchar*)data) =  value;
                return 1;
            }
            
            int hash(int max_num_partitions)
            {
                return value%max_num_partitions;
            }
            
            static int size(char *data){
                return 1;
            }
            
            inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 )
            {
                int ans = (*(uchar*)data1) - (*(uchar*)data2);
                *offset1++;
                *offset2++;
                
                return ans;
            }
            
            inline static int compare( char* data1 , char* data2 )
            {
                size_t offset_1=0;
                size_t offset_2=0;
                return compare( data1 , data2 , &offset_1 , &offset_2 );
            }
            
            void copyFrom( Code *other ){
                value = other->value;
            };
            
            std::string str(){
                std::ostringstream output;
                output << value;
                return output.str();
            }
	    */
            
        }; 	
        
        
    } // end of namespace samson
} // end of namespace system

#endif