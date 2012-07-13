
#ifndef _H_DELILAH_QT_PLOT
#define _H_DELILAH_QT_PLOT

#include <QtGui>
#include <sstream>

#include "au/RRT.h"
#include "au/string.h"

#include <KDChartChart.h>
#include <KDChartBarDiagram.h>
#include <KDChartLineDiagram.h>
#include <KDChartPieDiagram.h>
#include <KDChartCartesianAxis.h>

#include "ui_QueueWidget.h"


class Plot
{
   QStandardItemModel m_model;
   KDChart::Chart m_chart;

   // Values forsize    
   au::RRT<size_t ,60> samples_last_minute;
   au::RRT<size_t ,60> samples_last_hour;
   au::RRT<size_t ,24> samples_last_day;
    
   size_t samples;

   QGroupBox *box;
    
public:
    
    Plot( QGroupBox *box );
    
    void push( size_t value , bool complete_update );

private:
    
    void set( int r , int c , size_t v );

};

#endif
