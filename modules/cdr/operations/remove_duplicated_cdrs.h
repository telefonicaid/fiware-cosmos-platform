
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_cdr_sna_remove_duplicated_cdrs
#define _H_SAMSON_cdr_sna_remove_duplicated_cdrs


#include <samson/Operation.h>


namespace ss{
namespace cdr{


	class remove_duplicated_cdrs : public ss::Reduce
	{

		ss::system::UInt node;
		ss::cdr::CDR cdr1;
		ss::cdr::CDR cdr2;
		
		ss::cdr::CDR *current_CDR;
		ss::cdr::CDR *previous_CDR;	
		
	public:


		void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
		{
			
			if( inputs[0].num_kvs == 0)
				return;
			
			//Parse and emit the first one...
			node.parse(inputs[0].kvs[0]->key );
			cdr1.parse( inputs[0].kvs[0]->value );
			writer->emit(0,&node, &cdr1);	
			
			
			if( inputs[0].num_kvs < 2)	//One is never repeated
				return;
			
			current_CDR		= &cdr2;
			previous_CDR	= &cdr1;
			
			for (size_t i=1 ; i < inputs[0].num_kvs ; i++)
			{
				current_CDR->parse(inputs[0].kvs[i]->value);
				
				//Compare with the previous one
				if(  *current_CDR != *previous_CDR )
					writer->emit( 0 , &node, current_CDR);
				
				//Exchange of pointers
				ss::cdr::CDR *tmp = current_CDR;
				current_CDR = previous_CDR;
				previous_CDR = tmp;
			}			
		}


	};


} // end of namespace ss
} // end of namespace cdr

#endif
