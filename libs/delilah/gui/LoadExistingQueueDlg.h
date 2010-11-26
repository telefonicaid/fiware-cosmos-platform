/*
 * FILE:		LoadExistingQueueDlg.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#ifndef LOADEXISTINGQUEUEDLG_H
#define LOADEXISTINGQUEUEDLG_H

#include <QtGui/QDialog>
#include "ui_LoadExistingQueueDlg.h"

class LoadExistingQueueDlg : public QDialog
{
    Q_OBJECT

public:
    LoadExistingQueueDlg(QWidget* parent=0);
    ~LoadExistingQueueDlg();

    QString queueName() { return ui.nameLineEdit->text(); };

public slots:
	virtual void accept();
	void cancelError();
	void setQueueName(QTreeWidgetItem* item);

protected:
    void initializeQueueTree();

private:
    Ui::LoadExistingQueueDlgClass ui;
    int name_column_number;
    bool show_error;
};

#endif // LOADEXISTINGQUEUEDLG_H
