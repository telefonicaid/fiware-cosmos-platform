#ifndef DELILAH_TAB_H
#define DELILAH_TAB_H

#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>



/* ****************************************************************************
*
* DelilahTab -
*/
class DelilahTab : public QWidget
{
	Q_OBJECT

public:
	DelilahTab(const char* name, QWidget *parent = 0);

private slots:
	void send();

private:
	QGridLayout*  mainLayout;
	QLineEdit*    input;
	QPushButton*  sendButton;
	QTextEdit*    output;
};

#endif
