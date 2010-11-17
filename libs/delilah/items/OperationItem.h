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

class OperationItem : public ObjectItem
{
public:
	OperationItem()
		: ObjectItem() {};
	OperationItem(QSvgRenderer* renderer)
		: ObjectItem(renderer) {};
	~OperationItem() {};

	virtual int type() const { return Type; };

protected:
	virtual void initializeDefaultSize();

	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

public:
    enum { Type=OPERATION_ITEM };
};



#endif /* OPERATIONITEM_H_ */
