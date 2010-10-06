#ifndef SAMSON_KV_QUEUE_PROCESS_H
#define SAMSON_KV_QUEUE_PROCESS_H

/* ****************************************************************************
*
* FILE            KVQueueProcess.h - automatically process content of a queue
*
*
*
*/
#include <sys/types.h>           /* size_t                                   */
#include <string>                /* std::string                              */
#include <sstream>               /* std::ostringstream                       */



namespace ss
{
	class KVQueueProcess
	{
	public:
		KVQueueProcess( std::string _input_queue , std::string _operation_name , std::vector<std::string> _outputs_queues );
		
		std::string operation_name;					//!< Operation to run		
		std::vector<std::string> input_queues;		//!< Input queues ( queue.process )
		std::vector<std::string> output_queues;		//!< Output queues ( queues )
		
		std::string str()
		{
			std::ostringstream o;
			o << "Operation: " << operation_name << " ";
			
			o << "Inputs: ";
			for ( size_t i = 0 ; i < input_queues.size() ; i++)
				o << input_queues[i] << " ";
			
			o << "Outputs: ";
			for ( size_t i = 0 ; i < output_queues.size() ; i++)
				o << output_queues[i] << " ";
			
			return o.str();
		}
	};
}

#endif
