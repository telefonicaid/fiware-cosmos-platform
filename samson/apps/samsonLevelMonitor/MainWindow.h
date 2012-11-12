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

#include "Plot.h"                       // Basic plot using KDChart library

class MainWindow : public QMainWindow , public Ui_MainWindow
{    
    Q_OBJECT
    
    Plot* plot;  // Main plot using KVChart
    bool first_legend;
    std::vector<std::string> labels_;
	
public:
    
    MainWindow(QWidget *parent = 0);

private slots:
    
    void updateTimer();
    void update(){ ; }

    
};


