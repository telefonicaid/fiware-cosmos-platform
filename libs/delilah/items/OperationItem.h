/*
 * OperationItem.h
 *
 *  Created on: Oct 22, 2010
 *      Author: ania
 */

#ifndef OPERATIONITEM_H_
#define OPERATIONITEM_H_

#include "BaseItem.h"
#include "globals.h"

class BaseObject;
class Operation;

class OperationItem : public BaseItem
{
	Q_OBJECT

public:
	OperationItem()
		: BaseItem() {};
	OperationItem(QSvgRenderer* renderer)
		: BaseItem(renderer) {};
	OperationItem(Operation* _operation)
		: BaseItem(), operation(_operation) {};
	~OperationItem() {};

	virtual int type() const { return Type; };
	virtual void initText();

public slots:
	void showInfoSelected();
	void removeOperationSelected();
	void runOperationSelected();

protected:
	virtual void initializeDefaultSize();

	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

signals:
//	void infoRequested(BaseObject*);

public:
    enum { Type=OPERATION_ITEM };
    Operation* operation;


};



#endif /* OPERATIONITEM_H_ */
