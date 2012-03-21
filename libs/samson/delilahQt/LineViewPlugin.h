#ifndef _H_LineViewPlugin
#define _H_LineViewPlugin

/* ****************************************************************************
*
* FILE                     LineViewPlugin.h
*
* DESCRIPTION			   Plugin to visualize data coming from the queues in the form of a line graph
*
*/

#include "DataViewPlugin.h"
#include "LinePlot.h"
#include "BarPlot.h"

#include "au/RRT.h"             // au::ValueCollection 

#include <QtGui>
#include <vector>
#include <string>

namespace samson {


class LineViewPlugin: public DataViewPlugin
{
        Q_OBJECT
    public:
        
        LineViewPlugin(QWidget* parent = 0);
        //~LineViewPlugin();

    private:
        
        LinePlot* plot;
        //Data for the plot
        au::ResizableRRT<unsigned long> rateCollection;
        //Plot controls
        QHBoxLayout* plotControlsLayout;
        QPushButton* plotReset;
        QPushButton* plotStop;
        QLabel* plotNSamplesLabel;
        QLineEdit* plotNSamples;
        
        //QGroupBox* ratePlotBox;
        

    public slots:
        virtual void setData(std::string data);
        virtual void redrawPlot();
        virtual void onPlotReset();
        virtual void onPlotNSamplesChanged();
        
};

} //namespace

#endif //_H_LineViewPlugin
