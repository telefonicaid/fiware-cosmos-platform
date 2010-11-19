/*
 * FILE:		WorkspaceView.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include <iostream>

#include <QMenu>
#include <QMessageBox>

#include "WorkspaceView.h"
#include "WorkspaceScene.h"
#include "DelilahQtApp.h"
#include "CreateTXTQueueDlg.h"
#include "Misc.h"

WorkspaceView::WorkspaceView(QWidget* parent)
: QGraphicsView(parent)
{
	workspace = 0;
	status_view = 0;
}

WorkspaceView::WorkspaceView(Workspace* model, QWidget* parent)
	: QGraphicsView(parent)
{
	status_view = 0;
	setWorkspace(model);
}

WorkspaceView::~WorkspaceView()
{
}

void WorkspaceView::setWorkspace(Workspace* model)
{
	workspace = model;
	setScene(workspace->getScene());

	connect(scene(), SIGNAL(addQueueRequested(QPointF)), this, SLOT(selectQueueType(QPointF)));
	connect(this, SIGNAL(createQueueRequested(QueueType, QPointF, QString, QString, QString)),
			workspace, SLOT(createQueue(QueueType, QPointF, QString, QString, QString)));
	connect(workspace, SIGNAL(jobCreated(job_info)), this, SLOT(updateJobInfoView(job_info)));
	connect(workspace, SIGNAL(jobUpdated(job_info)), this, SLOT(updateJobInfoView(job_info)));
	connect(workspace, SIGNAL(unhandledFailure(QString)), this, SLOT(showError(QString)));
}

void WorkspaceView::showError(QString error)
{
	QMessageBox::critical(this, QString("Failure"), error);
}

void WorkspaceView::updateJobInfoView(job_info job)
{
	if(status_view==0)
	{
		status_view = new QPlainTextEdit(this);
		status_view->setReadOnly(true);
		status_view->show();
	}

	// Create description of the updated job
	QString new_info;
	if(job.status==IN_PROCESSING)
		 new_info = QString("job %1: %2").arg(job.id).arg(job.message);

	// Find if job is new or not
	int found = -1;
	int i;
	for (i=0; i<status.size(); i++)
	{
		if(status[i].contains( QString("job %1").arg(job.id)) )
		{
			found = i;
			break;
		}
	}

	// Update status list. If it's a new job, add it at the end.
	// If the job has 'FINISHED' status remove it from the list.
	// Otherwise update status info.
	if ( found==-1 )
		status.append(new_info);
	else
	{
		if (job.status==FINISHED || job.status==FAILED)
			status.removeAt(found);
		else
			status.replace(found, new_info);
	}

	// Set text to be displayed
	QString info;
	if (status.size()>0)
	{
		info.append("RUNNING JOBS\n");
		for(i=0; i<status.size(); i++)
			info.append(status[i] + "\n");
	}

	// Display updated jobs or if there are now jobs running remove status widget
	if (!info.isEmpty())
		status_view->setPlainText(info);
	else
	{
		delete status_view;
		status_view = 0;
	}

	// If updated job failed, show info to the user
	if (job.status==FAILED)
		QMessageBox::critical(this, QString("Job Failure"), job.message);
}

/*
 * SLOT. Creates context menu to select type of queue for inserting into workspace.
 */
void WorkspaceView::selectQueueType(const QPointF &scene_pos)
{
	QMenu* menu = new QMenu(this);

	ActionWithPos* txt_act = new ActionWithPos("New TXT Queue", this);
	txt_act->setPosition(scene_pos);
	connect(txt_act, SIGNAL(triggered(QPointF)), this, SLOT(createTXTQueueSelected(QPointF)));

	ActionWithPos* kv_act = new ActionWithPos("New KV Queue", this);
	kv_act->setPosition(scene_pos);
	connect(kv_act, SIGNAL(triggered(QPointF)), this, SLOT(createKVQueueSelected(QPointF)));

	menu->addAction("Existing Queue");//, this, SLOT(showAvailableQueues()));
	menu->addAction(txt_act);
	menu->addAction(kv_act);

	// Set menu's left upper corner at clicked position
	QPoint view_pos = mapFromScene(scene_pos);
	menu->exec(mapToGlobal(view_pos));
}

//void WorkspaceView::showAvailableQueues()
//{
//	std::cout << "TODO!!!!!!!!!!!!!!!!!!\n";
//}

void WorkspaceView::createTXTQueueSelected(const QPointF &scene_pos)
{
	CreateTXTQueueDlg* dlg = new CreateTXTQueueDlg(this);

	QString name;
	if (dlg->exec() == QDialog::Accepted)
	{
		name = dlg->name();
		emit(createQueueRequested(DATA_QUEUE, scene_pos, name));
	}

	delete dlg;
}

void WorkspaceView::createKVQueueSelected(const QPointF &scene_pos)
{
	std::cout << "TODO!!!!!!!!!!!!!!!!!!\n";
}
