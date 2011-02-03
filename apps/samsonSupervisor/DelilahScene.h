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

class QMenu;
class QGraphicsItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
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

	DelilahScene(QMenu *itemMenu, QObject *parent = 0);

	void           qCreate(void);
	void           connection(void);
	void           remove2(void);
	DelilahQueue*  lookup(QGraphicsItem* gItemP);

private:
	QMenu* myItemMenu;

	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
	virtual void wheelEvent(QGraphicsSceneWheelEvent* wheelEvent);

	void connectionDraw(DelilahQueue* qFromP, DelilahQueue* qToP);
	DelilahQueue* testq1;
	DelilahQueue* testq2;

private slots:
	void bind(void);
	void rename(void);
	void remove(void);
};

#endif
