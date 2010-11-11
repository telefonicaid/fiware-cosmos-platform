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


/*
 * SLOT. Set selected tool.
 */
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
	{
		jobs.append(job);
		emit(jobCreated(job));
	}

	sleep(2);
	app->receivedMessage(1, false, false, "PROCESSING");
}

/*
 * SLOT. Updates job info and emits signal with updated job
 */
void Workspace::updateJob(size_t id, bool error, QString message)
{
	int index = findJobIndex(id);
	if( index < 0 )
		return;

	// TODO:
	// Update info in jobs list
	job_info job = jobs[index];
	job.message = message;
	if(error)
		job.status = FAILED;

	std::cout << message.toStdString() << "\n";
	// emit signal that job was updated.
	// This signal should be received by WorkspaceView to update job status
	// to be shown to user
	emit(jobUpdated(job));

	sleep(2);
	DelilahQtApp* app = (DelilahQtApp*)qApp;
	app->receivedMessage(1, true, true, "FINISHED");
}

/*
 * SLOT.
 */
void Workspace::finishJob(size_t id, bool error, QString message)
{
	// TODO:

	int index = findJobIndex(id);
	if( index < 0 )
		return;

	job_info job = jobs[index];
	if (error)
		job.status = FAILED;
	else
		job.status = FINISHED;
	job.message = message;
	emit(jobFinished(job));

	// Proceed depending on the job status (failed or finished).
	if(job.status==FINISHED)
	{
		// TODO:
		// Depending on the type of the job call appropriate methods to create queues, operation, etc.
		// Update scene appropriately.
		// Update job status in WorkspaceView - remove if finished

		// It's for testing - change it!!!!!!!!!
		Queue* q = new Queue;
		scene->showQueue(q);
	}

	// TODO:
	//delete job from the list
	jobs.removeAt(index);
}


int Workspace::findJobIndex(size_t id)
{
	for(int i=0; i<jobs.size(); i++)
	{
		if (jobs[i].id==id)
			return i;
	}
	return -1;
}
