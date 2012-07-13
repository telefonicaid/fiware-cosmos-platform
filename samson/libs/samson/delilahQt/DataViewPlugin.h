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
        //~DataViewPlugin();

    private:
        
    public slots:
        virtual void setData(std::string data) = 0;
        virtual void redrawPlot() = 0;
        virtual void onPlotReset() = 0;
        virtual void onPlotNSamplesChanged() = 0;
        
};

} //namespace

#endif //_H_DataViewPlugin
