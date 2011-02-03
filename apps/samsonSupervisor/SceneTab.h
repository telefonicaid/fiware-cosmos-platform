#ifndef SCENE_TAB_H
#define SCENE_TAB_H

/* ****************************************************************************
*
* FILE                     SceneTab.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QMenu>
#include <QAction>
#include <QString>

#include "DelilahScene.h"        // DelilahScene



/* ****************************************************************************
*
* SceneTab -
*/
class SceneTab : public QWidget
{
	Q_OBJECT

public:
	SceneTab(const char* name, QWidget *parent = 0);

private slots:
	void about(void);

	void qCreate(void);
	void connection(void);
	void qDelete(void);
	void qView(void);
	void command(void);

private:
	QGraphicsView* view;
	DelilahScene*  scene;

	QAction* exitAction;
	QAction* deleteAction;
	QAction* aboutAction;


	QMenu* fileMenu;
	QMenu* itemMenu;
	QMenu* aboutMenu;
};

#endif
