#ifndef DELILAH_SCENE_H
#define DELILAH_SCENE_H

/* ****************************************************************************
*
* FILE                     DelilahScene.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QObject>
#include <QGraphicsScene>

class QGraphicsItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QGraphicsSceneContextMenuEvent;
class QAction;
class DelilahQueue;



/* ****************************************************************************
*
* DelilahScene - 
*/
class DelilahScene : public QGraphicsScene
{
	Q_OBJECT

public:
	enum Mode { InsertItem, InsertLine, InsertText, MoveItem };

	DelilahScene(QObject *parent = 0);

	void           qCreate(void);
	void           connection(void);
	DelilahQueue*  lookup(QGraphicsItem* gItemP);
	void           setCursor(const char* cursor);

private:
	QAction* removeAction;
	QAction* renameAction;
	QAction* configAction;
	QAction* bindAction;
	QAction* clearAction;
	QAction* queueAddAction;
	QAction* aboutAction;

	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
	virtual void wheelEvent(QGraphicsSceneWheelEvent* wheelEvent);
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* contextMenuEvent);

public slots:
	void remove(void);
	void removeFromMenu(void);
	void config(void);
	void bind();
	void rename();
	void clear(void);
	void queueAdd(void);
	void about(void);
};

#endif
