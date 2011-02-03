#ifndef DELILAH_CONNECTION_H
#define DELILAH_CONNECTION_H

/* ****************************************************************************
*
* FILE                     DelilahConnection.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 03 2011
*
*/
#include <QGraphicsLineItem>

#include "DelilahScene.h"      // DelilahScene
#include "DelilahQueue.h"      // DelilahQueue



/* ****************************************************************************
*
* DelilahConnection - 
*/
class DelilahConnection
{
public:
	DelilahConnection(DelilahScene* sceneP, DelilahQueue* from, DelilahQueue* to);
	~DelilahConnection();

	void move(void);

	DelilahScene*       scene;
	DelilahQueue*       qFromP;
	DelilahQueue*       qToP;
	QGraphicsLineItem*  lineItem;
};

#endif
