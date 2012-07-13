
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
    
    // KV Chart
    KDChart::Chart m_chart;
    
    // Group Box used to put the graph in
    // We can change the title of this group
    QGroupBox *box;
    
    KDChart::Legend* legend_;
    
private:
    
    // Diagram ( created in constructor )
    KDChart::LineDiagram* diagram;
    
public:
    
    Plot( QGroupBox *_box , const char* x_title ,  const char* y_title );
	~Plot();
    
	// Set labels for the plot
	void set_legend( std::string title, std::string label);
	void set_legend( std::string title, std::string label_1 , std::string label_2);
	void set_legend( std::string title,  std::vector<std::string>& labels);
    
    void set( int r , int c , double v );
    void clear();
    
};

#endif
