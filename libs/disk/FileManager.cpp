

#include "FileManager.h" // Own interface


namespace ss
{
	
	static FileManager* sharedFileManager=NULL;
	
	FileManager* FileManager::shared()
	{
		if( !  sharedFileManager )
			sharedFileManager = new FileManager();
		return sharedFileManager;
	}
	
	
}