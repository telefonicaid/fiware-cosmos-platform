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

protected:
    void initializeDataTypeTree();

private:
    Ui::CreateKVQueueDlgClass ui;

    int name_column_number;
};

#endif // CREATEKVQUEUEDLG_H
