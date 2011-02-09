/* ****************************************************************************
*
* FILE                     SceneItemMgr.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Popup.h"              // Popup
#include "DelilahSceneItem.h"   // DelilahSceneItem
#include "SceneItemMgr.h"       // Own interface



/* ****************************************************************************
*
* SceneItemMgr
*/
SceneItemMgr::SceneItemMgr(unsigned int size)
{
	this->size = size;

	itemV = (DelilahSceneItem**) calloc(size, sizeof(DelilahSceneItem*));
	if (itemV == NULL)
		LM_X(1, ("error allocating room for %d delilah scene items", size));
}



/* ****************************************************************************
*
* SceneItemMgr::items - 
*/
int SceneItemMgr::items(void)
{
	unsigned int  ix;
	int           itemNo = 0;

	for (ix = 0; ix < size; ix++)
	{
		if (itemV[ix] != NULL)
			++itemNo;
	}

	return itemNo;
}



/* ****************************************************************************
*
* SceneItemMgr::insert - 
*/
void SceneItemMgr::insert(DelilahSceneItem* item)
{
	unsigned int  ix;
	int           itemIx = -1;

	for (ix = 0; ix < size; ix++)
	{
		if (itemV[ix] == NULL)
		{
			itemIx = ix;
			break;
		}
	}

	if (itemIx == -1)
	{
		LM_TODO(("realloc item vector ..."));
		new Popup("Out of items", "Sorry, no room for more items.\nTo fix this, you need to contact the developer (kzangeli@tid.es).");

		return;
	}

	LM_T(LmtSceneItemMgr, ("Inserting item '%s'", item->displayName));
	itemV[itemIx] = item;
}



/* ****************************************************************************
*
* SceneItemMgr::remove - 
*/
void SceneItemMgr::remove(DelilahSceneItem* item)
{
	unsigned int ix;

	LM_T(LmtSceneItemMgr, ("Trying to remove item '%s'", item->displayName));
	for (ix = 0; ix < size; ix++)
	{
		if (itemV[ix] != item)
			continue;

		LM_T(LmtSceneItemMgr, ("Removing item '%s'", item->displayName));
		delete itemV[ix];
		itemV[ix] = NULL;
	}
}



/* ****************************************************************************
*
* lookup - 
*/
DelilahSceneItem* SceneItemMgr::lookup(QGraphicsItem* gItemP)
{
	unsigned int ix;

	if (gItemP == NULL)
		return NULL;

	for (ix = 0; ix < size; ix++)
	{
		if (itemV[ix] == NULL)
			continue;

		if ((QGraphicsItem*) itemV[ix]->pixmapItem == gItemP)
		   return itemV[ix];
		if ((QGraphicsItem*) itemV[ix]->nameItem == gItemP)
		   return itemV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* removeAll - 
*/
void SceneItemMgr::removeAll(void)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (itemV[ix] == NULL)
			continue;

		delete itemV[ix];
		itemV[ix] = NULL;
	}
}



/* ****************************************************************************
*
* move - 
*/
void SceneItemMgr::move(int x, int y)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (itemV[ix] == NULL)
			continue;

		LM_T(LmtMove, ("Calling moveTo for '%s'", itemV[ix]->displayName));
		itemV[ix]->moveTo(x, y);
	}
}
