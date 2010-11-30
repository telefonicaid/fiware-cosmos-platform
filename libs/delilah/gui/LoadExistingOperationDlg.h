/*
 * FILE:		LoadExistingOperationDlg.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#ifndef LOADEXISTINGOPERATION_H
#define LOADEXISTINGOPERATION_H

#include "LoadExistingBaseDlg.h"

class LoadExistingOperationDlg : public LoadExistingBaseDlg
{
    Q_OBJECT

public:
    LoadExistingOperationDlg(QWidget* parent=0)
		:LoadExistingBaseDlg(parent)
    {
    	setWindowTitle("Load Operation");
    };
    ~LoadExistingOperationDlg() {};

    virtual void initNameSelectionWidget();
};

#endif // LOADEXISTINGOPERATION_H
