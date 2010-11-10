/*
 * FILE:			WorkspaceView.h
 *
 * AUTHOR:			Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#ifndef WORKSPACEVIEW_H_
#define WORKSPACEVIEW_H_

#include <QGraphicsView>

#include "globals.h"

class Workspace;

class WorkspaceView : public QGraphicsView
{
	Q_OBJECT

public:
	WorkspaceView(QWidget* parent=0);
	WorkspaceView(Workspace* model, QWidget* parent=0);
	~WorkspaceView();

	void setWorkspace(Workspace* model);

public slots:
	// select the type of the queue to be (created and) inserted into workspace
	void selectQueueType(const QPointF &scene_pos);
	void createTXTQueueSelected(const QPointF &scene_pos);
	void createKVQueueSelected(const QPointF &scene_pos);

protected:
//	void showAvailableQueues();

signals:
	void createQueueRequested(QueueType type, const QPointF scene_pos, QString name, QString key=QString(), QString value=QString());

protected:
	Workspace* workspace;
};

#endif /* WORKSPACEVIEW_H_ */
