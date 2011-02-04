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
#include "DelilahQueue.h"       // DelilahQueue
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
void ConnectionMgr::insert(DelilahScene* sceneP, DelilahQueue* from, DelilahQueue* to)
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
DelilahConnection* ConnectionMgr::lookup(DelilahScene* sceneP, DelilahQueue* from, DelilahQueue* to)
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
void ConnectionMgr::remove(DelilahQueue* queue)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if ((conV[ix]->qFromP != queue) && (conV[ix]->qToP != queue))
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
void ConnectionMgr::move(DelilahQueue* queue)
{
	unsigned int ix;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if ((conV[ix]->qFromP != queue) && (conV[ix]->qToP != queue))
			continue;

		conV[ix]->move();
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
*/
int ConnectionMgr::outgoingConnections(DelilahQueue* from)
{
	unsigned int  ix;
	int           connections = 0;

	for (ix = 0; ix < size; ix++)
	{
		if (conV[ix] == NULL)
			continue;

		if (conV[ix]->qFromP == from)
			++connections;
	}

	return connections;
}



/* ****************************************************************************
*
* ConnectionMgr::incomingConnections - 
*/
int ConnectionMgr::incomingConnections(DelilahQueue* to)
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
