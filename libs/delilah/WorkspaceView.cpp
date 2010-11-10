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

#include "WorkspaceView.h"
#include "Workspace.h"
#include "WorkspaceScene.h"
#include "DelilahQtApp.h"
#include "CreateTXTQueueDlg.h"
#include "Misc.h"

WorkspaceView::WorkspaceView(QWidget* parent)
: QGraphicsView(parent)
{
	workspace = 0;
}

WorkspaceView::WorkspaceView(Workspace* model, QWidget* parent)
	: QGraphicsView(parent)
{
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
	connect(this, SIGNAL(createQueueRequested(QueueType, QPointF, QString, QString, QString)),
			workspace, SLOT(createQueue(QueueType, QPointF, QString, QString, QString)));
}

/*
 * SLOT. Creates context menu to select type of queue for inserting into workspace.
 */
void WorkspaceView::selectQueueType(const QPointF &scene_pos)
{
	QMenu* menu = new QMenu(this);

	ActionWithPos* txt_act = new ActionWithPos("New TXT Queue", this);
	txt_act->setPosition(scene_pos);
	connect(txt_act, SIGNAL(triggered(QPointF)), this, SLOT(createTXTQueueSelected(QPointF)));

	ActionWithPos* kv_act = new ActionWithPos("New KV Queue", this);
	kv_act->setPosition(scene_pos);
	connect(kv_act, SIGNAL(triggered(QPointF)), this, SLOT(createKVQueueSelected(QPointF)));

	menu->addAction("Existing Queue");//, this, SLOT(showAvailableQueues()));
	menu->addAction(txt_act);
	menu->addAction(kv_act);

	// Set menu's left upper corner at clicked position
	QPoint view_pos = mapFromScene(scene_pos);
	menu->exec(mapToGlobal(view_pos));
}

//void WorkspaceView::showAvailableQueues()
//{
//	std::cout << "TODO!!!!!!!!!!!!!!!!!!\n";
//}

void WorkspaceView::createTXTQueueSelected(const QPointF &scene_pos)
{
	CreateTXTQueueDlg* dlg = new CreateTXTQueueDlg(this);

	QString name;
	if (dlg->exec() == QDialog::Accepted)
	{
		name = dlg->name();
		emit(createQueueRequested(TXT_QUEUE, scene_pos, name));
	}

	delete dlg;
}

void WorkspaceView::createKVQueueSelected(const QPointF &scene_pos)
{
	std::cout << "TODO!!!!!!!!!!!!!!!!!!\n";
}
