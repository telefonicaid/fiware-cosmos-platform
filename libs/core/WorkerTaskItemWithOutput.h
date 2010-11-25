

#ifndef _H_WORKER_TASK_ITEM_WITH_OUTPUTS
#define _H_WORKER_TASK_ITEM_WITH_OUTPUTS

#include "WorkerTaskItem.h"		// ss::WorkerTaskItem
#include <vector>				// std::vector
#include "samson.pb.h"			// ss::network::...

namespace ss {
	

	/**
	 Spetial class to manager all the task items that emit key-values
	 */

	class WorkerTaskItemWithOutput : public WorkerTaskItem
	{
	public:
		std::vector<network::Queue> outputs;			// Queues to emit key-values ( inserted at data manager )
		
		WorkerTaskItemWithOutput( size_t _task_id , size_t _item_id , WorkerTaskItem::Type _type,  const network::WorkerTask &task ) 
		: WorkerTaskItem( _task_id , _item_id , _type )
		{
			// Save the outputs queues
			for (int i = 0 ; i < task.output_size() ; i++)
				outputs.push_back( task.output(i) );
		}
		
		network::Queue getOutputQueue( int o )
		{
			return outputs[o];
		}

		
		
	};
}
#endif


