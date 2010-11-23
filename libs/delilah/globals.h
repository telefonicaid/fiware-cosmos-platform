/*
 * globals.h
 *
 *  Created on: Oct 20, 2010
 *      Author: ania
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <QGraphicsItem>

#define TOOL_SELECT			(0)
#define TOOL_NEWQUEUE		(1)
#define TOOL_NEWOPERATION	(2)
#define TOOL_CONNECT		(3)

#define QUEUE_ITEM			QGraphicsItem::UserType+1
#define OPERATION_ITEM		QGraphicsItem::UserType+2

enum QueueType { DATA_QUEUE, KV_QUEUE };

#define CREATE_DATA_QUEUE_COMMAND		"add_data_queue"
#define CREATE_KV_QUEUE_COMMAND			"add_queue"
#define REMOVE_QUEUE_COMMAND			"remove_queue"

#endif /* GLOBALS_H_ */
