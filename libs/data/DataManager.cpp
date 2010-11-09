/* ****************************************************************************
*
* FILE                     DataManager.cpp - 
*
* AUTHOR                   Andreu Urruela
*
* CREATION DATE            Nov 8 2010
*
*/
#include "DataManager.h"         // Own interface



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
