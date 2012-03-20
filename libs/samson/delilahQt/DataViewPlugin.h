#ifndef _H_DataViewPlugin
#define _H_DataViewPlugin

/* ****************************************************************************
*
* FILE                     DataViewPlugin.h
*
* DESCRIPTION			   Plugins to visualize data coming from the queues
*
*/

#include "Plot.h"

#include "au/RRT.h"             // au::ValueCollection 

#include <QtGui>
#include <vector>
#include <string>

namespace samson {


class DataViewPlugin: public QFrame
{
        Q_OBJECT
    public:
        
        DataViewPlugin(QWidget* parent = 0);
        ~DataViewPlugin();

    private:
        
        Plot* plot;
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
        void setData(std::string data);
        void redrawPlot();
        void onPlotReset();
        void onPlotNSamplesChanged();
        
};

} //namespace

#endif //_H_DataViewPlugin
