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

public slots:
	void selectKeyButtonClicked();
	void selectValueButtonClicked();

	void cancelError();

protected:
    void initializeDataTypeTree();
    void showError(QString error);
    QString getSelectedDataTypeName();

private:
    Ui::CreateKVQueueDlgClass ui;

    bool show_error;
    int name_column_number;
};

#endif // CREATEKVQUEUEDLG_H
