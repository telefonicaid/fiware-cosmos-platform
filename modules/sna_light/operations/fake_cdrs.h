
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_light_fake_cdrs
#define _H_SAMSON_sna_light_fake_cdrs


#include <samson/Operation.h>


namespace ss{
namespace sna_light{


	class fake_cdrs : public ss::Generator
	{

	public:


		void run( ss::KVWriter *writer )
		{
			  ss::system::UInt id;
			  CDR cdr;
			  int num_nodes = 1000;
			  int num_links_per_node = 30;

			  for (int i = 0 ; i < num_nodes ; i++)
			  {
			    for (int j = 0 ; j < num_links_per_node ; j++)
			    {
			      id.value = i+1;

			      //Get a different link

			      do{
			        cdr.node = (rand()%num_nodes)+1;
			      } while( cdr.node == id.value );  //Make sure we are not autolinked

			      for (int k = 0 ; k < rand()%10 ; k++)
			      {
/*			        cdr.date.day  = rand()%30;
			        cdr.date.month  = rand()%12;
			        cdr.date.year = 10;
			        cdr.date.week_day = rand()%7;

			        cdr.time.hour = rand()%23;
			        cdr.time.minute = rand()%60;
*/

			        cdr.duration = rand()%100;

			        writer->emit(0, &id, &cdr);

			        if( (rand()%10) <= 2)
			          writer->emit(0, &id, &cdr);  //Duplicated


			      }
			    }
			  }
		}


	};


} // end of namespace ss
} // end of namespace sna_light

#endif
