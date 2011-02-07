#ifndef DELILAH_SOURCE_H
#define DELILAH_SOURCE_H

/* ****************************************************************************
*
* FILE                     DelilahSource.h
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
* DelilahSource - 
*/
class DelilahSource : public DelilahSceneItem
{
public:
	DelilahSource(DelilahScene* sceneP, const char* imagePath, const char* displayNameP = NULL, int x = 0, int y = 0);
	~DelilahSource();

	void  sourceFileNameSet(const char* newName);
	char* sourceFileName;
	bool  faked;
	int   fakeSize;
};

#endif
