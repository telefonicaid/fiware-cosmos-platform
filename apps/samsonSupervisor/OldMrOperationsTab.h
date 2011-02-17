#ifndef OLD_MR_OPERATIONS_TAB_H
#define OLD_MR_OPERATIONS_TAB_H

/* ****************************************************************************
*
* FILE                     OldMrOperationsTab.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
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
* OldMrOperationsTab -
*/
class OldMrOperationsTab : public QWidget
{
	Q_OBJECT

public:
	OldMrOperationsTab(const char* name, QWidget *parent = 0);

	QGraphicsView* view;
	QGraphicsItem* sceneLayer0;
	QGraphicsItem* sceneLayer1;

private slots:
	void qCreate(void);
	void source(void);
	void result(void);
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
