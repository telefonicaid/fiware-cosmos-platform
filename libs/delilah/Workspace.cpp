/*
 * FILE:		Workspace.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include <cassert>
#include <iostream>

#include "Workspace.h"
#include "WorkspaceScene.h"
#include "Queue.h"
#include "globals.h"
#include "DelilahQtApp.h"

Workspace::Workspace(QString _name)
{
	name = _name;
	scene = new WorkspaceScene();

	DelilahQtApp* app = (DelilahQtApp*)qApp;
	connect(app, SIGNAL(jobUpdated(size_t, bool, QString)), this, SLOT(updateJob(size_t, bool, QString)));
	connect(app, SIGNAL(jobFinished(size_t, bool, QString)), this, SLOT(finishJob(size_t, bool, QString)));
}

void Workspace::setTool(int tool)
{
	scene->setTool(tool);
}


/*
 * SLOT. Sends request to application to create new queue (TXT or KV) on Samson platform.
 */
void Workspace::createQueue(QueueType type, const QPointF &scene_pos, QString name, QString key, QString value)
{
	assert(!name.isEmpty());
	if (type==KV_QUEUE)
	{
		assert(!key.isEmpty() && !value.isEmpty());
	}

	DelilahQtApp* app = (DelilahQtApp*)qApp;

	job_info job;
	job.pos = scene_pos;
	job.status = IN_PROCESSING;
	job.message = "Sending request";

	switch(type)
	{
		case TXT_QUEUE:
			job.type = CREATE_TXT_QUEUE;
			job.id = app->sendCreateQueue(name);
			break;
		case KV_QUEUE:
			job.type = CREATE_KV_QUEUE;
			job.id = app->sendCreateQueue(name, key, value);
			break;
		default:
			std::cout << "This situation should never happen!!!!!!!!!\n";
			job.type = CANCELED;
			break;
	}

	if (job.type!=CANCELED)
		jobs.append(job);

	sleep(5);
	app->receivedMessage(1, false, false, "PROCESSING");
	sleep(5);
	app->receivedMessage(1, false, true, "FINISHED");
}

/*
 * SLOT. Updates job info and emits signal with updated job
 */
void Workspace::updateJob(size_t id, bool error, QString message)
{
	// TODO:
	// Update info in jobs list
	// emit signal that job was updated.
	// This signal should be received by WorkspaceView to update job status
	// to be shown to user

	std::cout << message.toStdString() << "\n";
}

/*
 * SLOT.
 */
void Workspace::finishJob(size_t id, bool error, QString message)
{
	// TODO:
	// Find job in the list. If it doesn't belongs to this workspace, return.
	// Otherwise close job in jobs list (remove it?) and proceed depending
	// on the job status (failed or finished)

	std::cout << message.toStdString() << "\n";
	if(error)
	{
		// TODO:
		// emit signal about failure. This should be received by WorkspaceView widget
		// which shows failure info to user
		std::cout << "job " << id << " finished with error: " << message.toStdString();
//		emit(jobFailed(id, message));
	}
	else
	{
		// TODO:
		// Depending on the type of the job call appropriate methods to create queues, operation, etc.
		// Update scene appropriately.

		// It's for testing - change it!!!!!!!!!
		Queue* q = new Queue;
		scene->showQueue(q);
	}
}
