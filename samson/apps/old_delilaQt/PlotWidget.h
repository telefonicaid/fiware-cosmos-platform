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
#include <iostream>
#include <KDChartChart.h>
#include <KDChartBarDiagram.h>

// Include header files for application components.
// ...


#include <assert.h>

class PlotWidget : public QWidget
{
    Q_OBJECT
	
	KDChart::Chart m_chart;
    QStandardItemModel m_model;
    
public:
    
    PlotWidget(QWidget *parent = 0);
    
private slots:
    
    
};

