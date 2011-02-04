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
class QGraphicsPixmapItem;
class QGraphicsSimpleTextItem;
class QGraphicsLineItem;
class QPixmap;
class QMenu;
class QAction;

class DelilahScene;



/* ****************************************************************************
*
* DelilahQueue - 
*/
class DelilahQueue
{
public:
	DelilahQueue(DelilahScene* sceneP, const char* imagePath, const char* displayNameP = NULL, int x = 0, int y = 0);
	~DelilahQueue();

	void                     moveTo(int x, int y);
	void                     displayNameSet(const char* newName);
	void                     inTypeSet(const char* newType);
	void                     outTypeSet(const char* newType);
	void                     nameCenter(void);

	DelilahScene*            scene;
	QGraphicsPixmapItem*     pixmapItem;
	QGraphicsSimpleTextItem* nameItem;
	char*                    displayName;
	DelilahQueue*            neighbor;
	char*                    outType;
	char*                    inType;
	int                      xpos;
	int                      ypos;
	char*                    name;

private:
	QPixmap*                 pixmap;
	QMenu*                   menu;
	QAction*                 renameAction;
	QAction*                 deleteAction;
	QAction*                 bindAction;   // Change mousepointer and bindqueue to the next selected by mouse
};

#endif
