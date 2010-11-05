#ifndef NEWTXTQUEUEDLG_H
#define NEWTXTQUEUEDLG_H

#include <QtGui/QDialog>
#include "ui_NewTXTQueueDlg.h"

class NewTXTQueueDlg : public QDialog
{
    Q_OBJECT

public:
    NewTXTQueueDlg(QWidget* parent=0)
		: QDialog(parent) { init(); };
    NewTXTQueueDlg(QList<QString> names, QWidget* parent=0);
    ~NewTXTQueueDlg() {};

    QString getName() { return name;};
public slots:
	virtual void accept();
    void setName(QString _name);

protected:
    void init();
    bool nameValid(const QString &_name);

protected:
	QList<QString> existing_names;
	QString name;

private:
    Ui::NewTXTQueueDlgClass ui;
};

#endif // NEWTXTQUEUEDLG_H
