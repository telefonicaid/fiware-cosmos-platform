#ifndef CREATETXTQUEUEDLG_H
#define CREATETXTQUEUEDLG_H

#include <QtGui/QDialog>
#include "ui_CreateTXTQueueDlg.h"

class CreateTXTQueueDlg : public QDialog
{
    Q_OBJECT

public:
    CreateTXTQueueDlg(QWidget* parent=0);
//		: QDialog(parent) {	ui.setupUi(this); };
    ~CreateTXTQueueDlg() {};

    QString name() { return ui.nameLineEdit->text(); };

public slots:
	void cancelError();
	virtual void accept();
//    void setName(QString _name);
//
//protected:
//    bool nameValid(const QString &_name);
//
//protected:
//	QList<QString> existing_names;
//	QString name;

private:
    Ui::CreateTXTQueueDlgClass ui;
    bool show_error;
};

#endif // CREATETXTQUEUEDLG_H
