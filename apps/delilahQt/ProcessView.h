/*
 * SceneView.h
 *
 *  Created on: Oct 20, 2010
 *      Author: ania
 */

#ifndef PROCESSVIEW_H_
#define PROCESSVIEW_H_

#include <QGraphicsView>
#include <QAction>

#include "ProcessScene.h"

class ProcessView : public QGraphicsView
{
	Q_OBJECT

public:
	ProcessView(QWidget* parent=0);
	ProcessView(ProcessScene* scene);
	~ProcessView();

public slots:
	void setWorkingAction(QAction* action);

public:
	QAction* working_action;
	QString work_act;
};

#endif /* PROCESSVIEW_H_ */
