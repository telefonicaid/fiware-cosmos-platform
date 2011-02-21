
#include "DataManagerItem.h"			// Own interface
#include "DataManager.h"				// ss::DataManager

namespace ss {

	void DataManagerItem::run( DataManager *manager )
	{
		
		for (size_t i = 0 ; i < command.size() ; i++)
			manager->_run( task_id, command[i] );
	}
}