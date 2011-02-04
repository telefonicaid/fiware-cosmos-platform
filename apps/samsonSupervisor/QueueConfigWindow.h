#ifndef QUEUE_CONFIG_WINDOW_H
#define QUEUE_CONFIG_WINDOW_H

/* ****************************************************************************
*
* FILE                     QueueConfigWindow.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 04 2011
*
*/
#include <QObject>
#include <QDialog>

class QLineEdit;
class DelilahQueue;



/* ****************************************************************************
*
* QueueConfigWindow - 
*/
class QueueConfigWindow : public QDialog
{
	Q_OBJECT

public:
	QueueConfigWindow(DelilahQueue* queue);

	DelilahQueue* queue;
	QLineEdit*    displayNameInput;
	QLineEdit*    inTypeInput;
	QLineEdit*    outTypeInput;

private slots:
	void die(void);
	void ok(void);
	void save(void);
	void apply(void);
	void cancel(void);
};

#endif
