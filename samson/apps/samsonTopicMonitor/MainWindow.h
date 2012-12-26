/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#include <QtGui>

#include "ui_MainWindow.h"              // Parent class


class MainWindow : public QMainWindow , public Ui_MainWindow
{    
    Q_OBJECT
    
        // Model for the list
    QStandardItemModel model;
	
public:
    
    MainWindow(QWidget *parent = 0);

private slots:
    
    void updateTimer();
    
};


