/*
 * ProcessView.cpp
 *
 *  Created on: Oct 20, 2010
 *      Author: ania
 */

#include "ProcessView.h"

ProcessView::ProcessView(QWidget* parent)
: QGraphicsView(parent)
{
	working_action = NULL;
	work_act = "";
}

ProcessView::ProcessView(ProcessScene* scene)
	: QGraphicsView(scene)
{

}

ProcessView::~ProcessView()
{
}

void ProcessView::setWorkingAction(QAction* action)
{
	working_action = action;
	work_act = action->objectName();
}

