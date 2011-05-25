
#include "DataManagerItem.h"			// Own interface
#include "samson/data/DataManager.h"				// samson::DataManager

namespace samson {

	void DataManagerItem::run( DataManager *manager )
	{
		
		for (size_t i = 0 ; i < command.size() ; i++)
			manager->_run( task_id, command[i] );
	}
}