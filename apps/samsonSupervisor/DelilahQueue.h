#ifndef DELILAH_QUEUE_H
#define DELILAH_QUEUE_H

/* ****************************************************************************
*
* FILE                     DelilahQueue.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include "DelilahSceneItem.h"   // DelilahSceneItem

class DelilahScene;



/* ****************************************************************************
*
* DelilahQueue - 
*/
class DelilahQueue : public DelilahSceneItem
{
public:
	DelilahQueue(DelilahScene* sceneP, const char* imagePath, const char* displayNameP = NULL, int x = 0, int y = 0);
	~DelilahQueue();

	char*  command;
	int    commandIndex;   // for QueueConfigWindow
	void   commandSet(const char* newType, int index);
};

#endif
