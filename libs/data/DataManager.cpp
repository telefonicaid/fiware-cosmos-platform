
#import "DataManager.h"

namespace ss {

	
	void DataManagerItem::un_run( DataManager *manager )
	{
		int command_size = command.size();
		
		if( command_size > 0)
		{
			for (int i = command_size-1 ; i>=0 ; i--)
				manager->_un_run( command[i] );
		}
	}

}