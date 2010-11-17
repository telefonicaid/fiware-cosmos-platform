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
	connect(app, SIGNAL(gotCommandResponse(unsigned int, bool, bool, QString)),
			this, SLOT(updateJob(unsigned int, bool, bool, QString)));
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
		case DATA_QUEUE:
			job.type = CREATE_DATA_QUEUE;
			job.args << name;
			job.id = app->sendCreateDataQueue(name);
			break;
		case KV_QUEUE:
			job.type = CREATE_KV_QUEUE;
			job.args << name << key << value;
			job.id = app->sendCreateKVQueue(name, key, value);
			break;
		default:
			// TODO:
			std::cout << "This situation should never happen!!!!!!!!!\n";
			job.type = CANCELED;
			break;
	}

	if (job.type!=CANCELED)
	{
		jobs.append(job);
		emit(jobCreated(job));
	}

}

/*
 * SLOT. Updates job info and emits signal with updated job
 */
void Workspace::updateJob(unsigned int id, bool finished, bool error, QString message)
{
	// check if the job was called from this workspace
	int index = findJobIndex(id);
	if( index < 0 )
		return;

	if(finished)
	{
		finishJob(id, error, message);
	}
	else
	{
		// Update info in jobs list
		job_info job = jobs[index];
		job.message = message;
		emit(jobUpdated(job));
	}
}

/*
 * SLOT.
 */
void Workspace::finishJob(unsigned int id, bool error, QString message)
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

	// Proceed depending on the job status (failed or finished).
	if(job.status==FINISHED)
	{
		// TODO:
		// Depending on the type of the job call appropriate methods to create queues, operation, etc.
		// Update scene appropriately.
		DelilahQtApp* app = (DelilahQtApp*)qApp;
		switch(job.type)
		{
			case CREATE_DATA_QUEUE:
				scene->showDataQueue(app->getDataQueue(job.args[0]), job.pos);
				break;
			case CREATE_KV_QUEUE:
			case REMOVE_QUEUE:
			case LOAD_FILE:
			case RUN_PROCESS:
			default:
				break;
		}

	}
	// TODO:
	//delete job from the list
	emit(jobUpdated(job));
	jobs.removeAt(index);
}


int Workspace::findJobIndex(unsigned int id)
{
	for(int i=0; i<jobs.size(); i++)
	{
		if (jobs[i].id==id)
			return i;
	}
	return -1;
}
