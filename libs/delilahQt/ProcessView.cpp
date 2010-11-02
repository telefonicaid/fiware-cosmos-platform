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
}

ProcessView::ProcessView(ProcessScene* scene)
	: QGraphicsView(scene)
{

}

ProcessView::~ProcessView()
{
}
