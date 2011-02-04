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

	QGraphicsView* view;

private slots:
	void qCreate(void);
	void connection(void);
	void qDelete(void);

	void help(void);
	void qView(void);
	void command(void);
	void dataTypes(void);
	void jobs(void);
	void workers(void);
	void env(void);
	void upload(void);
	void download(void);
	void load(void);
	
private:
	DelilahScene*  scene;

	QAction* exitAction;
	QAction* deleteAction;
	QAction* aboutAction;
};

#endif
