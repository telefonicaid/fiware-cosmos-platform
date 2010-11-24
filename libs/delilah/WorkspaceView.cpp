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
#include "Misc.h"

#include "CreateDataQueueDlg.h"
#include "LoadExistingQueueDlg.h"
#include "ConfirmationDlg.h"
#include "InfoBox.h"


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
	connect(scene(), SIGNAL(deleteQueueRequested(Queue*)), this, SLOT(confirmDeletingQueue(Queue*)));
	connect(scene(), SIGNAL(queueInfoRequested(Queue*)), this, SLOT(showQueueInfo(Queue*)));

	connect(this, SIGNAL(loadQueueRequested(QString, QPointF)),
			workspace, SLOT(loadQueue(QString, QPointF)));
	connect(this, SIGNAL(createQueueRequested(QueueType, QPointF, QString, QString, QString)),
			workspace, SLOT(createQueue(QueueType, QPointF, QString, QString, QString)));
	connect(this, SIGNAL(deleteQueueRequested(Queue*)), workspace, SLOT(deleteQueue(Queue*)));

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

	ActionWithPos* load_existing_act = new ActionWithPos("Load Existing Queue", this);
	load_existing_act->setPosition(scene_pos);
	connect(load_existing_act, SIGNAL(triggered(QPointF)), this, SLOT(loadExistingQueueSelected(QPointF)));

	ActionWithPos* create_data_act = new ActionWithPos("Create Data Queue", this);
	create_data_act->setPosition(scene_pos);
	connect(create_data_act, SIGNAL(triggered(QPointF)), this, SLOT(createDataQueueSelected(QPointF)));

	ActionWithPos* create_kv_act = new ActionWithPos("Create KV Queue", this);
	create_kv_act->setPosition(scene_pos);
	connect(create_kv_act, SIGNAL(triggered(QPointF)), this, SLOT(createKVQueueSelected(QPointF)));

	menu->addAction(load_existing_act);
	menu->addAction(create_data_act);
	menu->addAction(create_kv_act);

	// Set menu's left upper corner at clicked position
	QPoint view_pos = mapFromScene(scene_pos);
	menu->exec(mapToGlobal(view_pos));

	delete load_existing_act;
	delete create_data_act;
	delete create_kv_act;
}

void WorkspaceView::loadExistingQueueSelected(const QPointF &scene_pos)
{
	LoadExistingQueueDlg* dlg = new LoadExistingQueueDlg(this);

	if(dlg->exec() == QDialog::Accepted)
	{
		emit(loadQueueRequested(dlg->queueName(), scene_pos));
	}

	delete dlg;
}

void WorkspaceView::createDataQueueSelected(const QPointF &scene_pos)
{
	CreateDataQueueDlg* dlg = new CreateDataQueueDlg(this);

	QString name;
	if (dlg->exec() == QDialog::Accepted)
	{
		name = dlg->queueName();
		emit(createQueueRequested(DATA_QUEUE, scene_pos, name));
	}

	delete dlg;
}

void WorkspaceView::createKVQueueSelected(const QPointF &scene_pos)
{
	// TODO:
}

void WorkspaceView::confirmDeletingQueue(Queue* queue)
{
	ConfirmationDlg* dlg = new ConfirmationDlg(this);
	dlg->setText(QString("Are you sure you want to delete queue '%1' from the SAMSON platform?").arg(queue->getName()));
	if (dlg->exec() == QDialog::Accepted)
		emit(deleteQueueRequested(queue));

	delete dlg;
}

void WorkspaceView::showQueueInfo(Queue* queue)
{
	QueueInfoBox* box = new QueueInfoBox(this);
	box->setInfo(queue);
	box->exec();
	delete box;
}
