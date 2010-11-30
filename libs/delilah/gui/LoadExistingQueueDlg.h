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

#include "LoadExistingBaseDlg.h"

class LoadExistingQueueDlg : public LoadExistingBaseDlg
{
    Q_OBJECT

public:
    LoadExistingQueueDlg(QWidget* parent=0)
		: LoadExistingBaseDlg(parent)
    {
    	setWindowTitle("Load Queue");
    };
    ~LoadExistingQueueDlg() {};

    virtual void initNameSelectionWidget();
};

#endif // LOADEXISTINGQUEUEDLG_H
