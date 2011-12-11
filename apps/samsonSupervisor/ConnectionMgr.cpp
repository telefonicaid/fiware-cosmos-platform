/* ****************************************************************************
*
* FILE                     ConnectionMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Popup.h"              // Popup
#include "DelilahScene.h"       // DelilahScene
#include "DelilahSceneItem.h"   // DelilahSceneItem
#include "DelilahConnection.h"  // DelilahConnection
#include "ConnectionMgr.h"      // Own interface



/* ****************************************************************************
*
* ConnectionMgr
*/
ConnectionMgr::ConnectionMgr(unsigned int size)
{
	this->size = size;

	conV = (DelilahConnection**) calloc(size, sizeof(DelilahConnection*));
	if (conV == NULL)
		LM_X(1, ("error allocating room for %d delilah connections", size));
}



/* ****************************************************************************
*
* ConnectionMgr::connections - 
*/
int ConnectionMgr::connections(void)
{
	unsigned int  ix;
	int           connectionNo = 0;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] != NULL)
			++connectionNo;
	}

	return connectionNo;
}



/* ****************************************************************************
*
* ConnectionMgr::insert - 
*/
void ConnectionMgr::insert(DelilahScene* sceneP, DelilahSceneItem* from, DelilahSceneItem* to)
{
	unsigned int  ix;
	int           conIx = -1;

	if (from == to)
	{
		char eText[256];

		snprintf(eText, sizeof(eText), "Can't connect to yourself, can you?\nFucking Idiot!");
		new Popup("Meathead", eText);
		return;
	}



	if (lookup(sceneP, from, to) != NULL)
	{
		char eText[256];

		snprintf(eText, sizeof(eText), "The queues '%s' and '%s' are already connected.", from->displayName, to->displayName);
		new Popup("Connection already exists", eText);
		return;
	}

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
		{
			conIx = ix;
			break;
		}
	}

	if (conIx == -1)
	{
		LM_TODO(("realloc connection vector ..."));
		new Popup("Out of connections", "Sorry, no room for more connections.\nTo fix this, you need to contact the developer (kzangeli@tid.es).");

		return;
	}

	conV[conIx] = new DelilahConnection(sceneP, from, to);
}



/* ****************************************************************************
*
* ConnectionMgr::lookup - 
*/
DelilahConnection* ConnectionMgr::lookup(DelilahScene* sceneP, DelilahSceneItem* from, DelilahSceneItem* to)
{
	unsigned int  ix;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if (conV[ix]->scene != sceneP)
			continue;

		if ((conV[ix]->qFromP == from) && (conV[ix]->qToP == to))
			return conV[ix];

		if ((conV[ix]->qFromP == to) && (conV[ix]->qToP == from))
			return conV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* ConnectionMgr::lookup - 
*/
DelilahConnection* ConnectionMgr::lookup(QGraphicsItem* lineItem)
{
	unsigned int  ix;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if (conV[ix]->lineItem == lineItem)
			return conV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* ConnectionMgr::remove - 
*/
void ConnectionMgr::remove(DelilahSceneItem* si)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if ((conV[ix]->qFromP != si) && (conV[ix]->qToP != si))
			continue;

		delete conV[ix];
		conV[ix] = NULL;
	}
}



/* ****************************************************************************
*
* ConnectionMgr::remove - 
*/
void ConnectionMgr::remove(DelilahConnection* connection)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] != connection)
			continue;

		delete conV[ix];
		conV[ix] = NULL;
	}
}



/* ****************************************************************************
*
* ConnectionMgr::move - 
*/
void ConnectionMgr::move(DelilahSceneItem* si)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if ((conV[ix]->qFromP != si) && (conV[ix]->qToP != si))
			continue;

		conV[ix]->move();
	}
}



/* ****************************************************************************
*
* ConnectionMgr::setOpacity - 
*/
void ConnectionMgr::setOpacity(DelilahSceneItem* si, float opacity)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if ((conV[ix]->qFromP != si) && (conV[ix]->qToP != si))
			continue;

		conV[ix]->setOpacity(opacity);
	}
}



/* ****************************************************************************
*
* removeAll - 
*/
void ConnectionMgr::removeAll(void)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		delete conV[ix];
		conV[ix] = NULL;
	}
}



/* ****************************************************************************
*
* ConnectionMgr::outgoingConnections - 
*
* Make sure outV has enough room ...
*/
int ConnectionMgr::outgoingConnections(DelilahSceneItem* from, DelilahConnection** outV)
{
	unsigned int  ix;
	int           connections = 0;
	
	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if (conV[ix]->qFromP == from)
		{
			if (outV != NULL)
				outV[connections] = conV[ix];
			++connections;
		}
	}

	return connections;
}



/* ****************************************************************************
*
* ConnectionMgr::incomingConnections - 
*/
int ConnectionMgr::incomingConnections(DelilahSceneItem* to)
{
	unsigned int  ix;
	int           connections = 0;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if (conV[ix]->qToP == to)
			++connections;
	}

	return connections;
}