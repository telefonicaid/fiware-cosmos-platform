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

class BaseItem;
class Queue;
class Process;
class WorkspaceScene;
class DelilahQtApp;

enum JobTypes { CANCELED, CREATE_DATA_QUEUE, CREATE_KV_QUEUE, DELETE_QUEUE,
				UPLOAD_FILE, DOWNLOAD_FILE, RUN_PROCESS };
enum JobStatus { IN_PROCESSING, FINISHED, FAILED };

/*
 * Structure to keep information about status of requests sent to Samson platform
 */
typedef struct
{
	JobTypes type;
	QPointF pos;			// Scene position where user evoke request (optional)
	unsigned int id;		// Job id
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

	void loadQueue(const QString &name, const QPointF &scene_pos);
	void createQueue(QueueType type, const QPointF &scene_pos, QString name, QString key=QString(), QString value=QString());
	void deleteQueue(Queue* queue);
	void uploadToQueue(Queue* queue, QStringList files);

	void loadOperation(const QString &name, const QPointF &scene_pos);
	void removeItem(BaseItem* item);

	void updateJob(unsigned int id, bool finished, bool error, QString message);
	void finishJob(unsigned int id, bool error, QString message);

protected:
	int findJobIndex(unsigned int id);

signals:
	void jobCreated(job_info job);
	void jobUpdated(job_info job);

	void unhandledFailure(QString error);

protected:
	QString name;
	WorkspaceScene* scene;
	DelilahQtApp* app;

	QList<job_info> jobs;

//	QList<Queue*> queues;
	QList<Process*> processes;
};


#endif /* WORKSPACE_H_ */
