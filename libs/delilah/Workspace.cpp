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
	app = (DelilahQtApp*)qApp;
	scene = new WorkspaceScene();

	connect(scene, SIGNAL(removeQueueFromWorkspaceRequested(Queue*)), this, SLOT(removeQueueFromWorkspace(Queue*)));
	connect(scene, SIGNAL(deleteQueueRequested(Queue*)), this, SLOT(deleteQueue(Queue*)));

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

	job_info job;
	job.pos = scene_pos;
	job.status = IN_PROCESSING;
	job.message = "Sending request: create queue ";

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
			job.type = CANCELED;
			QString error = QString("Could not create %1 queue:\n"
					"No information how to handle this type of queue").arg(name);
			emit(unhandledFailure(error));
			break;
	}

	if (job.type!=CANCELED)
	{
		jobs.append(job);
		emit(jobCreated(job));
	}

}

void Workspace::removeQueueFromWorkspace(Queue* queue)
{
	// TODO: remove queue from all processes it is involved in

	scene->removeQueue(queue);
}

void Workspace::deleteQueue(Queue* queue)
{
	job_info job;
//	job.pos = scene_pos;
	job.status = IN_PROCESSING;
	job.message = "Sending request: delete queue";

	switch(queue->type())
	{
		case DATA_QUEUE:
			job.type = DELETE_DATA_QUEUE;
			job.args << queue->getName();
			job.id = app->sendDeleteDataQueue(queue->getName());
			break;
		case KV_QUEUE:
			job.type = DELETE_KV_QUEUE;
			job.args << queue->getName();
			job.id = app->sendDeleteKVQueue(queue->getName());
			break;
		default:
			job.type = CANCELED;
			QString error = QString(
					"Could not delete %1 queue:\nNo information how to handle this type of queue.").arg(queue->getName());
			emit(unhandledFailure(error));
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
				{
					DataQueue* queue = app->getDataQueue(job.args[0]);
					if (queue)
						scene->showDataQueue(queue, job.pos);
					else
					{
						QString error = QString(
								"Creation of %1 queue failed:\n Created queue is not available").arg(job.args[0]);
						emit(unhandledFailure(error));
					}
				}
				break;
			case CREATE_KV_QUEUE:
				break;
			case DELETE_DATA_QUEUE:
				{
					DataQueue* queue = app->getDataQueue(job.args[0], true);
					if (queue)
						scene->removeQueue(queue);
					else
					{
						QString error = QString("Deleting of %1 queue failed:\nQueue is not deleted").arg(job.args[0]);
						emit(unhandledFailure(error));
					}
				}
				break;
			case DELETE_KV_QUEUE:
			case LOAD_FILE:
			case RUN_PROCESS:
			default:
				break;
		}

	}

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
