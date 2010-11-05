#ifndef NEWTXTQUEUEDLG_H
#define NEWTXTQUEUEDLG_H

#include <QtGui/QDialog>
#include "ui_NewTXTQueueDlg.h"

class NewTXTQueueDlg : public QDialog
{
    Q_OBJECT

public:
    NewTXTQueueDlg(QWidget *parent = 0);
    ~NewTXTQueueDlg();

//public slots:
//	virtual void accept();

signals:
	void nameChanged(QString);

private:
    Ui::NewTXTQueueDlgClass ui;
    QString name;
};

#endif // NEWTXTQUEUEDLG_H
