#ifndef DELILAH_RESULT_H
#define DELILAH_RESULT_H

/* ****************************************************************************
*
* FILE                     DelilahResult.h
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
* DelilahResult - 
*/
class DelilahResult : public DelilahSceneItem
{
public:
	DelilahResult(DelilahScene* sceneP, const char* imagePath, const char* displayNameP = NULL, int x = 0, int y = 0);
	~DelilahResult();

	char*   inType;
	int     inTypeIndex;    // for ResultConfigWindow
	void    inTypeSet(const char* newType);
};

#endif
