/*
 * FILE:		InfoBox.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#ifndef INFOBOX_H
#define INFOBOX_H

#include <QtGui/QDialog>
#include "ui_InfoBox.h"

class Queue;

/*
 * Abstract class for showing some information.
 */
class InfoBox : public QDialog
{
    Q_OBJECT

public:
    InfoBox(QWidget* parent=0);
    ~InfoBox() {};

    // Methods to generate text with information and to display it.
    // The text format to display information is HTML.
    // setInfo() method should create appropriate QString with text in HTML
    // and later call setHtml() to display this text.
    // setHtml() method is a simple rapper that allows child classes to
    // set the text.
    virtual void setInfo() = 0;
    void setHtml(const QString text)
	{
    	ui.textEdit->setHtml(text);
	};

private:
    Ui::InfoBoxClass ui;
};


/*
 *
 */
class QueueInfoBox : public InfoBox
{
public:
	QueueInfoBox(QWidget* parent=0)
		: InfoBox(parent), queue(0)
	{
		setWindowTitle("Queue Info");
	};
	QueueInfoBox(Queue* _queue, QWidget* parent=0)
		: InfoBox(parent)
	{
		queue=_queue;
		setWindowTitle("Queue Info");
	};
	~QueueInfoBox() {};

	virtual void setInfo();
	virtual void setInfo(Queue* _queue);
private:
	Queue* queue;
};

#endif // INFOBOX_H
