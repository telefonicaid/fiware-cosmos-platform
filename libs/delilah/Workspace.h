/*
 * FILE:		WorkspaceClient.h
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

enum JobTypes { CANCELED, CREATE_TXT_QUEUE, CREATE_KV_QUEUE, LOAD_FILE, RUN_PROCESS };
enum JobStatus { IN_PROCESSING, FINISHED, FAILED };

/*
 * Structure to keep information about status of requests sent to Samson platform
 */
typedef struct
{
	JobTypes type;
	QPointF pos;			// Scene position where user evoke request (optional)
	size_t id;				// Job id (as returned by delilah)
	JobStatus status;		// Current status of the job
	QString message;		// Extra info about the job
} job_info;


class Workspace: public QObject
{
	Q_OBJECT

public:
	Workspace();
	~Workspace() {};

public slots:
	void createQueue(QueueType type, const QPointF &scene_pos, QString name, QString key=QString(), QString value=QString());
	void updateJob(size_t id, bool error, QString message);
	void finishJob(size_t id, bool error, QString message);

protected:
	QList<job_info> jobs;
};



#endif /* WORKSPACE_H_ */
