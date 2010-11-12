/*
 * FILE:		Workspace.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#ifndef WORKSPACE_H_
#define WORKSPACE_H_

#include <QObject>
#include <QPointF>

#include "globals.h"

class Queue;
class Process;
class WorkspaceScene;

enum JobTypes { CANCELED, CREATE_TXT_QUEUE, CREATE_KV_QUEUE, LOAD_FILE, RUN_PROCESS };
enum JobStatus { IN_PROCESSING, FINISHED, FAILED };

/*
 * Structure to keep information about status of requests sent to Samson platform
 */
typedef struct
{
	JobTypes type;
	QPointF pos;			// Scene position where user evoke request (optional)
	size_t id;				// Job id
	QList<QString> args;	// Command line arguments
	JobStatus status;		// Current status of the job
	QString message;		// Extra info about the job
} job_info;


class Workspace: public QObject
{
	Q_OBJECT

public:
	Workspace(QString _name="");
	~Workspace() {};

	WorkspaceScene* getScene() { return scene; };

public slots:
	void setTool(int tool);
	void createQueue(QueueType type, const QPointF &scene_pos, QString name, QString key=QString(), QString value=QString());
	void updateJob(size_t id, bool error, QString message);
	void finishJob(size_t id, bool error, QString message);

protected:
	int findJobIndex(size_t id);

signals:
	void jobCreated(job_info job);
	void jobUpdated(job_info job);
	void jobFinished(job_info job);
//	void jobFailed(size_t id, QString message);

protected:
	QString name;
	QList<job_info> jobs;
	QList<Queue*> queues;
	QList<Process*> processes;
	WorkspaceScene* scene;
};


#endif /* WORKSPACE_H_ */
