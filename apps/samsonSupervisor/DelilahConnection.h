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
#include "DelilahSceneItem.h"  // DelilahSceneItem



/* ****************************************************************************
*
* DelilahConnection - 
*/
class DelilahConnection
{
public:
	DelilahConnection(DelilahScene* sceneP, DelilahSceneItem* from, DelilahSceneItem* to);
	~DelilahConnection();

	void move(void);

	DelilahScene*       scene;
	DelilahSceneItem*   qFromP;
	DelilahSceneItem*   qToP;
	QGraphicsLineItem*  lineItem;
};

#endif
