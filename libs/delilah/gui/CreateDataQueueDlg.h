#ifndef CREATEDATAQUEUEDLG_H
#define CREATEDATAQUEUEDLG_H

#include <QtGui/QDialog>
#include "ui_CreateDataQueueDlg.h"

class CreateDataQueueDlg : public QDialog
{
    Q_OBJECT

public:
    CreateDataQueueDlg(QWidget* parent=0);
    ~CreateDataQueueDlg() {};

    QString name() { return ui.nameLineEdit->text(); };

public slots:
	void cancelError();
	virtual void accept();

private:
    Ui::CreateDataQueueDlgClass ui;
    bool show_error;
};

#endif // CREATEDATAQUEUEDLG_H
