
#ifndef _H_DELILAH_QT_PLOT
#define _H_DELILAH_QT_PLOT

#include <QtGui>
#include <sstream>

#include "au/string.h"

#include <KDChartChart.h>
#include <KDChartBarDiagram.h>
#include <KDChartLineDiagram.h>
#include <KDChartPieDiagram.h>
#include <KDChartCartesianAxis.h>


class Plot
{
    // Model for this plot
    QStandardItemModel m_model;
    
    // QFrame used to put the graph in
    QFrame *widget;
    
    
private:
    
    // Diagram ( created in constructor )
    KDChart::LineDiagram* diagram;
    
public:
    
    //Plot( QFrame *_widget , const char* x_title ,  const char* y_title );
	//~Plot() = 0;
    
    QVBoxLayout* layout;
    
    // KV Chart
    KDChart::Chart m_chart;
    
	// Set labels for the plot
	virtual void set_legend( std::string title, std::string label) = 0;
	virtual void set_legend( std::string title, std::string label_1 , std::string label_2) = 0;
	virtual void set_legend( std::string title,  std::vector<std::string>& labels) = 0;
    
    virtual void set( int r , int c , double v ) = 0;
    virtual void clear() = 0;
    
};

#endif
