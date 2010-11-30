/*
 * FILE:		InfoBox.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION: Shows information about object (queue or operation)
 *
 */

#ifndef INFOBOX_H
#define INFOBOX_H

#include <QtGui/QDialog>
#include "ui_InfoBox.h"

class InfoBox : public QDialog
{
    Q_OBJECT

public:
    InfoBox(QWidget* parent=0) : QDialog(parent)//, Qt::CustomizeWindowHint)
	{
		ui.setupUi(this);
	};
    ~InfoBox() {};

    void setHtml(QString text="")
	{
    	if (text.isEmpty())
    		text = "<FONT color=red><b>Error</b>: No info available.</FONT>";
    	ui.textEdit->setHtml(text);
	};

private:
    Ui::InfoBoxClass ui;
};

#endif // INFOBOX_H
