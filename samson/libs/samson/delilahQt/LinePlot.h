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

#ifndef _H_LinePlot
#define _H_LinePlot

#include <QtGui>
#include <sstream>

#include "Plot.h"
#include "au/string.h"

#include <KDChartChart.h>
#include <KDChartBarDiagram.h>
#include <KDChartLineDiagram.h>
#include <KDChartPieDiagram.h>
#include <KDChartCartesianAxis.h>


class LinePlot: public Plot
{
    // Model for this plot
    QStandardItemModel m_model;
    
    // QFrame used to put the graph in
    QFrame *widget;
    
    
private:
    
    // Diagram ( created in constructor )
    KDChart::LineDiagram* diagram;
    
public:
    
    LinePlot( QFrame *_widget , const char* x_title ,  const char* y_title );
	virtual ~LinePlot();
    
    QVBoxLayout* layout;
    
    // KV Chart
    KDChart::Chart m_chart;
    
	// Set labels for the plot
	virtual void set_legend( std::string title, std::string label);
	virtual void set_legend( std::string title, std::string label_1 , std::string label_2);
	virtual void set_legend( std::string title,  std::vector<std::string>& labels);
    
    virtual void set( int r , int c , double v );
    virtual void clear();
    
};

#endif
