/*
 * FILE:			WorkspaceView.h
 *
 * AUTHOR:			Anna Wojdel
 *
 * DESCRIPTION:		Widget to display all GUI related to one workspace
 *
 */

#ifndef WORKSPACEVIEW_H_
#define WORKSPACEVIEW_H_

#include <QGraphicsView>

#include "WorkspaceScene.h"
#include "globals.h"

class WorkspaceView : public QGraphicsView
{
	Q_OBJECT

public:
	WorkspaceView(QWidget* parent=0);
	WorkspaceView(WorkspaceScene* scene, QWidget *parent=0);
	~WorkspaceView();

public slots:
	// select the type of the queue to be (created and) inserted into workspace
	void selectQueueType(const QPointF &scene_pos);
	void createTXTQueueSelected(const QPointF &scene_pos);
	void createKVQueueSelected(const QPointF &scene_pos);

	// show info about status of the process
	void showProcessInfo(size_t id, QString message);

protected:
//	void showAvailableQueues();

signals:
	void createQueueRequested(QueueType type, const QPointF scene_pos, QString name, QString key=QString(), QString value=QString());
};

#endif /* WORKSPACEVIEW_H_ */
