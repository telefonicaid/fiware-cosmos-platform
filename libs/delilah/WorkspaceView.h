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
#include <QPlainTextEdit>

#include "globals.h"

#include "Workspace.h"

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
	void createDataQueueSelected(const QPointF &scene_pos);
	void createKVQueueSelected(const QPointF &scene_pos);
	void loadExistingQueueSelected(const QPointF &scene_pos);

	void confirmDeletingQueue(Queue* queue);
	void showQueueInfo(Queue* queue);

	void showError(QString error);

	/*
	 * Slot responsible for displaying information about running jobs.
	 */
	void updateJobInfoView(job_info job);

signals:
	void loadQueueRequested(const QString &name, const QPointF scene_pos);
	void createQueueRequested(QueueType type, const QPointF scene_pos, QString name, QString key=QString(), QString value=QString());
	void deleteQueueRequested(Queue* queue);

protected:
	Workspace* workspace;

	QList<QString> status;
	QPlainTextEdit* status_view;
};

#endif /* WORKSPACEVIEW_H_ */
