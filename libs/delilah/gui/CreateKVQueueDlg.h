/*
 * FILE:		CreateKVQueueDlg.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#ifndef CREATEKVQUEUEDLG_H
#define CREATEKVQUEUEDLG_H

#include <QtGui/QDialog>
#include "ui_CreateKVQueueDlg.h"

class CreateKVQueueDlg : public QDialog
{
    Q_OBJECT

public:
    CreateKVQueueDlg(QWidget *parent = 0);
    ~CreateKVQueueDlg();

    QString queueName() { return ui.nameLineEdit->text(); };
    QString keyType() { return ui.keyLineEdit->text(); };
    QString valueType() { return ui.valueLineEdit->text(); };

public slots:
	void selectKeyButtonClicked();
	void selectValueButtonClicked();

	void cancelError();

protected:
    void initializeDataTypeTree();
    void showError(QString error);
    QString getSelectedDataTypeName();

    virtual void accept();

private:
    Ui::CreateKVQueueDlgClass ui;

    bool error_visible;
    int name_column_number;
};

#endif // CREATEKVQUEUEDLG_H
