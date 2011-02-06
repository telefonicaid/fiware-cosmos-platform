#ifndef DELILAH_SCENE_ITEM_H
#define DELILAH_SCENE_ITEM_H

/* ****************************************************************************
*
* FILE                     DelilahSceneItem.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 05 2011
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
* DelilahSceneItem
*/
class DelilahSceneItem
{
public:
	enum Type
	{
		Source,
		Queue,
		Result,
		Connection
	};

	DelilahSceneItem(Type type, DelilahScene* sceneP, const char* imagePath, const char* displayNameP = NULL, int x = 0, int y = 0);
	~DelilahSceneItem();

	void                     moveTo(int x, int y);
	void                     displayNameSet(const char* newName);
	void                     nameCenter(void);

	Type                     type;
	DelilahScene*            scene;
	QGraphicsPixmapItem*     pixmapItem;
	QGraphicsSimpleTextItem* nameItem;
	char*                    displayName;
	int                      xpos;
	int                      ypos;
	char*                    name;
	QPixmap*                 pixmap;

	char*   inType;
	int     inTypeIndex;    // for QueueConfigWindow
	void    inTypeSet(const char* newType);

	char*   outType;
	int     outTypeIndex;   // for QueueConfigWindow
	void    outTypeSet(const char* newType);

private:
	QMenu*                   menu;
	QAction*                 renameAction;
	QAction*                 deleteAction;
	QAction*                 bindAction;   // Change mousepointer and bind queue to the next selected by mouse
};

#endif
