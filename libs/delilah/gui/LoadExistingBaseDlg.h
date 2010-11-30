/*
 * FILE:		LoadExistingBaseDlg.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#ifndef LOADEXISTINGBASE_H_
#define LOADEXISTINGBASE_H_

#include <QDialog>
#include "ui_LoadExistingBaseDlg.h"

class LoadExistingBaseDlg : public QDialog
{
    Q_OBJECT

public:
    LoadExistingBaseDlg(QWidget* parent=0);
    ~LoadExistingBaseDlg();

    QString getName() { return ui.nameLineEdit->text(); };

    virtual void initNameSelectionWidget() = 0;

public slots:
	virtual void accept();
	void cancelError();
	void setName(QTreeWidgetItem* item);

protected:
    void showError(QString error);

protected:
    Ui::LoadExistingBaseDlgClass ui;

    bool error_visible;
    unsigned int name_column_number;
};


#endif /* LOADEXISTINGBASE_H_ */
