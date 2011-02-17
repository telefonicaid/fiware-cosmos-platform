#ifndef MR_OPERATIONS_TAB_H
#define MR_OPERATIONS_TAB_H

/* ****************************************************************************
*
* FILE                     MrOperationsTab.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 17 2011
*
*/
#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMenu>
#include <QAction>
#include <QString>

#include "DelilahScene.h"        // DelilahScene



/* ****************************************************************************
*
* MrOperationsTab -
*/
class MrOperationsTab : public QWidget
{
	Q_OBJECT

public:
	MrOperationsTab(const char* name, QWidget *parent = 0);

	QGraphicsView* view;
	QGraphicsItem* sceneLayer0;
	QGraphicsItem* sceneLayer1;

private slots:
private:
	DelilahScene*  scene;
};

#endif
