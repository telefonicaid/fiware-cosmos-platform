/*
 * OperationItem.h
 *
 *  Created on: Oct 22, 2010
 *      Author: ania
 */

#ifndef OPERATIONITEM_H_
#define OPERATIONITEM_H_

#include "ObjectItem.h"
#include "globals.h"

class Operation;

class OperationItem : public ObjectItem
{
public:
	OperationItem()
		: ObjectItem() {};
	OperationItem(QSvgRenderer* renderer)
		: ObjectItem(renderer) {};
	OperationItem(Operation* _operation)
		: ObjectItem(), operation(_operation) {};
	~OperationItem() {};

	virtual int type() const { return Type; };
	virtual void initText();

protected:
	virtual void initializeDefaultSize();

	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

public:
    enum { Type=OPERATION_ITEM };
    Operation* operation;

};



#endif /* OPERATIONITEM_H_ */
