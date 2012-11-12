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


// Include header files for application components.
// ...


#include "au/string.h"          // au::str()

#include "PlotWidget.h"
#include "MainWindow.h"

#include "DelilahConnection.h"     // DelilahConnection

extern QApplication *app;
extern MainWindow *mainWindow;      
extern PlotWidget *samsonConnect; 

extern DelilahConnection* delilahConnection; 

#include <KDChartWidget.h>

PlotWidget::PlotWidget( QWidget *parent ) : QWidget(parent)
{



    for (size_t column = 0; column < 5 ; ++column)
    {
	for (size_t row = 0; row < 20 ; ++row) {
	    {
	      std::string value = au::str( rand()%(20+20*column) );

		QStandardItem *item = new QStandardItem( QString( value.c_str() ) );
		m_model.setItem(row, column, item);
	    }
	}
    }
    
  
  
    KDChart::BarDiagram* diagram = new KDChart::BarDiagram;
    diagram->setModel(&m_model);
    diagram->setPen( QPen( Qt::black, 0 ) );

    diagram->setType( KDChart::BarDiagram::Stacked );
    diagram->setOrientation( Qt::Horizontal );

    m_chart.coordinatePlane()->replaceDiagram(diagram);

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(&m_chart);
    setLayout(l);



    
}

